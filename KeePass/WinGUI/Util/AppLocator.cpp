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
#include "AppLocator.h"
#include "WinUtil.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include <boost/algorithm/string/predicate.hpp>

bool AppLocator::m_bPathsQueried = false;
std::basic_string<TCHAR> AppLocator::m_strIEPath;
std::basic_string<TCHAR> AppLocator::m_strFirefoxPath;
std::basic_string<TCHAR> AppLocator::m_strOperaPath;
std::basic_string<TCHAR> AppLocator::m_strChromePath;
std::basic_string<TCHAR> AppLocator::m_strSafariPath;
std::basic_string<TCHAR> AppLocator::m_strEdgePath;

AppLocator::AppLocator()
{
}

AppLocator::~AppLocator()
{
}

void AppLocator::FillPlaceholders(CString* pString, const SPR_CONTENT_FLAGS* pcf)
{
	if(pString == NULL) { ASSERT(FALSE); return; }

	AppLocator::GetPaths();

	AppLocator::ReplacePath(pString, _T("{INTERNETEXPLORER}"), m_strIEPath, pcf);
	AppLocator::ReplacePath(pString, _T("{FIREFOX}"), m_strFirefoxPath, pcf);
	AppLocator::ReplacePath(pString, _T("{OPERA}"), m_strOperaPath, pcf);
	AppLocator::ReplacePath(pString, _T("{GOOGLECHROME}"), m_strChromePath, pcf);
	AppLocator::ReplacePath(pString, _T("{SAFARI}"), m_strSafariPath, pcf);
	AppLocator::ReplacePath(pString, _T("{EDGE}"), m_strEdgePath, pcf);
}

void AppLocator::ReplacePath(CString* p, LPCTSTR lpPlaceholder,
	const std::basic_string<TCHAR>& strFill, const SPR_CONTENT_FLAGS* pcf)
{
	if(p == NULL) { ASSERT(FALSE); return; }
	if(lpPlaceholder == NULL) { ASSERT(FALSE); return; }
	if(lpPlaceholder[0] == 0) { ASSERT(FALSE); return; }

	std::basic_string<TCHAR> str;
	if((pcf != NULL) && pcf->bMakeCmdQuotes)
	{
		str = _T("\"");
		str += SprTransformContent(strFill.c_str(), pcf);
		str += _T("\"");
	}
	else
	{
		std::basic_string<TCHAR> strWithQ = _T("\"");
		strWithQ += strFill;
		strWithQ += _T("\"");

		str = SprTransformContent(strWithQ.c_str(), pcf);
	}

	p->Replace(lpPlaceholder, str.c_str());
}

void AppLocator::GetPaths()
{
	if(m_bPathsQueried) return;

	if(m_strIEPath.size() == 0) AppLocator::FindInternetExplorer();
	if(m_strFirefoxPath.size() == 0) AppLocator::FindFirefox();
	if(m_strOperaPath.size() == 0) AppLocator::FindOpera();
	if(m_strChromePath.size() == 0) AppLocator::FindChrome();
	if(m_strSafariPath.size() == 0) AppLocator::FindSafari();
	if(m_strEdgePath.size() == 0) AppLocator::FindEdge();

	m_bPathsQueried = true;
}

void AppLocator::FindInternetExplorer()
{
	LPCTSTR lpIE = _T("SOFTWARE\\Clients\\StartMenuInternet\\IEXPLORE.EXE\\shell\\open\\command");

	for(int i = 0; i < 4; ++i)
	{
		std::basic_string<TCHAR> str;

		// https://msdn.microsoft.com/en-us/library/windows/desktop/dd203067.aspx
		if(i == 0)
			str = WU_GetRegStr(HKEY_CURRENT_USER, lpIE, _T(""));
		else if(i == 1)
			str = WU_GetRegStr(HKEY_LOCAL_MACHINE, lpIE, _T(""));
		else if(i == 2)
			str = WU_GetRegStr(HKEY_CLASSES_ROOT,
				_T("IE.AssocFile.HTM\\shell\\open\\command"), _T(""));
		else
			str = WU_GetRegStr(HKEY_CLASSES_ROOT,
				_T("Applications\\iexplore.exe\\shell\\open\\command"), _T(""));

		str = AppLocator::Fix(str);
		if(str.size() == 0) continue;
		// https://sourceforge.net/p/keepass/discussion/329221/thread/6b292ede/
		if(boost::algorithm::starts_with(str, _T("iexplore.exe"))) continue;

		m_strIEPath = str;
		break;
	}
}

void AppLocator::FindFirefox()
{
	LPCTSTR lpRoot = _T("SOFTWARE\\Mozilla\\Mozilla Firefox");
	LPCTSTR lpRootEsr = _T("SOFTWARE\\Mozilla\\Mozilla Firefox ESR");

	std_string strVer = WU_GetRegStr(HKEY_LOCAL_MACHINE, lpRoot, _T("CurrentVersion"));
	if(strVer.size() == 0)
		strVer = WU_GetRegStr(HKEY_LOCAL_MACHINE, lpRootEsr, _T("CurrentVersion"));

	if(strVer.size() != 0)
	{
		// The ESR version stores the 'CurrentVersion' value under
		// 'Mozilla Firefox ESR', but the version-specific info
		// under 'Mozilla Firefox\\<Version>' (without 'ESR')
		const std_string strMain = std_string(lpRoot) + _T("\\") +
			strVer + _T("\\Main");

		std_string str = WU_GetRegStr(HKEY_LOCAL_MACHINE, strMain.c_str(), _T("PathToExe"));
		m_strFirefoxPath = AppLocator::Fix(str);
	}

	if(m_strFirefoxPath.size() == 0)
		m_strFirefoxPath = FindAppByClass(_T(".html"), _T("firefox.exe"));
}

void AppLocator::FindOpera()
{
	// Old Opera versions
	LPCTSTR lpOp12 = _T("SOFTWARE\\Clients\\StartMenuInternet\\Opera\\shell\\open\\command");
	// Opera >= 20.0.1387.77
	LPCTSTR lpOp20 = _T("SOFTWARE\\Clients\\StartMenuInternet\\OperaStable\\shell\\open\\command");

	for(int i = 0; i < 4; ++i)
	{
		std::basic_string<TCHAR> str;

		// https://msdn.microsoft.com/en-us/library/windows/desktop/dd203067.aspx
		if(i == 0)
			str = WU_GetRegStr(HKEY_CURRENT_USER, lpOp20, _T(""));
		else if(i == 1)
			str = WU_GetRegStr(HKEY_CURRENT_USER, lpOp12, _T(""));
		else if(i == 2)
			str = WU_GetRegStr(HKEY_LOCAL_MACHINE, lpOp20, _T(""));
		else
			str = WU_GetRegStr(HKEY_LOCAL_MACHINE, lpOp12, _T(""));

		str = AppLocator::Fix(str);
		if(str.size() == 0) continue;

		m_strOperaPath = str;
		break;
	}
}

void AppLocator::FindChrome()
{
	LPCTSTR lpPath = _T("ChromeHTML\\shell\\open\\command");
	std_string str = WU_GetRegStr(HKEY_CLASSES_ROOT, lpPath, _T(""));
	m_strChromePath = AppLocator::Fix(str);
	if(m_strChromePath.size() > 0) return;

	const size_t ccNameBuf = MAX_PATH + 2;
	TCHAR tszName[ccNameBuf];
	ZeroMemory(&tszName[0], ccNameBuf * sizeof(TCHAR));
	DWORD dwIndex = 0;
	while(true)
	{
		DWORD dwNameLen = ccNameBuf - 1;
		const LONG lRes = RegEnumKeyEx(HKEY_CLASSES_ROOT, dwIndex, &tszName[0],
			&dwNameLen, NULL, NULL, NULL, NULL);
		++dwIndex;

		if(lRes == ERROR_NO_MORE_ITEMS) break;
		if(lRes != ERROR_SUCCESS) continue;

		if((_tcslen(&tszName[0]) >= 11) && (_tcsnicmp(&tszName[0],
			_T("ChromeHTML."), 11) == 0))
		{
			std_string strKey = &tszName[0];
			strKey += _T("\\shell\\open\\command");

			str = WU_GetRegStr(HKEY_CLASSES_ROOT, strKey.c_str(), _T(""));
			m_strChromePath = AppLocator::Fix(str);
			if(m_strChromePath.size() > 0) return;
		}
	}

	lpPath = _T("Applications\\chrome.exe\\shell\\open\\command");
	str = WU_GetRegStr(HKEY_CLASSES_ROOT, lpPath, _T(""));
	m_strChromePath = AppLocator::Fix(str);
}

void AppLocator::FindSafari()
{
	LPCTSTR lpPath = _T("SOFTWARE\\Apple Computer, Inc.\\Safari");
	std_string str = WU_GetRegStr(HKEY_LOCAL_MACHINE, lpPath, _T("BrowserExe"));
	m_strSafariPath = AppLocator::Fix(str);
}

void AppLocator::FindEdge()
{
	LPCTSTR lpPath = _T("SOFTWARE\\Clients\\StartMenuInternet\\Microsoft Edge\\shell\\open\\command");
	std_string str = WU_GetRegStr(HKEY_LOCAL_MACHINE, lpPath, _T(""));
	m_strEdgePath = AppLocator::Fix(str);
}

std_string AppLocator::FindAppByClass(LPCTSTR lpClass, LPCTSTR lpExeName)
{
	std_string str = FindAppByClass(lpClass, lpExeName, KEY_WOW64_64KEY);
	if(str.size() == 0) str = FindAppByClass(lpClass, lpExeName, KEY_WOW64_32KEY);
	return str;
}

std_string AppLocator::FindAppByClass(LPCTSTR lpClass, LPCTSTR lpExeName,
	REGSAM rsAdd)
{
	std_string str;
	if((lpClass == NULL) || (*lpClass == _T('\0'))) { ASSERT(FALSE); return str; }
	if((lpExeName == NULL) || (*lpExeName == _T('\0'))) { ASSERT(FALSE); return str; }

	ASSERT(*lpClass == _T('.')); // File extension class

	const REGSAM rs = (KEY_QUERY_VALUE | rsAdd);
	const std_string strOpenWith = std_string(lpClass) + _T("\\OpenWithProgids");

	HKEY hkOpenWith = NULL;
	if(RegOpenKeyEx(HKEY_CLASSES_ROOT, strOpenWith.c_str(), 0, rs, &hkOpenWith) !=
		ERROR_SUCCESS) { ASSERT(FALSE); return str; }

	for(DWORD dwIndex = 0; dwIndex < DWORD_MAX; ++dwIndex)
	{
		TCHAR tszOpenWithClass[MAX_PATH + 1] = { _T('\0') };
		DWORD cch = MAX_PATH;

		if(RegEnumValue(hkOpenWith, dwIndex, tszOpenWithClass, &cch, NULL,
			NULL, NULL, NULL) != ERROR_SUCCESS) break;
		if(tszOpenWithClass[0] == _T('\0')) { ASSERT(FALSE); continue; }
		if(tszOpenWithClass[MAX_PATH - 1] != _T('\0')) { ASSERT(FALSE); continue; }

		const std_string strCommandKey = std_string(tszOpenWithClass) +
			_T("\\Shell\\open\\command");
		const std_string strPath = AppLocator::Fix(WU_GetRegStr(
			HKEY_CLASSES_ROOT, strCommandKey.c_str(), _T("")));
		if(strPath.size() != 0)
		{
			const CString csPath(strPath.c_str());
			const CString csName = CsFileOnly(&csPath);

			if(csName.CompareNoCase(lpExeName) == 0)
			{
				str = strPath;
				break;
			}
		}
	}

	VERIFY(RegCloseKey(hkOpenWith) == ERROR_SUCCESS);
	return str;
}

std::basic_string<TCHAR> AppLocator::Fix(const std::basic_string<TCHAR>& strPath)
{
	return SU_GetQuotedPath(strPath);
}
