/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2025 Dominik Reichl <dominik.reichl@t-online.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "StdAfx.h"
#include <mmsystem.h>

#pragma warning(push)
#pragma warning(disable: 4201) // Non-standard extension: nameless structure/union
#include <winioctl.h>
#pragma warning(pop)

#ifdef _UNICODE
#include <atlbase.h>
#endif

#include "WinUtil.h"
#include "CmdLine/Executable.h"
#include "SprEngine/SprEngine.h"
#include "PrivateConfigEx.h"
#include "AppLocator.h"
#include "../Resource.h"
#include "../PwSafeDlg.h"
#include "../Plugins/KpApiImpl.h"
#include "../Plugins/PluginMgr.h"
#include "../NewGUI/NewGUICommon.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"
#include "../../KeePassLibCpp/Util/EntryUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "../../KeePassLibCpp/Util/PwUtil.h"
#include "../../KeePassLibCpp/PwStructsEx.h"
#include "Deprecated.h"                                                           // rrvt

#include <boost/scoped_array.hpp>
#include <boost/static_assert.hpp>
#include <iostream>

using boost::scoped_array;

static unsigned char g_shaLastString[32];
// static LPCTSTR g_lpChildWindowText = NULL;

static int g_nAppHelpSource = APPHS_LOCAL;

static bool g_bStdInPw = false;
static std::basic_string<TCHAR> g_strStdInPw;

static BOOL g_bClipNoPersist = TRUE;

#ifndef _WIN32_WCE

#ifdef _UNICODE
#define CF_TTEXTEX CF_UNICODETEXT
#else
#define CF_TTEXTEX CF_TEXT
#endif

bool WU_OpenClipboard(HWND hWndNewOwner)
{
  // https://referencesource.microsoft.com/#system.windows.forms/winforms/managed/system/winforms/Clipboard.cs
  const DWORD dwRetries = 15; // Default in .NET is 10
  const DWORD dwDelay = 100;

  for(DWORD i = 0; i < dwRetries; ++i)
  {
    if(OpenClipboard(hWndNewOwner) != FALSE) return true;
    Sleep(dwDelay);
  }

  ASSERT(FALSE);
  return false;
}

bool WU_SetClipboardData(UINT uFormat, LPCVOID lpData, SIZE_T cbData)
{
  if(lpData == NULL) { ASSERT(FALSE); return false; }

  HGLOBAL h = GlobalAlloc(GHND, cbData);
  if(h == NULL) { ASSERT(FALSE); return false; }

  LPVOID lp = GlobalLock(h);
  if(lp == NULL) { ASSERT(FALSE); GlobalFree(h); return false; }

  memcpy(lp, lpData, cbData);
  GlobalUnlock(h);

  if(SetClipboardData(uFormat, h) == NULL)
  {
    ASSERT(FALSE);
    GlobalFree(h);
    return false;
  }

  return true;
}

void CopyStringToClipboard(const TCHAR *lptString, PW_ENTRY *pEntryContext,
  CPwManager *pDatabaseContext, HWND hWnd)
{
  if(!WU_OpenClipboard(hWnd)) return;
  if(EmptyClipboard() == FALSE) { ASSERT(FALSE); return; }

  if(lptString == NULL) // No string to copy => empty clipboard only
  {
    VERIFY(CloseClipboard());
    return;
  }

  CString strData = SprCompile(lptString, false, pEntryContext,
    pDatabaseContext, false, false);

  size_t cbData = static_cast<size_t>(strData.GetLength()) * sizeof(TCHAR);
  if(cbData == 0) // No string to copy => empty clipboard only
  {
    VERIFY(CloseClipboard());
    return;
  }
  cbData += sizeof(TCHAR); // Plus null terminator of string

  WU_SetClipboardIgnoreFormats();
  WU_SetClipboardData(CF_TTEXTEX, (LPCTSTR)strData, cbData);
  VERIFY(CloseClipboard());

  RegisterOwnClipboardData((unsigned char *)(LPCTSTR)strData,
    static_cast<unsigned long>(cbData - sizeof(TCHAR)));
}

void RegisterOwnClipboardData(unsigned char* pData, unsigned long dwDataSize)
{
  ASSERT(pData != NULL); if(pData == NULL) return;

  sha256_ctx shactx;
  sha256_begin(&shactx);

  if(dwDataSize > 0)
    sha256_hash(pData, dwDataSize, &shactx);

  sha256_end(g_shaLastString, &shactx);
}

void ClearClipboardIfOwner()
{
  if(!WU_OpenClipboard(NULL)) return;

  if((IsClipboardFormatAvailable(CF_TEXT) == FALSE) &&
    (IsClipboardFormatAvailable(CF_OEMTEXT) == FALSE))
  {
    CloseClipboard();
    return;
  }

  HANDLE hClipboardData = GetClipboardData(CF_TTEXTEX);
  if(hClipboardData == NULL) { CloseClipboard(); return; }

  TCHAR *lpString = (TCHAR *)GlobalLock(hClipboardData);
  if(lpString == NULL) { CloseClipboard(); return; }

  sha256_ctx shactx;
  unsigned char uHash[32];
  sha256_begin(&shactx);
  sha256_hash((unsigned char *)lpString, (unsigned long)(_tcslen(lpString) *
    sizeof(TCHAR)), &shactx);
  sha256_end(uHash, &shactx);

  GlobalUnlock(hClipboardData);

  // If we have copied the string to the clipboard, delete it
  if(memcmp(uHash, g_shaLastString, 32) == 0)
  {
    VERIFY(EmptyClipboard());
  }

  VERIFY(CloseClipboard());
}

// Thanks to Gabe Martin for the contribution of the following
// two secure clipboard functions!
// https://sourceforge.net/p/keepass/patches/6/

BOOL MakeClipboardDelayRender(HWND hOwner, HWND *phNextCB)
{
  if(!WU_OpenClipboard(hOwner)) return FALSE;

  // Add a clipboard listener to the cb chain so we can block any listeners from
  // knowing we are adding sensitive data to the clipboard
  if(phNextCB != NULL)
    if(*phNextCB == NULL)
      *phNextCB = SetClipboardViewer(hOwner);

  EmptyClipboard();
  WU_SetClipboardIgnoreFormats();
  SetClipboardData(CF_TTEXTEX, NULL);
  CloseClipboard();

  return TRUE;
}

void CopyDelayRenderedClipboardData(const TCHAR *lptString, CPwManager *pReferenceSource)
{
  ASSERT(lptString != NULL); if(lptString == NULL) return;

  WU_SetClipboardIgnoreFormats();

  CString strData = SprCompile(lptString, false, NULL, pReferenceSource,
    false, false);
  const size_t cb = static_cast<size_t>(strData.GetLength()) * sizeof(TCHAR);

  // Put the delayed clipboard data in the clipboard
  WU_SetClipboardData(CF_TTEXTEX, (LPCTSTR)strData, cb + sizeof(TCHAR));

  RegisterOwnClipboardData((unsigned char *)(LPCTSTR)strData,
    static_cast<unsigned long>(cb));
}

void WU_SetClipboardIgnoreFormats()
{
  UINT uID = RegisterClipboardFormat(CFN_CLIPBOARD_VIEWER_IGNORE);
  if(uID != 0)
  {
    if(AU_IsWin9xSystem() != FALSE)
    {
      std::basic_string<char> str = _StringToAnsiStl(PWM_PRODUCT_NAME_SHORT);
      WU_SetClipboardData(uID, str.c_str(), (str.size() + 1) * sizeof(char));
    }
    else
    {
      std::basic_string<WCHAR> str = _StringToUnicodeStl(PWM_PRODUCT_NAME_SHORT);
      WU_SetClipboardData(uID, str.c_str(), (str.size() + 1) * sizeof(WCHAR));
    }
  }
  else { ASSERT(FALSE); }

  if(g_bClipNoPersist != FALSE)
  {
    BOOST_STATIC_ASSERT(sizeof(DWORD) == 4);
    BOOST_STATIC_ASSERT(static_cast<DWORD>(FALSE) == 0);
    BOOST_STATIC_ASSERT(static_cast<DWORD>(TRUE) == 1);
    const DWORD dwFalse = 0;
    const DWORD dwTrue = 1;

    uID = RegisterClipboardFormat(CFN_MONITORPROC_EXCL);
    // The value type is not defined/documented; we store a BOOL/DWORD
    if(uID != 0) WU_SetClipboardData(uID, &dwTrue, sizeof(DWORD));
    else { ASSERT(FALSE); }

    uID = RegisterClipboardFormat(CFN_CLOUD_ALLOWED);
    if(uID != 0) WU_SetClipboardData(uID, &dwFalse, sizeof(DWORD));
    else { ASSERT(FALSE); }

    uID = RegisterClipboardFormat(CFN_HISTORY_ALLOWED);
    if(uID != 0) WU_SetClipboardData(uID, &dwFalse, sizeof(DWORD));
    else { ASSERT(FALSE); }
  }
}

#endif

#ifdef _UNICODE
#define PRPT_API_NAME "PathRelativePathToW"
#else
#define PRPT_API_NAME "PathRelativePathToA"
#endif

#ifndef _WIN32_WCE
CString MakeRelativePathEx(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile)
{
  LPPATHRELATIVEPATHTO lpRel;
  HINSTANCE hShl;
  TCHAR tszPath[MAX_PATH * 2];
  BOOL bResult = FALSE;
  CString str;
  BOOL bMod;

  if((lpBaseFile[1] == _T(':')) && (lpTargetFile[1] == _T(':')) &&
    (lpBaseFile[2] == _T('\\')) && (lpTargetFile[2] == _T('\\')) &&
    (lpBaseFile[0] != lpTargetFile[0]))
  {
    return CString(lpTargetFile);
  }
  else if((lpTargetFile[0] == _T('\\')) && (lpTargetFile[1] == _T('\\')))
  {
    return CString(lpTargetFile);
  }

  hShl = AU_LoadLibrary(_T("ShlWApi.dll"));
  if(hShl == NULL) return CString(lpTargetFile);

  lpRel = (LPPATHRELATIVEPATHTO)GetProcAddress(hShl, PRPT_API_NAME);
  if(lpRel != NULL)
  {
    bResult = lpRel(tszPath, lpBaseFile, 0, lpTargetFile, 0);
  }
  FreeLibrary(hShl); hShl = NULL;
  if(bResult == FALSE) return CString(lpTargetFile);

  str = tszPath;
  while(1) // Remove all .\\ from the left of the path
  {
    bMod = FALSE;

    if(str.Left(2) == _T(".\\"))
    {
      str = str.Right(str.GetLength() - 2);
      bMod = TRUE;
    }

    if(bMod == FALSE) break;
  }

  if(bResult == TRUE) return str;
  else return CString(lpTargetFile);
}
#else
CString MakeRelativePathEx(LPCTSTR lpBaseFile, LPCTSTR lpTargetFile)
{
  return CString(lpTargetFile);
}
#endif

CString GetShortestAbsolutePath(LPCTSTR lpFilePath)
{
  CString str;
  if(lpFilePath == NULL) { ASSERT(FALSE); return str; }

  DWORD dwBufLen = static_cast<DWORD>(_tcslen(lpFilePath) + 12);
  if(dwBufLen <= MAX_PATH) dwBufLen = MAX_PATH + 1;

  LPTSTR lpBuf = new TCHAR[dwBufLen + 1];
  ZeroMemory(lpBuf, (dwBufLen + 1) * sizeof(TCHAR));

  DWORD dw = GetFullPathName(lpFilePath, dwBufLen, lpBuf, NULL);

  if((dw != 0) && (dw <= dwBufLen)) str = lpBuf;
  else { ASSERT(FALSE); str = lpFilePath; }

  SAFE_DELETE_ARRAY(lpBuf);
  return str;
}

#pragma warning(push)
#pragma warning(disable: 4996) // _tcscpy deprecated

BOOL GetRegKeyEx(HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpRetData)
{
  if(lpRetData == NULL) { ASSERT(FALSE); return FALSE; }

  HKEY h = NULL;
  LONG lRet = RegOpenKeyEx(hKey, lpSubKey, 0, KEY_QUERY_VALUE, &h);

  if(lRet == ERROR_SUCCESS)
  {
    const DWORD ccAlloc = MAX_PATH * 4;

    TCHAR tszData[ccAlloc];
    ZeroMemory(tszData, ccAlloc * sizeof(TCHAR));
    LONG cbData = static_cast<LONG>((ccAlloc - 2) * sizeof(TCHAR));

    lRet = RegQueryValue(h, NULL, tszData, &cbData);
    if(lRet == ERROR_SUCCESS) _tcscpy(lpRetData, tszData);
    else *lpRetData = _T('\0');

    VERIFY(RegCloseKey(h) == ERROR_SUCCESS);
  }
  else *lpRetData = _T('\0');

  return ((lRet == ERROR_SUCCESS) ? TRUE : FALSE);
}

#pragma warning(pop)

bool WU_GetRegStrPriv(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName,
  REGSAM rsAdd, std::basic_string<TCHAR>& strOut)
{
  REGSAM rs = (KEY_QUERY_VALUE | rsAdd);
  HKEY h = NULL;

  if(RegOpenKeyEx(hKey, lpSubKey, 0, rs, &h) != ERROR_SUCCESS)
  {
    strOut.clear();
    return false;
  }

  DWORD ccAlloc = MAX_PATH * 4 + 2;
  DWORD cbData = (ccAlloc - 2) * sizeof(TCHAR);
  DWORD dwDataType = 0;

  scoped_array<TCHAR> aData(new TCHAR[ccAlloc]);
  ZeroMemory(aData.get(), ccAlloc * sizeof(TCHAR));

  const LONG l = RegQueryValueEx(h, lpValueName, NULL, &dwDataType,
    (LPBYTE)aData.get(), &cbData);
  bool bRet = true;

  if(l == ERROR_SUCCESS) strOut = aData.get();
  else if(l == ERROR_MORE_DATA)
  {
    if(cbData > 0xFFFFFFU) { ASSERT(FALSE); bRet = false; }
    else
    {
      ccAlloc = (cbData / sizeof(TCHAR)) + 3;

      aData.reset(new TCHAR[ccAlloc]);
      ZeroMemory(aData.get(), ccAlloc * sizeof(TCHAR));

      if(RegQueryValueEx(h, lpValueName, NULL, &dwDataType,
        (LPBYTE)aData.get(), &cbData) == ERROR_SUCCESS)
        strOut = aData.get();
      else { ASSERT(FALSE); bRet = false; }
    }
  }
  else bRet = false;

  VERIFY(RegCloseKey(h) == ERROR_SUCCESS);
  if(!bRet) { strOut.clear(); return false; }

  if((dwDataType != REG_EXPAND_SZ) && (dwDataType != REG_SZ))
  {
    ASSERT(FALSE);
    strOut.clear();
    return false;
  }

  return true;
}

std::basic_string<TCHAR> WU_GetRegStr(HKEY hKey, LPCTSTR lpSubKey,
  LPCTSTR lpValueName)
{
  return WU_GetRegStrEx(hKey, lpSubKey, lpValueName, WU_REG_ALL);
}

std::basic_string<TCHAR> WU_GetRegStrEx(HKEY hKey, LPCTSTR lpSubKey,
  LPCTSTR lpValueName, DWORD dwFlags)
{
  std::basic_string<TCHAR> str;

  const bool b32 = ((dwFlags & WU_REG_32) != 0);
  const bool b64 = ((dwFlags & WU_REG_64) != 0);

  if(!b32 && !b64)
    WU_GetRegStrPriv(hKey, lpSubKey, lpValueName, 0, str);
  else
  {
    if(b64)
    {
      if(WU_GetRegStrPriv(hKey, lpSubKey, lpValueName, KEY_WOW64_64KEY, str))
        return str;
    }

    if(b32)
    {
      if(WU_GetRegStrPriv(hKey, lpSubKey, lpValueName, KEY_WOW64_32KEY, str))
        return str;
    }
  }

  return str;
}

BOOL OpenUrlInNewBrowser(LPCTSTR lpURL)
{
  ASSERT(lpURL != NULL); if(lpURL == NULL) return FALSE;

  TCHAR tszKey[MAX_PATH * 4];
  ZeroMemory(tszKey, MAX_PATH * 4 * sizeof(TCHAR));
  UINT uResult = 0;

  if(GetRegKeyEx(HKEY_CLASSES_ROOT, _T("http\\shell\\open\\command"),
    tszKey) == TRUE)
  {
    TCHAR *pos = _tcsstr(tszKey, _T("\"%1\""));
    if(pos == NULL) // No quotes found
    {
      pos = _tcsstr(tszKey, _T("%1")); // Check for %1, without quotes
      if(pos != NULL) *pos = _T('\0'); // Remove the parameter
    }
    else *pos = _T('\0'); // Remove the parameter

    std::basic_string<TCHAR> strExec = tszKey;

    if(pos == NULL) strExec += _T(" ");
    strExec += lpURL;

    uResult = TWinExec(strExec.c_str(), KPSW_SHOWDEFAULT);
  }

  return ((uResult > 31) ? TRUE : FALSE);
}

BOOL OpenUrlUsingPutty(LPCTSTR lpURL, LPCTSTR lpUser)
{
  ASSERT(lpURL != NULL); if(lpURL == NULL) return FALSE;
  CString strURL = WU_ExpandEnvironmentVars(lpURL).c_str();

  strURL = strURL.TrimLeft(_T(" \t\r\n"));

  BOOL bResult = FALSE;

  if(strURL.Find(_T("ssh:")) == 0)
  {
    TCHAR tszKey[MAX_PATH << 1];
    _tcscpy_s(tszKey, _T("PUTTY.EXE -ssh "));

    // Parse out the "http://" and "ssh://"
    if(strURL.Find(_T("http://")) == 0)
      strURL = strURL.Right(strURL.GetLength() - (int)_tcslen(_T("http://")));

    strURL = strURL.Right(strURL.GetLength() - (int)_tcslen(_T("ssh:")));
    if(strURL.Left(1) == _T("/"))
      strURL = strURL.Right(strURL.GetLength() - 1);
    if(strURL.Left(1) == _T("/"))
      strURL = strURL.Right(strURL.GetLength() - 1);

    // Add pre-URL command-line parameters
    if(lpUser != NULL)
    {
      if(_tcslen(lpUser) > 0)
      {
        _tcscat_s(tszKey, lpUser);
        _tcscat_s(tszKey, _T("@"));
      }
    }

    // Add the URL
    _tcscat_s(tszKey, (LPCTSTR)strURL);

    // Execute the ssh client
    bResult = ((TWinExec(tszKey, KPSW_SHOWDEFAULT) > 31) ? TRUE : FALSE);
  }
  else if(strURL.Find(_T("telnet:")) == 0)
  {
    TCHAR tszKey[MAX_PATH << 1];
    _tcscpy_s(tszKey, _T("PUTTY.EXE "));

    // Parse out the "http://" and "telnet://"
    if(strURL.Find(_T("http://")) == 0)
      strURL = strURL.Right(strURL.GetLength() - (int)_tcslen(_T("http://")));

    strURL = strURL.Right(strURL.GetLength() - (int)_tcslen(_T("telnet:")));
    if(strURL.Left(1) == _T("/"))
      strURL = strURL.Right(strURL.GetLength() - 1);
    if(strURL.Left(1) == _T("/"))
      strURL = strURL.Right(strURL.GetLength() - 1);

    // Add the url
    _tcscat_s(tszKey, _T("telnet://"));
    _tcscat_s(tszKey, strURL.GetBuffer(0));

    // Execute the ssh client
    bResult = ((TWinExec(tszKey, KPSW_SHOWDEFAULT) > 31) ? TRUE : FALSE);
  }

  return bResult;
}

// If hParent is not NULL, the function will show an error message if
// the URL cannot be opened
void OpenUrlEx(LPCTSTR lpURL, HWND hParent)
{
  ASSERT(lpURL != NULL); if(lpURL == NULL) return;
  if(lpURL[0] == 0) return; // Valid, but nothing to do

  std::basic_string<TCHAR> strURL = WU_ExpandEnvironmentVars(lpURL);

  CPrivateConfigEx cfg(FALSE);
  BOOL bPrevMethod = cfg.GetBool(PWMKEY_HTMURLMETHOD, FALSE);
  if(bPrevMethod == FALSE)
  {
    OpenUrlShellExec(strURL.c_str(), hParent);
    return;
  }

  if(_tcsncmp(strURL.c_str(), _T("http://"), 7) == 0)
  {
    if(OpenUrlInNewBrowser(strURL.c_str()) == FALSE)
      ShellExecute(NULL, NULL, strURL.c_str(), NULL, NULL, KPSW_SHOWDEFAULT);
  }
  else if(_tcsncmp(strURL.c_str(), _T("https://"), 8) == 0)
  {
    if(OpenUrlInNewBrowser(strURL.c_str()) == FALSE)
      ShellExecute(NULL, NULL, strURL.c_str(), NULL, NULL, KPSW_SHOWDEFAULT);
  }
  else if(_tcsncmp(strURL.c_str(), _T("cmd://"), 6) == 0)
  {
    if(_tcslen(strURL.c_str()) > 6)
    {
      TWinExec(&strURL.c_str()[6], KPSW_SHOWDEFAULT);
    }
  }
  else if(_tcsncmp(strURL.c_str(), _T("\\\\"), 2) == 0)
  {
    if(_tcslen(strURL.c_str()) > 2)
    {
      TWinExec(&strURL.c_str()[2], KPSW_SHOWDEFAULT);
    }
  }
  else ShellExecute(NULL, NULL, strURL.c_str(), NULL, NULL, KPSW_SHOWDEFAULT);
}

// Internal function
void OpenUrlShellExec(LPCTSTR lpURL, HWND hParent)
{
  ASSERT(lpURL != NULL); if(lpURL == NULL) return;

  CString strURL = lpURL;
  strURL = strURL.TrimLeft(_T(" \t\r\n"));
  if(strURL.GetLength() == 0) return;

  CString strLower = strURL;
  strLower = strLower.MakeLower();

  if(strLower.Left(6) == _T("cmd://"))
    OpenUrlProcess(strURL.Right(strURL.GetLength() - 6), hParent);
  else if(strLower.Left(2) == _T("\\\\"))
    OpenUrlProcess(strURL, hParent);
  else // Standard method
    WU_SysExecute(strURL, NULL, hParent);
}

// Internal function
void OpenUrlProcess(LPCTSTR lpURL, HWND hParent)
{
  ASSERT(lpURL != NULL); if(lpURL == NULL) return;

  CString strLine = lpURL, strFile, strParam;
  BOOL bFile = FALSE, bParam = FALSE;

  strLine = strLine.TrimLeft(_T(" \t\r\n"));

  if(strLine.Left(1) == _T("\""))
  {
    int nSecond = strLine.Find(_T('\"'), 1);
    if(nSecond >= 1)
    {
      strFile = strLine.Mid(1, nSecond - 1).Trim();
      bFile = TRUE;

      strParam = strLine.Right(strLine.GetLength() - (nSecond + 1)).Trim();
      bParam = TRUE;
    }
  }

  if(bFile == FALSE)
  {
    int nSpace = strLine.Find(_T(' '));

    if(nSpace >= 0)
    {
      strFile = strLine.Left(nSpace);
      bFile = TRUE;

      strParam = strLine.Right(strLine.GetLength() - nSpace).Trim();
      bParam = TRUE;
    }
    else { strFile = strLine; bFile = TRUE; }
  }

  if((bParam == TRUE) && (strParam.GetLength() > 0))
    WU_SysExecute(strFile, strParam, hParent);
  else
    WU_SysExecute(strFile, NULL, hParent);
}

void WU_SysExecute(LPCTSTR lpFile, LPCTSTR lpParameters, HWND hParent)
{
  ASSERT(lpFile != NULL); if(lpFile == NULL) return;
  ASSERT(lpFile[0] != 0); if(lpFile[0] == 0) return;

  SHELLEXECUTEINFO sei;
  ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

  sei.cbSize = sizeof(SHELLEXECUTEINFO);
  sei.fMask = SEE_MASK_FLAG_NO_UI; // | SEE_MASK_NOZONECHECKS
  sei.hwnd = hParent;
  sei.lpFile = lpFile;
  sei.lpParameters = lpParameters;
  sei.nShow = SW_SHOW;

  const DWORD dwDummyErr = 0x19B5A28F;
  const DWORD dwPrevErr = GetLastError();
  SetLastError(dwDummyErr);

  if(ShellExecuteEx(&sei) == FALSE)
  {
    const DWORD dwErr = GetLastError();
    if(dwErr == dwDummyErr) { SetLastError(dwPrevErr); return; }

    std::basic_string<TCHAR> strErr = CPwUtil::FormatSystemMessage(dwErr);
    if(strErr.size() == 0) { SetLastError(dwPrevErr); return; }

    // Try with CreateProcess API now (supports longer paths)
    if(WU_SysCreateProcess(lpFile, lpParameters) == TRUE)
    {
      SetLastError(dwPrevErr);
      return;
    }

    CString strMsg = TRL("&File");
    RemoveAcceleratorTip(&strMsg);
    strMsg += _T(": ");
    strMsg += lpFile;

    if(lpParameters != NULL)
    {
      strMsg += _T("\r\n\r\n");
      strMsg += TRL("Arguments");
      strMsg += _T(": ");
      strMsg += lpParameters;
    }

    strMsg += _T("\r\n\r\n");
    strMsg += strErr.c_str();

    MessageBox(hParent, strMsg, PWM_PRODUCT_NAME_SHORT, MB_ICONWARNING | MB_OK);
  }

  SetLastError(dwPrevErr);
}

BOOL WU_SysCreateProcess(LPCTSTR lpFile, LPCTSTR lpParameters)
{
  ASSERT(lpFile != NULL); if(lpFile == NULL) return TRUE;
  ASSERT(lpFile[0] != 0); if(lpFile[0] == 0) return TRUE;

  std::basic_string<TCHAR> strCmdLine;

  const size_t uFileLen = _tcslen(lpFile);
  if((lpFile[0] == _T('\"')) && (lpFile[uFileLen - 1] == _T('\"')))
    strCmdLine = lpFile;
  else
  {
    strCmdLine = _T("\"");
    strCmdLine += lpFile;
    strCmdLine += _T("\"");
  }

  if((lpParameters != NULL) && (lpParameters[0] != 0))
  {
    strCmdLine += _T(" ");
    strCmdLine += lpParameters;
  }

  STARTUPINFO sui;
  ZeroMemory(&sui, sizeof(STARTUPINFO));
  sui.cb = sizeof(STARTUPINFO);

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

  const size_t dwBufLen = strCmdLine.size() + 8;
  LPTSTR lpBuf = new TCHAR[dwBufLen];
  ZeroMemory(lpBuf, dwBufLen * sizeof(TCHAR));
  _tcscpy_s(lpBuf, dwBufLen - 4, strCmdLine.c_str());

  BOOL bRes = ((CreateProcess(NULL, lpBuf, NULL, NULL, FALSE,
    0, NULL, NULL, &sui, &pi) == FALSE) ? FALSE : TRUE);

  if((pi.hThread != NULL) && (pi.hThread != INVALID_HANDLE_VALUE))
    CloseHandle(pi.hThread);
  if((pi.hProcess != NULL) && (pi.hProcess != INVALID_HANDLE_VALUE))
    CloseHandle(pi.hProcess);

  SAFE_DELETE_ARRAY(lpBuf);
  return bRes;
}

BOOL _FileAccessible(LPCTSTR lpFile)
{
  ASSERT(lpFile != NULL); if(lpFile == NULL) return FALSE;
  if(lpFile[0] == 0) return FALSE;

  FILE *fp = NULL;
  _tfopen_s(&fp, lpFile, _T("rb"));
  if(fp == NULL) return FALSE;
  fclose(fp);
  return TRUE;
}

BOOL _FileWritable(LPCTSTR lpFile)
{
  ASSERT(lpFile != NULL); if(lpFile == NULL) return FALSE;
  if(lpFile[0] == 0) return FALSE;

  FILE *fp = NULL;
  _tfopen_s(&fp, lpFile, _T("ab"));
  if(fp == NULL) return FALSE;
  fclose(fp);
  return TRUE;
}

int _OpenLocalFile(LPCTSTR szFile, int nMode)
{
  std_string strPath = Executable::instance().getPathOnly();

  std_string strFile = strPath;
  strFile += szFile;

#ifndef _WIN32_WCE
  LPCTSTR lpVerb = NULL;
  if(nMode == OLF_OPEN) { } // Default == OLF_OPEN
  else if(nMode == OLF_PRINT) lpVerb = _T("print");
  else if(nMode == OLF_EXPLORE) lpVerb = _T("explore");
  else { ASSERT(FALSE); }

  return (int)(INT_PTR)ShellExecute(::GetActiveWindow(), lpVerb,
    strFile.c_str(), NULL, strPath.c_str(), KPSW_SHOWDEFAULT);
#else
  ASSERT(FALSE); // Implement before using on WinCE
  return 0;
#endif
}

BOOL WU_GetFileNameSz(BOOL bOpenMode, LPCTSTR lpSuffix, LPTSTR lpStoreBuf, DWORD dwBufLen)
{
  ASSERT(lpSuffix != NULL); if(lpSuffix == NULL) return FALSE;
  ASSERT(lpStoreBuf != NULL); if(lpStoreBuf == NULL) return FALSE;
  ASSERT(dwBufLen != 0); if(dwBufLen == 0) return FALSE;

  CString strSample = _T("*.");
  strSample += lpSuffix;

  CString strFilter = TRL("All Files");
  strFilter += _T(" (*.*)|*.*||");

  DWORD dwFlags = 0;
  if(bOpenMode == FALSE)
  {
    dwFlags |= (OFN_LONGNAMES | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
    dwFlags |= OFN_EXTENSIONDIFFERENT;
    dwFlags |= (OFN_EXPLORER | OFN_ENABLESIZING | OFN_NOREADONLYRETURN);
  }
  else
  {
    dwFlags |= (OFN_LONGNAMES | OFN_EXTENSIONDIFFERENT);
    dwFlags |= (OFN_EXPLORER | OFN_ENABLESIZING);
    dwFlags |= (OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);
  }

  CFileDialog dlg(bOpenMode, lpSuffix, strSample, dwFlags, strFilter, NULL);
  if(NewGUI_DoModal(&dlg) == IDOK)
  {
    strSample = dlg.GetPathName();

    if(static_cast<DWORD>(strSample.GetLength()) < dwBufLen)
    {
      _tcscpy_s(lpStoreBuf, dwBufLen, (LPCTSTR)strSample);
      return TRUE;
    }
    else _tcsncpy_s(lpStoreBuf, dwBufLen, (LPCTSTR)strSample, dwBufLen - 1);
  }

  return FALSE;
}

std::vector<std::basic_string<TCHAR> > WU_GetFileNames(BOOL bOpenMode,
  LPCTSTR lpSuffix, LPCTSTR lpFilter, BOOL bAllowMultiSelect,
  CWnd* pParent, BOOL bAddToRecent, BOOL bNoChangeDir)
{
  std::vector<std::basic_string<TCHAR> > v;

  const std::basic_string<TCHAR> strDir = WU_GetCurrentDirectory();

  std::basic_string<TCHAR> strInitial = _T("*.");
  strInitial += ((lpSuffix != NULL) ? lpSuffix : _T("*"));

  DWORD dwFlags = 0;
  if(bAllowMultiSelect == TRUE) dwFlags |= OFN_ALLOWMULTISELECT;
  if(bAddToRecent == FALSE) dwFlags |= OFN_DONTADDTORECENT;
  if(bNoChangeDir == TRUE) dwFlags |= OFN_NOCHANGEDIR;

  dwFlags |= (OFN_EXPLORER | OFN_ENABLESIZING);
  dwFlags |= (OFN_EXTENSIONDIFFERENT | OFN_HIDEREADONLY | OFN_LONGNAMES);
  dwFlags |= OFN_PATHMUSTEXIST;

  if(bOpenMode == FALSE) // Save
    dwFlags |= OFN_OVERWRITEPROMPT;
  else // Open
    dwFlags |= OFN_FILEMUSTEXIST;

  CFileDialog dlg(bOpenMode, NULL, strInitial.c_str(), dwFlags, lpFilter, pParent);
  if(NewGUI_DoModal(&dlg) == IDOK)
  {
    if(bAllowMultiSelect == FALSE)
    {
      CString strSelected = dlg.GetPathName();
      LPCTSTR lpSelected = strSelected; // Avoid cast
      v.push_back(lpSelected);
    }
    else // Multi-select
    {
      POSITION pos = dlg.GetStartPosition();

      while(pos != NULL)
      {
        CString strFile = dlg.GetNextPathName(pos);
        LPCTSTR lpFile = strFile; // Avoid cast
        v.push_back(lpFile);
      }
    }
  }

  if(bNoChangeDir == TRUE) WU_SetCurrentDirectory(strDir.c_str());

  return v;
}

int WU_GetAppHelpSource()
{
  return g_nAppHelpSource;
}

void WU_SetAppHelpSource(int nSource)
{
  ASSERT((nSource >= 0) && (nSource <= 1));
  g_nAppHelpSource = nSource;
}

BOOL WU_OpenAppHelp(LPCTSTR lpTopicFile, HWND hParent)
{
  if(g_nAppHelpSource == APPHS_LOCAL)
  {
    WU_RemoveAppHelpZoneIdentifier();

    TCHAR tszBuf[MAX_PATH * 2];
    AU_GetApplicationDirectory(tszBuf, MAX_PATH * 2 - 2, TRUE, TRUE);

    // CString str = _T("hh.exe ms-its:");
    CString str = _T("\"ms-its:");
    str += tszBuf;
    str += _T("/");
    str += PWM_README_FILE;

    if(lpTopicFile != NULL)
    {
      str += _T("::/");
      str += lpTopicFile;
    }

    str += _T("\"");

    // TWinExec(str, KPSW_SHOWDEFAULT);
    WU_SysExecute(_T("hh.exe"), str, hParent);
  }
  else // APPHS_ONLINE
  {
    CString str = PWM_HOMEPAGE;
    if(lpTopicFile != NULL) str += lpTopicFile;
    else str = PWM_URL_HELP;

    OpenUrlEx(str, NULL);
  }

  return TRUE;
}

void WU_RemoveAppHelpZoneIdentifier()
{
  TCHAR tszLocalDir[MAX_PATH * 2];
  AU_GetApplicationDirectory(tszLocalDir, MAX_PATH * 2 - 2, FALSE, FALSE);

  CString strLocal = tszLocalDir;
  strLocal += _T("\\");
  strLocal += PWM_README_FILE;

  AU_RemoveZoneIdentifier(strLocal);
}

UINT TWinExec(LPCTSTR lpCmdLine, WORD wCmdShow)
{
  ASSERT(lpCmdLine != NULL);
  if(lpCmdLine == NULL) return ERROR_PATH_NOT_FOUND;

    STARTUPINFO sui;
  ZeroMemory(&sui, sizeof(STARTUPINFO));
  sui.cb = sizeof(STARTUPINFO);
  sui.wShowWindow = wCmdShow;

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

  const size_t dwCmdLen = _tcslen(lpCmdLine);
  LPTSTR lp = new TCHAR[dwCmdLen + 4];
  _tcscpy_s(lp, dwCmdLen + 2, lpCmdLine);

  BOOL bResult = CreateProcess(NULL, lp, NULL, NULL, FALSE, 0,
    NULL, NULL, &sui, &pi);

  // LPVOID lpMsgBuf;
  // FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
  // MessageBox(NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION);
  // LocalFree(lpMsgBuf);

  if((pi.hThread != NULL) && (pi.hThread != INVALID_HANDLE_VALUE))
    CloseHandle(pi.hThread);
  if((pi.hProcess != NULL) && (pi.hProcess != INVALID_HANDLE_VALUE))
    CloseHandle(pi.hProcess);

  SAFE_DELETE_ARRAY(lp);
  return ((bResult != FALSE) ? 32 : ERROR_FILE_NOT_FOUND);
}

BOOL WU_SupportsMultiLineTooltips()
{
#if 1                                                                             // rrvt
  return currentOS.is >= CurrentOS::WinXP;                                        // OS >= XP
#else                                                                             // rrvt
  OSVERSIONINFO osvi;
  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);

  return ((((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1)) ||
    (osvi.dwMajorVersion > 5)) ? TRUE : FALSE);
#endif                                                                            // rrvt
}

std::basic_string<TCHAR> WU_GetTempDirectory()
{
  TCHAR szDir[MAX_PATH * 2];
  GetTempPath(MAX_PATH * 2 - 2, szDir);
  if(szDir[_tcslen(szDir) - 1] != _T('\\'))
    _tcscat_s(szDir, _T("\\"));
  VERIFY(WU_CreateDirectoryTree(szDir) == S_OK);

  std::basic_string<TCHAR> str = szDir;
  return str;
}

std::basic_string<TCHAR> WU_GetTempFile(LPCTSTR lpSuffix)
{
  if((lpSuffix == NULL) || (lpSuffix[0] == 0))
    lpSuffix = _T(".tmp");

  const std::basic_string<TCHAR> strTempDir = WU_GetTempDirectory();

  const DWORD dwOffset = (GetTickCount() << 10);
  std::basic_string<TCHAR> tszFile;
  for(size_t iTry = 0; iTry < 100; ++iTry)
  {
    const DWORD dwTest = dwOffset + rand();

    CString strTest;
    strTest.Format(_T("%s%s%u%s"), strTempDir.c_str(), _T("Tmp"), dwTest, lpSuffix);
    if(_FileAccessible(strTest) == TRUE) continue; // Exists already
    if(_FileWritable(strTest) == FALSE) continue; // Test write access

    VERIFY(DeleteFile(strTest) != FALSE);
    tszFile = (LPCTSTR)strTest;
    break;
  }

  return tszFile;
}

std::basic_string<TCHAR> WU_GetUserName()
{
  std::basic_string<TCHAR> tsz;

  LPTSTR lpUser = new TCHAR[WU_MAX_USER_LEN + 1];
  memset(lpUser, 0, (WU_MAX_USER_LEN + 1) * sizeof(TCHAR));
  DWORD dwUserLength = WU_MAX_USER_LEN;
  GetUserName(lpUser, &dwUserLength);
  tsz += lpUser;
  SAFE_DELETE_ARRAY(lpUser);

  if(tsz.size() > 0) tsz += _T(" @ ");

  LPTSTR lpMachine = new TCHAR[WU_MAX_MACHINE_LEN + 1];
  memset(lpMachine, 0, (WU_MAX_MACHINE_LEN + 1) * sizeof(TCHAR));
  DWORD dwMachineLength = WU_MAX_MACHINE_LEN;
  GetComputerName(lpMachine, &dwMachineLength);
  tsz += lpMachine;
  SAFE_DELETE_ARRAY(lpMachine);

  return tsz;
}

/*
// Warning: this function is NOT multithreading-safe!
BOOL ContainsChildWindow(HWND hWndContainer, LPCTSTR lpChildWindowText)
{
  ASSERT(lpChildWindowText != NULL);
  if(lpChildWindowText == NULL) return FALSE;
  ASSERT(lpChildWindowText[0] != 0);
  if(lpChildWindowText[0] == 0) return FALSE;

  g_lpChildWindowText = lpChildWindowText;

  BOOL bWindowFound = FALSE;
  EnumChildWindows(hWndContainer, CcwEnumChildProc, (LPARAM)&bWindowFound);
  return bWindowFound;
}

BOOL CALLBACK CcwEnumChildProc(HWND hWnd, LPARAM lParam)
{
  // int nLength = GetWindowTextLength(hWnd);
  // if(nLength <= 0) return TRUE; // Continue enumeration

  int nLength = _tcslen(g_lpChildWindowText);

  int nAllocated = nLength + 4;
  LPTSTR lpText = new TCHAR[nAllocated];
  memset(lpText, 0, nAllocated * sizeof(TCHAR));

  GetWindowText(hWnd, lpText, nAllocated - 2);

  if(_tcsstr(lpText, g_lpChildWindowText) != NULL)
  {
    *((BOOL *)lParam) = TRUE;

    SAFE_DELETE_ARRAY(lpText);
    return FALSE; // Stop enumeration
  }

  SAFE_DELETE_ARRAY(lpText);
  return TRUE; // Continue enumeration
}
*/

void SafeActivateNextWindow(HWND hWndBase)
{
  HWND hWnd = GetWindow(hWndBase, GW_HWNDNEXT);

  WINDOWPLACEMENT wp;
  wp.length = sizeof(WINDOWPLACEMENT);

  while(true)
  {
    if(hWnd != hWndBase)
    {
      const LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
      GetWindowPlacement(hWnd, &wp);

      if(((lStyle & WS_VISIBLE) != 0) && (wp.showCmd != SW_SHOWMINIMIZED))
      {
        if(GetWindowTextLength(hWnd) != 0) break;
      }
    }

    hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    if(hWnd == NULL) break;
  }

  SetWindowPos(hWndBase, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

  if(hWnd != NULL)
  {
    SetForegroundWindow(hWnd);

    const DWORD dwStartTime = timeGetTime();
    while(true)
    {
      if(GetForegroundWindow() == hWnd) break;

      if((timeGetTime() - dwStartTime) > 1000) break;
      Sleep(50);
    }
  }
}

HWND WU_ShowWindowInTaskbar(HWND hWndShow, HWND hParent, BOOL bShow)
{
  LONG_PTR sty;

  if(bShow == FALSE) // Hide
  {
    sty = GetWindowLongPtr(hWndShow, GWL_EXSTYLE);
    SetWindowLongPtr(hWndShow, GWL_EXSTYLE, sty & ~(WS_EX_APPWINDOW));

    sty = GetWindowLongPtr(hWndShow, GWL_STYLE);
    SetWindowLongPtr(hWndShow, GWL_STYLE, (sty | WS_CHILD) & ~(WS_POPUP));

    return SetParent(hWndShow, hParent);
  }
  else // Show
  {
    HWND h = SetParent(hWndShow, NULL);

    sty = GetWindowLongPtr(hWndShow, GWL_STYLE);
    SetWindowLongPtr(hWndShow, GWL_STYLE, (sty | WS_POPUP) & ~(WS_CHILD));

    sty = GetWindowLongPtr(hWndShow, GWL_EXSTYLE);
    SetWindowLongPtr(hWndShow, GWL_EXSTYLE, sty | WS_EX_APPWINDOW);

    return h;
  }
}

#define LCL_WUEE_FAIL { ASSERT(FALSE); SAFE_DELETE_ARRAY(pBuf); return str; }

std::basic_string<TCHAR> WU_ExpandEnvironmentVars(LPCTSTR lpSrc)
{
  std::basic_string<TCHAR> str;

  ASSERT(lpSrc != NULL); if(lpSrc == NULL) return str;

  str = lpSrc;

  TCHAR vDummy[4] = { 0, 0, 0, 0 };
  const DWORD dwReq = ExpandEnvironmentStrings(lpSrc, &vDummy[0], 2);
  if(dwReq == 0) { ASSERT(FALSE); return str; }

  TCHAR *pBuf = new TCHAR[dwReq + 6];
  ZeroMemory(pBuf, (dwReq + 6) * sizeof(TCHAR));
  const DWORD dwCpy = ExpandEnvironmentStrings(lpSrc, pBuf, dwReq + 4);
  if(dwCpy == 0) LCL_WUEE_FAIL;
  if(dwCpy > (dwReq + 1)) LCL_WUEE_FAIL;

  str = pBuf;
  SAFE_DELETE_ARRAY(pBuf);
  return str;
}

BOOL WU_IsAbsolutePath(LPCTSTR lpPath)
{
  ASSERT(lpPath != NULL); if(lpPath == NULL) return FALSE;

  if(lpPath[0] == 0) return FALSE; // String has length 0
  if(lpPath[1] == 0) return FALSE; // String has length 1

  if((lpPath[0] == _T('\\')) && (lpPath[1] == _T('\\')))
    return TRUE; // Absolute network path

  if((lpPath[1] == _T(':')) && (lpPath[2] == _T('\\')))
    return TRUE; // Absolute Windows path

  return FALSE;
}

void WU_FillPlaceholders(CString* pString, const SPR_CONTENT_FLAGS* pcf)
{
  ASSERT(pString != NULL); if(pString == NULL) return;

  AppLocator::FillPlaceholders(pString, pcf);
}

BOOL WU_FlushStorageBuffers(TCHAR tchDriveLetter, BOOL bOnlyIfRemovable)
{
  if(bOnlyIfRemovable == TRUE)
  {
    std::basic_string<TCHAR> strRoot;
    strRoot += tchDriveLetter;
    strRoot += _T(":\\");

    if(GetDriveType(strRoot.c_str()) != DRIVE_REMOVABLE) return TRUE;
  }

  std::basic_string<TCHAR> strDevice = _T("\\\\.\\");
  strDevice += tchDriveLetter;
  strDevice += _T(":");

  HANDLE hDevice = CreateFile(strDevice.c_str(), GENERIC_READ |
    GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
    OPEN_EXISTING, 0, NULL);
  if(hDevice == INVALID_HANDLE_VALUE) { ASSERT(FALSE); return FALSE; }

  const std::basic_string<TCHAR> strDir = WU_FreeDriveIfCurrent(tchDriveLetter);

  BOOL bResult = TRUE;
  DWORD dwDummy = 0;
  if(DeviceIoControl(hDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0,
    &dwDummy, NULL) != FALSE)
  {
    dwDummy = 0;
    VERIFY(DeviceIoControl(hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0,
      NULL, 0, &dwDummy, NULL));
  }
  else bResult = FALSE;

  WU_SetCurrentDirectory(strDir.c_str());

  VERIFY(CloseHandle(hDevice));
  return bResult;
}

BOOL WU_FlushStorageBuffersEx(LPCTSTR lpFileOnStorage, BOOL bOnlyIfRemovable)
{
  if(lpFileOnStorage == NULL) { ASSERT(FALSE); return FALSE; }
  if(_tcslen(lpFileOnStorage) < 3) return FALSE;
  if(lpFileOnStorage[1] != _T(':')) return FALSE;
  if(lpFileOnStorage[2] != _T('\\')) return FALSE;

  const TCHAR tchDrive = static_cast<TCHAR>(toupper(
    static_cast<int>(lpFileOnStorage[0])));
  return WU_FlushStorageBuffers(tchDrive, bOnlyIfRemovable);
}

std::basic_string<TCHAR> WU_GetCurrentDirectory()
{
  BOOST_STATIC_ASSERT(SI_REGSIZE >= MAX_PATH);

  TCHAR tszDir[SI_REGSIZE];
  tszDir[0] = 0;
  if(GetCurrentDirectory(SI_REGSIZE - 1, tszDir) == 0)
  {
    ASSERT(FALSE);
    return std::basic_string<TCHAR>();
  }

  return std::basic_string<TCHAR>(tszDir);
}

void WU_SetCurrentDirectory(LPCTSTR lpDirectory)
{
  if(lpDirectory == NULL) { ASSERT(FALSE); return; }
  if(lpDirectory[0] == 0) return;

  VERIFY(SetCurrentDirectory(lpDirectory));
}

std::basic_string<TCHAR> WU_FreeDriveIfCurrent(TCHAR tchDriveLetter)
{
  std::basic_string<TCHAR> strEmpty;

  std::basic_string<TCHAR> strDir = WU_GetCurrentDirectory();
  if(strDir.size() < 3) return strEmpty;
  if(strDir[1] != _T(':')) return strEmpty;
  if(strDir[2] != _T('\\')) return strEmpty;

  const TCHAR tchPar = static_cast<TCHAR>(toupper(static_cast<int>(tchDriveLetter)));
  const TCHAR tchCur = static_cast<TCHAR>(toupper(static_cast<int>(strDir[0])));
  if(tchPar != tchCur) return strEmpty;

  TCHAR tszTemp[SI_REGSIZE];
  if(GetTempPath(SI_REGSIZE - 1, tszTemp) == 0) { ASSERT(FALSE); return strEmpty; }
  VERIFY(WU_CreateDirectoryTree(tszTemp) == S_OK);

  WU_SetCurrentDirectory(&tszTemp[0]);
  return strDir;
}

PWG_ERROR PwgGenerateWithExtVerify(std::vector<TCHAR>& vOutPassword,
  const PW_GEN_SETTINGS_EX* pSettings, CNewRandom* pRandomSource, HWND hWndCtx)
{
  CPwManager* pmCtx = NULL;
  CPwSafeDlg* pDlg = (CPwSafeDlg*)KPMI_GetMainDialog();
  if(pDlg != NULL) pmCtx = &pDlg->m_mgr;
  else { ASSERT(FALSE); }

  PW_ENTRY peDummy;
  CEntryUtil::GetDummyEntry(&peDummy, pmCtx);

  PWG_ERROR e;
  while(true)
  {
    e = PwgGenerateEx(vOutPassword, pSettings, pRandomSource);
    if(e != PWGE_SUCCESS) break;

    TCHAR tchNull = 0;
    CString str = ((vOutPassword.size() > 0) ? &vOutPassword[0] : &tchNull);
    CString strCmp = SprCompile(str, false, &peDummy, pmCtx, false, false);
    if(str != strCmp)
    {
      BYTE btGen = ((pSettings != NULL) ? pSettings->btGeneratorType : PWGT_NULL);
      if(btGen == PWGT_CHARSET) continue; // Silently try again
      else
      {
        ASSERT(btGen == PWGT_PATTERN);

        CString strText = str;
        strText += _T("\r\n\r\n");
        strText += TRL("The generated password contains a placeholder. When using this password (e.g. by copying it to the clipboard or auto-typing it), the placeholder will be replaced, i.e. effectively a different password might be used.");
        strText += _T("\r\n\r\n");
        strText += TRL("Accept this password?");

        int iRes = MessageBox(hWndCtx, strText, PWM_PRODUCT_NAME_SHORT,
          (MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2));
        if((iRes != IDYES) && (iRes != IDOK)) continue;
      }
    }

    KP_GENERATED_PASSWORD gp;
    ZeroMemory(&gp, sizeof(KP_GENERATED_PASSWORD));
    gp.lpPassword = str;
    gp.lpSettings = pSettings;

    LPCTSTR lpObjection = NULL;
    _CallPlugins(KPM_VALIDATE_GENPASSWORD, (LPARAM)&gp, (LPARAM)&lpObjection);
    if(lpObjection != NULL) continue;

    break;
  }

  return e;
}

CString FilterTrlComment(LPCTSTR lpEnglishString)
{
  if(lpEnglishString == NULL) { ASSERT(FALSE); return CString(); }
  if(lpEnglishString[0] == 0) { ASSERT(FALSE); return CString(); }

  CString strTrl = TRL_VAR(lpEnglishString);
  const int nStart = strTrl.Find(_T('['));
  const int nEnd = strTrl.Find(_T(']'));
  if((nStart >= 0) && (nEnd >= 0) && (nEnd > nStart))
  {
    CString strRet = strTrl.Left(nStart) + strTrl.Right(strTrl.GetLength() -
      nEnd - 1);
    return strRet.Trim(_T(" \t\r\n"));
  }
  else { ASSERT(FALSE); }

  CString strEng = lpEnglishString;
  const int nEngStart = strEng.Find(_T('['));
  const int nEngEnd = strEng.Find(_T(']'));
  if((nEngStart >= 0) && (nEngEnd >= 0) && (nEngEnd > nEngStart))
  {
    CString strEngRet = strEng.Left(nEngStart) + strEng.Right(
      strEng.GetLength() - nEngEnd - 1);
    return strEngRet.Trim(_T(" \t\r\n"));
  }

  ASSERT(FALSE);
  return CString();
}

void WU_GetUserApplications(std::vector<AV_APP_INFO>& vStorage)
{
  vStorage.clear();

  LPCTSTR lpDisplay = NULL;
  LPCTSTR lpPath = NULL;

  _CallPlugins(KPM_USERAPP_GETFIRST, (LPARAM)&lpDisplay, (LPARAM)&lpPath);
  while((lpDisplay != NULL) && (lpPath != NULL))
  {
    AV_APP_INFO appInfo;
    appInfo.strDisplayName = lpDisplay;
    appInfo.strPath = lpPath;
    vStorage.push_back(appInfo);

    lpDisplay = NULL; lpPath = NULL;
    _CallPlugins(KPM_USERAPP_GETNEXT, (LPARAM)&lpDisplay, (LPARAM)&lpPath);
  }
}

HRESULT WU_CreateDirectoryTree(LPCTSTR lpDirPath)
{
  ASSERT(lpDirPath != NULL); if(lpDirPath == NULL) return E_POINTER;

  std::basic_string<TCHAR> strPath = lpDirPath;
  std::vector<std::basic_string<TCHAR> > vDirs;
  SU_Split(vDirs, strPath, _T("/\\"));

  std::basic_string<TCHAR> strCur;
  for(size_t i = 0; i < vDirs.size(); ++i)
  {
    if(vDirs[i].size() == 0) continue;
    if(strCur.size() > 0) strCur += _T("\\");
    strCur += vDirs[i];

    if(strCur[strCur.size() - 1] == _T(':')) continue;

    if((CreateDirectory(strCur.c_str(), NULL) == FALSE) &&
      (GetLastError() != ERROR_ALREADY_EXISTS))
      return E_FAIL;
  }

  return S_OK;
}

/* void WU_MouseClick(bool bRightClick)
{
  INPUT inputClick;

  ZeroMemory(&inputClick, sizeof(INPUT));
  inputClick.type = INPUT_MOUSE;
  inputClick.mi.dwFlags = (bRightClick ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN);
  inputClick.mi.dwExtraInfo = GetMessageExtraInfo();
  VERIFY(SendInput(1, &inputClick, sizeof(INPUT)) == 1);

  ZeroMemory(&inputClick, sizeof(INPUT));
  inputClick.type = INPUT_MOUSE;
  inputClick.mi.dwFlags = (bRightClick ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP);
  inputClick.mi.dwExtraInfo = GetMessageExtraInfo();
  VERIFY(SendInput(1, &inputClick, sizeof(INPUT)) == 1);
} */

bool WU_IsCommandLineURL(const CString& strURL)
{
  const int nLength = strURL.GetLength();

  if(nLength >= 6)
  {
    CString strTemp = strURL.Left(6);
    strTemp = strTemp.MakeLower();

    if(strTemp == _T("cmd://")) return true;
  }

  if(nLength >= 2)
  {
    if(strURL.Left(2) == _T("\\\\")) return true; // UNC path support
  }

  return false;
}

BOOL WU_RunElevated(LPCTSTR lpExe, LPCTSTR lpArgs, HWND hParent)
{
  if(lpExe == NULL) { ASSERT(FALSE); return FALSE; }

  SHELLEXECUTEINFO sei;
  ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));

  sei.cbSize = sizeof(SHELLEXECUTEINFO);
  // sei.fMask = SEE_MASK_NOZONECHECKS;
  sei.hwnd = hParent;
  sei.lpFile = lpExe;
  sei.lpParameters = lpArgs;
  sei.nShow = SW_SHOW;

  // Elevate on Windows Vista and higher
  if(AU_IsAtLeastWinVistaSystem() == TRUE) sei.lpVerb = _T("runas");

  return ShellExecuteEx(&sei);
}

/* std::basic_string<TCHAR> WU_CreateBackupFile(LPCTSTR lpBaseFile)
{
  std::basic_string<TCHAR> strResult;
  if(lpBaseFile == NULL) { ASSERT(FALSE); return strResult; }

  const std::basic_string<TCHAR> strBase = lpBaseFile;
  const std::basic_string<TCHAR> strBackup = strBase + _T(".bak");

  const bool bMadeUnhidden = CPwUtil::UnhideFile(strBackup.c_str());

  if(CopyFile(strBase.c_str(), strBackup.c_str(), FALSE) == FALSE)
  {
    const DWORD dwResult = GetLastError();
    if(dwResult != 0) strResult = WU_FormatSystemMessage(dwResult);
  }

  if(bMadeUnhidden) CPwUtil::HideFile(strBackup.c_str(), true);

  if(strResult.size() > 0)
  {
    const std::basic_string<TCHAR> strF = TRL("Failed to create a backup file!");
    strResult = strBackup + _T("\r\n\r\n") + strF + _T("\r\n\r\n") + strResult;
  }
  return strResult;
} */

#ifdef _UNICODE
#define WU_CIN std::wcin
#else
#define WU_CIN std::cin
#endif

bool WU_StdInReadLine(std::basic_string<TCHAR>& strOut)
{
  strOut.clear();

  try
  {
    if(!WU_CIN) return false;

    std::basic_string<TCHAR> str;
    WU_CIN >> std::noskipws;
    std::getline(WU_CIN, str);

    CString cs = str.c_str();
    cs = cs.Trim();

    strOut = (LPCTSTR)cs;
  }
  catch(...) { return false; }

  return true;
}

// Read a password from StdIn. The password is read only once
// and then cached.
std::basic_string<TCHAR> WU_StdInReadPassword()
{
  if(!g_bStdInPw)
  {
    WU_StdInReadLine(g_strStdInPw);
    g_bStdInPw = true;
  }

  return g_strStdInPw;
}

std::basic_string<TCHAR> WU_GetEnv(LPCTSTR lpVarName, bool bExpand)
{
  std::basic_string<TCHAR> str;
  if(lpVarName == NULL) { ASSERT(FALSE); return str; }
  if(lpVarName[0] == 0) { ASSERT(FALSE); return str; }

  const DWORD cchBuf = 32767;
  const DWORD cchAlloc = cchBuf + 5;

  scoped_array<TCHAR> vBuf(new TCHAR[cchAlloc]);
  ZeroMemory(vBuf.get(), sizeof(TCHAR) * cchAlloc);

  if(GetEnvironmentVariable(lpVarName, vBuf.get(), cchBuf) == 0)
    return str;

  str = vBuf.get();

  if(bExpand)
  {
    ZeroMemory(vBuf.get(), sizeof(TCHAR) * cchAlloc);

    // Plus 1 required by documentation of ExpandEnvironmentStrings
    if(ExpandEnvironmentStrings(str.c_str(), vBuf.get(), cchBuf + 1) != 0)
    {
      if(vBuf.get()[0] != 0) str = vBuf.get();
    }
  }

  return str;
}

LONG WU_RegCreateKey(_In_ HKEY hKey, _In_opt_ LPCTSTR lpSubKey,
  _Out_ PHKEY phkResult)
{
  // RegCreateKey supports NULL as lpSubKey; RegCreateKeyEx does not
  if(lpSubKey == NULL)
  {
    if(phkResult != NULL) *phkResult = hKey;
    return ERROR_SUCCESS;
  }

  // The RegCreateKey API function is for compatibility with 16-bit
  // versions of Windows only, thus we call RegCreateKeyEx
  return RegCreateKeyEx(hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE,
    KEY_ALL_ACCESS, NULL, phkResult, NULL);
}

void WU_PrintHtmlFile(LPCTSTR lpFile, HWND hParent)
{
  if((lpFile == NULL) || (*lpFile == _T('\0'))) { ASSERT(FALSE); return; }

  CString str = WU_GetRegStr(HKEY_CLASSES_ROOT,
    _T("htmlfile\\shell\\print\\command"), _T("")).c_str();
  if(str.GetLength() > 0)
  {
    WU_FixPrintCommandLine(str);

    CString strOrg = str;
    str.Replace(_T("%1"), lpFile);
    str.Replace(_T("%L"), lpFile);
    str.Replace(_T("%l"), lpFile);
    str.Replace(_T("%*"), lpFile);

    if(str != strOrg)
    {
      VERIFY(str.Replace(_T("%0"), _T("")) == 0);
      VERIFY(str.Replace(_T("%2"), _T("")) == 0);
      VERIFY(str.Replace(_T("%3"), _T("")) == 0);
      VERIFY(str.Replace(_T("%4"), _T("")) == 0);

      CString strUrl = _T("cmd://");
      strUrl += str;

      OpenUrlEx(strUrl, hParent);
      return;
    }
  }

  ASSERT(FALSE);
  // Not all browsers support the "print" shell verb
  ShellExecute(hParent, _T("print"), lpFile, NULL, NULL, SW_SHOW);
}

void WU_FixPrintCommandLine(CString& str)
{
  CString strLwr = str;
  strLwr = strLwr.MakeLower();

  // Workaround for Microsoft Office breaking the 'Print' shell verb;
  // https://sourceforge.net/p/keepass/bugs/1675/
  // https://support.microsoft.com/en-us/help/274527/cannot-print-file-with--htm-extension-from-windows-explorer-by-right-c
  if(strLwr.Find(_T("\\msohtmed.exe")) >= 0)
  {
    const UINT ccMax = MAX_PATH;
    const UINT ccAlloc = ccMax + 2;
    LPTSTR lpSys = new TCHAR[ccAlloc];
    ZeroMemory(lpSys, ccAlloc * sizeof(TCHAR));

    size_t cc = GetSystemDirectory(lpSys, ccMax);
    if((cc > 0) && (cc < ccMax))
    {
      cc = _tcslen(lpSys);
      if(lpSys[cc - 1] != _T('\\')) lpSys[cc] = _T('\\');

      CString strNew = _T("\"");
      strNew += lpSys;
      strNew += _T("rundll32.exe\" \"");
      strNew += lpSys;
      strNew += _T("mshtml.dll\",PrintHTML \"%1\"");

      str = strNew;
    }
    else { ASSERT(FALSE); }

    SAFE_DELETE_ARRAY(lpSys);
  }
}

BOOL WU_GetConfigBool(int iCfgID, BOOL bDefault)
{
  if(iCfgID == WU_CFG_CLIP_NOPERSIST) return g_bClipNoPersist;

  ASSERT(FALSE);
  return bDefault;
}

void WU_SetConfigBool(int iCfgID, BOOL bValue)
{
  if(iCfgID == WU_CFG_CLIP_NOPERSIST) g_bClipNoPersist = bValue;
  else { ASSERT(FALSE); }
}
