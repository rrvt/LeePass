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
#include "../Util/AppUtil.h"
#include "../Util/MemUtil.h"
#include "../Util/NewRandom.h"
#include "../Util/PwUtil.h"
#include "../Util/StrUtil.h"
#include "../Util/FileTransactionEx.h"
#include "../PwManager.h"
#include "Deprecated.h"                                                 // rrvt

#include <aclapi.h>
#include <boost/scoped_array.hpp>

// #ifndef _WIN32_WCE
// #include <objbase.h>
// #include <atlconv.h>
// #endif // _WIN32_WCE

using boost::scoped_array;

static bool g_bAuInitialized = false;

static bool g_bOSInitialized = false;
static bool g_bIsWin9xSystem = false;
static bool g_bIsAtLeastWinVistaSystem = false;
static bool g_bIsAtLeastWin7System = false;
static bool g_bIsWine = false;
static bool g_bIsAppX = false;

void AU_EnsureInitialized()
{
  if(g_bAuInitialized) return;

  HMODULE hKernel = ::LoadLibrary(_T("Kernel32.dll")); // Native, not AU
  if(hKernel != NULL)
  {
    // Do not load libraries from the current working directory
    LPSETDLLDIRECTORY lpSetDllDirectory = (LPSETDLLDIRECTORY)
      ::GetProcAddress(hKernel, SETDLLDIRECTORY_FNNAME);
    if(lpSetDllDirectory != NULL) lpSetDllDirectory(_T(""));

    // Enable data execution prevention (DEP)
    LPSETPROCESSDEPPOLICY lpSetProcessDEPPolicy = (LPSETPROCESSDEPPOLICY)
      ::GetProcAddress(hKernel, SETPROCESSDEPPOLICY_FNNAME);
    if(lpSetProcessDEPPolicy != NULL) lpSetProcessDEPPolicy(PROCESS_DEP_ENABLE);

    ::FreeLibrary(hKernel);
    hKernel = NULL;
  }
  else { ASSERT(FALSE); }

  HMODULE hWer = ::LoadLibrary(_T("Wer.dll"));
  if(hWer != NULL)
  {
    LPWERADDEXCLUDEDAPPLICATION lpExcl = (LPWERADDEXCLUDEDAPPLICATION)
      ::GetProcAddress(hWer, WERADDEXCLUDEDAPPLICATION_FNNAME);
    if(lpExcl != NULL)
    {
      std::basic_string<WCHAR> str = _StringToUnicodeStl(PWM_EXENAME);
      str += L".exe";

      VERIFY(SUCCEEDED(lpExcl(str.c_str(), FALSE)));
    }
    else { ASSERT(FALSE); }

    ::FreeLibrary(hWer);
    hWer = NULL;
  }
  else { ASSERT(FALSE); }

  g_bAuInitialized = true;
}

HMODULE AU_LoadLibrary(LPCTSTR lpFileName)
{
  return ::LoadLibrary(lpFileName);
}

BOOL AU_GetApplicationDirectory(LPTSTR lpStoreBuf, DWORD dwBufLen, BOOL bFilterSpecial, BOOL bMakeURL)
{
  int i, j = 0;

  ASSERT(lpStoreBuf != NULL); if(lpStoreBuf == NULL) return FALSE;
  ASSERT(dwBufLen >= 3); if(dwBufLen < 3) return FALSE;

  lpStoreBuf[0] = 0; lpStoreBuf[1] = 0;
  GetModuleFileName(NULL, lpStoreBuf, dwBufLen);
  for(i = (int)_tcslen(lpStoreBuf) - 1; i > 1; --i) // Extract dir
  {
    if((lpStoreBuf[i] == _T('\\')) || (lpStoreBuf[i] == _T('/')))
    {
      lpStoreBuf[i] = 0;
      break;
    }
  }

  for(i = 0; i < (int)_tcslen(lpStoreBuf); ++i)
  {
    if((bMakeURL == TRUE) && (lpStoreBuf[i] == _T('\\'))) lpStoreBuf[i] = _T('/');

    if(bFilterSpecial == TRUE)
    {
      if(lpStoreBuf[i] != _T('\"'))
      {
        lpStoreBuf[j] = lpStoreBuf[i];
        j++;
      }
    }
  }

  return TRUE;
}

#ifndef _WIN32_WCE
BOOL AU_SecureDeleteFile(LPCTSTR pszFilePath)
{
  DWORD dwDummy = 0, dwTmp = 0;

  HANDLE hFile = CreateFile(pszFilePath, GENERIC_WRITE, FILE_SHARE_READ |
    FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
  if(hFile == INVALID_HANDLE_VALUE) return FALSE;

  const DWORD dwSizeLo = GetFileSize(hFile, NULL);
  if(dwSizeLo == 0) { CloseHandle(hFile); return DeleteFile(pszFilePath); }

  SetFilePointer(hFile, 0, NULL, FILE_END);
  WriteFile(hFile, &dwDummy, 4, &dwTmp, NULL);
  FlushFileBuffers(hFile);

  BYTE* pBuf = new BYTE[SDF_BUF_SIZE];
  if(pBuf == NULL) { CloseHandle(hFile); return FALSE; }

  // Four rounds: zeros, ones, random, random
  for(int n = 0; n < 4; ++n)
  {
    if(n == 0) memset(pBuf, 0, SDF_BUF_SIZE);
    else if(n == 1) memset(pBuf, 0xFF, SDF_BUF_SIZE);
    else
    {
      for(DWORD j = 0; j < SDF_BUF_SIZE; ++j)
        pBuf[j] = static_cast<BYTE>(randXorShift());
    }

    VERIFY(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == 0);
    DWORD i = (dwSizeLo + static_cast<DWORD>(n)); // Each round one more character at the end

    while(i != 0)
    {
      const DWORD m = ((i < SDF_BUF_SIZE) ? i : SDF_BUF_SIZE);

      const BOOL bRet = WriteFile(hFile, pBuf, m, &dwTmp, NULL);
      FlushFileBuffers(hFile);

      ASSERT(bRet == TRUE);
      if(bRet != TRUE) break;
      if(m != dwTmp) { ASSERT(FALSE); break; }

      i -= m;
    }
  }

  CloseHandle(hFile);
  SAFE_DELETE_ARRAY(pBuf);

  if(DeleteFile(pszFilePath) == FALSE)
  {
    if(_tremove(pszFilePath) != 0)
      return FALSE;
  }

  return TRUE;
}
#endif

int AU_WriteBigFile(LPCTSTR lpFilePath, const BYTE* pData, DWORD dwDataSize,
  BOOL bTransacted)
{
  // const bool bMadeUnhidden = CPwUtil::UnhideFile(lpFilePath);

  CFileTransactionEx ft(lpFilePath, (bTransacted != FALSE));
  std_string strBufFile;
  if(!ft.OpenWrite(strBufFile)) return PWE_GETLASTERROR;

  HANDLE hFile = CreateFile(strBufFile.c_str(), GENERIC_WRITE, 0, NULL,
    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if(hFile == INVALID_HANDLE_VALUE)
  {
    // if(bMadeUnhidden) CPwUtil::HideFile(lpFilePath, true);
    return PWE_NOFILEACCESS_WRITE;
  }

  int nResult = PWE_SUCCESS;
  DWORD dwRemaining = dwDataSize, dwPosition = 0;

  while(dwRemaining != 0)
  {
    const DWORD dwWrite = ((dwRemaining > AU_MAX_WRITE_BLOCK) ?
      AU_MAX_WRITE_BLOCK : dwRemaining);

    DWORD dwWritten = 0;
    if(WriteFile(hFile, &pData[dwPosition], dwWrite, &dwWritten, NULL) == FALSE)
    {
      nResult = PWE_FILEERROR_WRITE;
      break;
    }
    if(dwWritten == 0) { nResult = PWE_FILEERROR_WRITE; break; }

    dwPosition += dwWritten;
    dwRemaining -= dwWritten;
  }

  VERIFY(FlushFileBuffers(hFile));
  VERIFY(CloseHandle(hFile));

  if(!ft.CommitWrite()) return PWE_GETLASTERROR;

  // if(bMadeUnhidden) CPwUtil::HideFile(lpFilePath, true);
  return nResult;
}

void Priv_AU_EnsureOSInitialized()
{
  if(g_bOSInitialized) return;

#if 1                                                                   // rrvt
  g_bIsWin9xSystem           = currentOS.is <= CurrentOS::Win98;
  g_bIsAtLeastWinVistaSystem = currentOS.is >= CurrentOS::WinVista;
  g_bIsAtLeastWin7System     = currentOS.is >= CurrentOS::Win7;
#else
  OSVERSIONINFO osvi;
  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);

  g_bIsWin9xSystem = (osvi.dwMajorVersion <= 4);
  g_bIsAtLeastWinVistaSystem = (osvi.dwMajorVersion >= 6);
  g_bIsAtLeastWin7System = ((osvi.dwMajorVersion > 6) ||
    ((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 1)));
#endif                                                                  // rrvt

  // https://wiki.winehq.org/Developer_FAQ#How_can_I_detect_Wine.3F
  HMODULE hNT = LoadLibrary(_T("NTDLL.dll"));
  if(hNT != NULL)
  {
    g_bIsWine = (GetProcAddress(hNT, "wine_get_version") != NULL);
    FreeLibrary(hNT);
  }

  const DWORD ccPath = MAX_PATH * 4;
  scoped_array<TCHAR> aPath(new TCHAR[ccPath + 2]);
  if(AU_GetApplicationDirectory(aPath.get(), ccPath, FALSE, FALSE) != FALSE)
  {
    CString strPath = aPath.get();
    strPath = strPath.MakeLower();
    if(strPath.Find(_T("\\windowsapps\\")) >= 0)
    {
      boost::basic_regex<TCHAR> rx(_T("\\\\WindowsApps\\\\.*?_\\d+(\\.\\d+)*_"),
        boost::regex_constants::icase);
      g_bIsAppX = boost::regex_search(aPath.get(), rx,
        boost::regex_constants::match_any);
    }
    else { ASSERT(!g_bIsAppX); } // No AppX by default
  }
  else { ASSERT(FALSE); }

  g_bOSInitialized = true;
}

BOOL AU_IsWin9xSystem()
{
  Priv_AU_EnsureOSInitialized();
  return (g_bIsWin9xSystem ? TRUE : FALSE);
}

BOOL AU_IsAtLeastWinVistaSystem()
{
  Priv_AU_EnsureOSInitialized();
  return (g_bIsAtLeastWinVistaSystem ? TRUE : FALSE);
}

BOOL AU_IsAtLeastWin7System()
{
  Priv_AU_EnsureOSInitialized();
  return (g_bIsAtLeastWin7System ? TRUE : FALSE);
}

BOOL AU_IsWine()
{
  Priv_AU_EnsureOSInitialized();
  return (g_bIsWine ? TRUE : FALSE);
}

BOOL AU_IsAppX()
{
  Priv_AU_EnsureOSInitialized();
  return (g_bIsAppX ? TRUE : FALSE);
}

#ifndef _WIN32_WCE
/* BOOL _AU_RemoveZoneIdentifier(LPCTSTR lpFile)
{
  USES_CONVERSION;

  IPersistFile* pf = NULL;
  if(CoCreateInstance(CLSID_PersistentZoneIdentifier, NULL, CLSCTX_INPROC_SERVER,
    IID_IPersistFile, (LPVOID*)&pf) != S_OK) return FALSE;
  if(pf == NULL) { ASSERT(FALSE); return FALSE; }

  LPCOLESTR lpOleFile = T2OLE(lpFile);
  if(pf->Load(lpOleFile, STGM_READWRITE | STGM_SHARE_EXCLUSIVE) != S_OK) return FALSE;

  IZoneIdentifier* pz = NULL;
  if(pf->QueryInterface(IID_IZoneIdentifier, (void**)&pz) != S_OK) return FALSE;
  if(pz == NULL) { ASSERT(FALSE); pf->Release(); return FALSE; }

  DWORD dwZone = 0;
  VERIFY(pz->GetId(&dwZone) == S_OK);
  if((dwZone == URLZONE_INTERNET) || (dwZone == URLZONE_UNTRUSTED))
  {
    VERIFY(pz->Remove() == S_OK);
    VERIFY(pf->Save(NULL, TRUE) == S_OK);
  }

  pz->Release();
  pf->Release();
  return TRUE;
}

BOOL AU_RemoveZoneIdentifier(LPCTSTR lpFile)
{
  if(lpFile == NULL) { ASSERT(FALSE); return FALSE; }

  HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  if((hrCo != S_OK) && (hrCo != S_FALSE))
  {
    ASSERT(FALSE);
    return FALSE;
  }

  const BOOL bResult = _AU_RemoveZoneIdentifier(lpFile);

  CoUninitialize();
  return bResult;
} */

BOOL AU_RemoveZoneIdentifier(LPCTSTR lpFile)
{
  if((lpFile == NULL) || (lpFile[0] == 0)) { ASSERT(FALSE); return FALSE; }

  std::basic_string<TCHAR> str = lpFile;
  str += _T(":Zone.Identifier");

  if(GetFileAttributes(str.c_str()) != INVALID_FILE_ATTRIBUTES)
    DeleteFile(str.c_str());
  else return FALSE;

  return TRUE;
}
#endif // _WIN32_WCE

void AU_ProtectProcessWithDacl_Priv(HANDLE hProcess, HANDLE hToken)
{
  DWORD cbTokenUser = 0;
  GetTokenInformation(hToken, TokenUser, NULL, 0, &cbTokenUser);
  if(cbTokenUser == 0) { ASSERT(FALSE); return; }

  scoped_array<BYTE> aTokenUser(new BYTE[cbTokenUser]);
  ZeroMemory(aTokenUser.get(), cbTokenUser);
  if(GetTokenInformation(hToken, TokenUser, aTokenUser.get(),
    cbTokenUser, &cbTokenUser) == FALSE) { ASSERT(FALSE); return; }

  PTOKEN_USER pTokenUser = (PTOKEN_USER)aTokenUser.get();
  PSID pSid = pTokenUser->User.Sid;
  if((pSid == NULL) || (IsValidSid(pSid) == FALSE)) { ASSERT(FALSE); return; }

  DWORD cbAcl = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) +
    GetLengthSid(pSid);
  scoped_array<BYTE> aAcl(new BYTE[cbAcl]);
  PACL pAcl = (PACL)aAcl.get();

  if(InitializeAcl(pAcl, cbAcl, ACL_REVISION) == FALSE) { ASSERT(FALSE); return; }

  if(AddAccessAllowedAce(pAcl, ACL_REVISION, SYNCHRONIZE |
    PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, pSid) == FALSE)
  {
    ASSERT(FALSE);
    return;
  }

  VERIFY(SetSecurityInfo(hProcess, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
    NULL, NULL, pAcl, NULL) == ERROR_SUCCESS);
}

void AU_ProtectProcessWithDacl()
{
  HANDLE hProcess = GetCurrentProcess();
  HANDLE hToken = NULL;

  if(OpenProcessToken(hProcess, TOKEN_QUERY, &hToken) != FALSE)
  {
    AU_ProtectProcessWithDacl_Priv(hProcess, hToken);
    VERIFY(CloseHandle(hToken));
  }
  else { ASSERT(FALSE); }
}
