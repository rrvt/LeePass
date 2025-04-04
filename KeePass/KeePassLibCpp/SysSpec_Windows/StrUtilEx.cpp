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
#include "../Util/MemUtil.h"
#include "../Util/StrUtil.h"
#include "../Util/AppUtil.h"

char* _StringToAnsi(const WCHAR* lpwString)
{
	if(lpwString == NULL) { ASSERT(FALSE); return NULL; }

	const int nChars = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK,
		lpwString, -1, NULL, 0, NULL, NULL);
	char* p = new char[nChars + 2];
	p[0] = 0; p[1] = 0; p[nChars] = 0; p[nChars + 1] = 0;
	VERIFY(WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, lpwString,
		-1, p, nChars, NULL, NULL) == nChars);

	return p;
}

WCHAR* _StringToUnicode(const char* pszString)
{
	if(pszString == NULL) { ASSERT(FALSE); return NULL; }

	const int nChars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszString,
		-1, NULL, 0);
	WCHAR* p = new WCHAR[nChars + 2];
	p[0] = 0; p[1] = 0; p[nChars] = 0; p[nChars + 1] = 0;
	VERIFY(MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszString,
		-1, p, nChars) == nChars);

	return p;
}

std::basic_string<char> _StringToAnsiStl(const TCHAR* pszString)
{
	std::basic_string<char> strA;

#ifdef _UNICODE
	LPSTR lpA = _StringToAnsi(pszString);
	if(lpA != NULL) strA = (LPCSTR)lpA;
	SAFE_DELETE_ARRAY(lpA);
#else
	if(pszString == NULL) { ASSERT(FALSE); return strA; }
	strA = pszString;
#endif

	return strA;
}

std::basic_string<WCHAR> _StringToUnicodeStl(const TCHAR* pszString)
{
	std::basic_string<WCHAR> strW;

#ifdef _UNICODE
	if(pszString == NULL) { ASSERT(FALSE); return strW; }
	strW = pszString;
#else
	LPWSTR lpW = _StringToUnicode(pszString);
	if(lpW != NULL) strW = (LPCWSTR)lpW;
	SAFE_DELETE_ARRAY(lpW);
#endif

	return strW;
}

UTF8_BYTE *_StringToUTF8(const TCHAR *pszSourceString)
{
	DWORD i, j = 0;
	DWORD dwLength, dwBytesNeeded;
	BYTE *p = NULL;
	WCHAR ut;
	const WCHAR *pUni = NULL;

#ifndef _UNICODE
	WCHAR *pUniBuffer = NULL;
	DWORD dwUniBufferLength = 0;
#endif

	ASSERT(pszSourceString != NULL); if(pszSourceString == NULL) return NULL;

#ifdef _UNICODE
	dwLength = lstrlen(pszSourceString) + 1; // In order to be compatible with the code below, add 1 for the zero at the end of the buffer
	pUni = pszSourceString;
#else
	// This returns the new length plus the zero byte - i.e. the whole buffer!
	dwLength = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszSourceString, -1, NULL, 0);
	dwUniBufferLength = dwLength + 2;
	pUniBuffer = new WCHAR[dwUniBufferLength];
	pUniBuffer[0] = 0; pUniBuffer[1] = 0;
	pUni = pUniBuffer;
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszSourceString, -1, pUniBuffer, dwLength);
#endif

	// Both counting and converting routines need update to support surrogates
	// count UTF-8 needed bytes
	dwBytesNeeded = 0;
	for(i = 0; i < dwLength; ++i)
	{
		ut = pUni[i];

		if(ut == 0) break;

		if(ut < 0x80) dwBytesNeeded++;
		else if(ut < 0x0800) dwBytesNeeded += 2;
		else dwBytesNeeded += 3;
	}

	p = new BYTE[dwBytesNeeded + 2];
	ASSERT(p != NULL); if(p == NULL) return NULL;

	j = 0;
	for(i = 0; i < dwLength; ++i)
	{
		ut = pUni[i];

		// if(ut == 0) break;

		if(ut < 0x80) // 7-bit character, store as it is
		{
			p[j] = (BYTE)ut; j++;
		}
		else if(ut < 0x800) // Are 2 bytes enough?
		{
			p[j] = (BYTE)(0xC0 | (ut >> 6)); j++;
			p[j] = (BYTE)(0x80 | (ut & 0x3F)); j++;
		}
		else // Maximum bytes needed for UCS-2 is 3 bytes in UTF-8
		{
			p[j] = (BYTE)(0xE0 | (ut >> 12)); j++;
			p[j] = (BYTE)(0x80 | ((ut >> 6) & 0x3F)); j++;
			p[j] = (BYTE)(0x80 | (ut & 0x3F)); j++;
		}
	}
	p[j] = 0; // Terminate string
	ASSERT(j == (dwBytesNeeded + 1));

#ifndef _UNICODE
	mem_erase(pUniBuffer, dwUniBufferLength * sizeof(WCHAR));
	SAFE_DELETE_ARRAY(pUniBuffer);
#endif

	return p;
}

DWORD _UTF8NumChars(const UTF8_BYTE *pUTF8String)
{
	DWORD i = 0, dwLength = 0;
	BYTE bt;

	ASSERT(pUTF8String != NULL);
	if(pUTF8String == NULL) return 0;

	while(1)
	{
		bt = pUTF8String[i];
		if(bt == 0) break;
		else if((bt & 0x80) == 0) dwLength++;
		else if((bt & 0xC0) == 0xC0) dwLength++;
		else if((bt & 0xE0) == 0xE0) dwLength++;
		i++;
	}

	return dwLength;
}

// This returns the needed bytes to represent the string, without terminating NULL character
DWORD _UTF8BytesNeeded(const TCHAR *pszString)
{
	DWORD i = 0;
	DWORD dwBytes = 0;
	USHORT us;

	// Don't use this function any more. The _StringToUTF8 function uses some pre-conversion
	// functions that makes a simple length calculation like in this function impossible.
	// If you really need this function, comment out the following ASSERT, but be careful!
	ASSERT(FALSE);

	ASSERT(pszString != NULL);
	if(pszString == NULL) return 0;

	while(1)
	{
#ifdef _UNICODE
		us = (USHORT)pszString[i];
#else
		us = (USHORT)(((USHORT)((BYTE)pszString[i])) & 0x00FF);
#endif

		if(us == 0) break;

		if(us < 0x0080) dwBytes++;
		else if(us < 0x0800) dwBytes += 2;
		else dwBytes += 3;

		i++;
	}

	return dwBytes;
}

TCHAR *_UTF8ToString(const UTF8_BYTE *pUTF8String)
{
	DWORD i, j;
	DWORD dwNumChars, dwMoreBytes, dwPBufLength;
	BYTE b0, b1, b2;
	WCHAR *p;
	WCHAR tch;

#ifndef _UNICODE
	WCHAR *pANSI;
#endif

	ASSERT(pUTF8String != NULL); if(pUTF8String == NULL) return NULL;

	// Count the needed Unicode chars (right counterpart to _StringToUTF8)
	i = 0; dwNumChars = 0;
	while(1)
	{
		b0 = (BYTE)pUTF8String[i];
		dwMoreBytes = 0;
		if(b0 == 0) break;
		else if(b0 < 0xC0) dwMoreBytes++;
		else if(b0 < 0xE0) dwMoreBytes++;
		else if(b0 < 0xF0) dwMoreBytes++;
		dwNumChars++;
		i += dwMoreBytes;

		if(dwMoreBytes == 0) return NULL; // Invalid UTF-8 string
	}
	// if(dwNumChars == 0) return NULL;

	dwPBufLength = dwNumChars + 2;
	p = new WCHAR[dwPBufLength];
	ASSERT(p != NULL); if(p == NULL) return NULL;

	i = 0; j = 0;
	while(1)
	{
		b0 = pUTF8String[i]; i++;

		if(b0 < 0x80)
		{
			p[j] = (WCHAR)b0; j++;
		}
		else
		{
			b1 = pUTF8String[i]; i++;

			ASSERT((b1 & 0xC0) == 0x80);
			if((b1 & 0xC0) != 0x80) break;

			if((b0 & 0xE0) == 0xC0)
			{
				tch = (WCHAR)(b0 & 0x1F);
				tch <<= 6;
				tch |= (b1 & 0x3F);
				p[j] = tch; j++;
			}
			else
			{
				b2 = pUTF8String[i]; i++;

				ASSERT((b2 & 0xC0) == 0x80);
				if((b2 & 0xC0) != 0x80) break;

				tch = (WCHAR)(b0 & 0xF);
				tch <<= 6;
				tch |= (b1 & 0x3F);
				tch <<= 6;
				tch |= (b2 & 0x3F);
				p[j] = tch; j++;
			}
		}

		if(b0 == 0) break;
	}

#ifdef _UNICODE
	return (TCHAR *)p;
#else
	// Got Unicode, convert to ANSI
	dwNumChars = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, p, -1, NULL, 0, NULL, NULL);
	pANSI = new WCHAR[dwNumChars + 2];
	pANSI[0] = 0;

	int nErr = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, p, -1, (LPSTR)pANSI, dwNumChars, NULL, NULL);
	if(dwNumChars != 122) { ASSERT(nErr != 122); } // ERROR_INSUFFICIENT_BUFFER is defined as 122...
	else { ASSERT(GetLastError() == 0); nErr = nErr; }

	if(p != NULL) mem_erase(p, dwPBufLength);
	SAFE_DELETE_ARRAY(p);
	return (TCHAR *)pANSI;
#endif
}

BOOL _IsUTF8String(const UTF8_BYTE *pUTF8String)
{
	DWORD i = 0;
	BYTE b0, b1, b2;

	ASSERT(pUTF8String != NULL); if(pUTF8String == NULL) return FALSE;

	if(pUTF8String[0] == 0xEF)
		if(pUTF8String[1] == 0xBB)
			if(pUTF8String[2] == 0xBF) i += 3;

	while(1)
	{
		b0 = pUTF8String[i]; i++;

		if(b0 >= 0x80)
		{
			b1 = pUTF8String[i]; i++;

			if((b1 & 0xC0) != 0x80) return FALSE;

			if((b0 & 0xE0) != 0xC0)
			{
				b2 = pUTF8String[i]; i++;

				if((b2 & 0xC0) != 0x80) return FALSE;
			}
		}

		if(b0 == 0) break;
	}

	return TRUE;
}

static HMODULE m_hShlWApi = NULL;
static LPCWSTRCMPEX m_lpCmpNatural = NULL;

#ifndef _WIN32_WCE
void NSCAPI_Initialize()
{
	NSCAPI_Exit(); // Free any previously loaded library

	// ShlWApi.dll is available on all Windows operating systems >= 98
	m_hShlWApi = AU_LoadLibrary(_T("ShlWApi.dll"));
	if(m_hShlWApi == NULL) return;

	m_lpCmpNatural = (LPCWSTRCMPEX)GetProcAddress(m_hShlWApi, "StrCmpLogicalW");
	if(m_lpCmpNatural == NULL) NSCAPI_Exit();
}

void NSCAPI_Exit()
{
	m_lpCmpNatural = NULL;

	if(m_hShlWApi != NULL)
	{
		FreeLibrary(m_hShlWApi);
		m_hShlWApi = NULL;
	}
}

bool NSCAPI_Supported()
{
	return (m_lpCmpNatural != NULL);
}
#endif

int StrCmpNaturalEx(LPCTSTR x, LPCTSTR y)
{
	if(m_lpCmpNatural == NULL) { ASSERT(FALSE); return _tcsicmp(x, y); }

	CT2CW xw(x);
	CT2CW yw(y);

	return m_lpCmpNatural(xw, yw);
}

LPCTSTRCMPEX StrCmpGetNaturalMethodOrFallback()
{
	return ((m_lpCmpNatural != NULL) ? StrCmpNaturalEx : _tcsicmp);
}
