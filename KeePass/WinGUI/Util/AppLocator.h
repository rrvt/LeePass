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

#ifndef ___APPLICATION_LOCATOR_H___
#define ___APPLICATION_LOCATOR_H___

#include <string>
#include <tchar.h>

#include <boost/utility.hpp>

#include "SprEngine/SprEngine.h"

class AppLocator : boost::noncopyable
{
private:
	AppLocator();
	~AppLocator();

public:
	static void FillPlaceholders(CString* pString, const SPR_CONTENT_FLAGS* pcf);

private:
	static void ReplacePath(CString* p, LPCTSTR lpPlaceholder,
		const std::basic_string<TCHAR>& strFill, const SPR_CONTENT_FLAGS* pcf);

	static void GetPaths();

	static void FindInternetExplorer();
	static void FindFirefox();
	static void FindOpera();
	static void FindChrome();
	static void FindSafari();
	static void FindEdge();

	static std_string FindAppByClass(LPCTSTR lpClass, LPCTSTR lpExeName);
	static std_string FindAppByClass(LPCTSTR lpClass, LPCTSTR lpExeName,
		REGSAM rsAdd);

	static std::basic_string<TCHAR> Fix(const std::basic_string<TCHAR>& strPath);

	static bool m_bPathsQueried;
	static std::basic_string<TCHAR> m_strIEPath;
	static std::basic_string<TCHAR> m_strFirefoxPath;
	static std::basic_string<TCHAR> m_strOperaPath;
	static std::basic_string<TCHAR> m_strChromePath;
	static std::basic_string<TCHAR> m_strSafariPath;
	static std::basic_string<TCHAR> m_strEdgePath;
};

#endif // ___APPLICATION_LOCATOR_H___
