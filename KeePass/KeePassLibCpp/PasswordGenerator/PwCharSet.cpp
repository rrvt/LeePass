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
#include "PwCharSet.h"

#include "../Util/MemUtil.h"

PwCharSet::PwCharSet()
{
	this->Clear();
}

PwCharSet::PwCharSet(LPCWSTR lpCharacters)
{
	this->Clear();
	this->Add(lpCharacters);
}

PwCharSet::~PwCharSet()
{
}

void PwCharSet::Clear()
{
	ASSERT(sizeof(WCHAR) == 2);

	memset(m_vTab, 0, PCS_TABSIZE);
	m_vChars.clear();
}

unsigned int PwCharSet::Size() const
{
	return static_cast<unsigned int>(m_vChars.size());
}

bool PwCharSet::Contains(WCHAR ch) const
{
	return (((m_vTab[ch / 8] >> (ch % 8)) & 1) != 0);
}

bool PwCharSet::Contains(LPCWSTR lpCharacters) const
{
	if(lpCharacters == NULL) { ASSERT(FALSE); return false; }

	DWORD i = 0;
	while(lpCharacters[i] != L'\0')
	{
		if(this->Contains(lpCharacters[i]) == false) return false;
		++i;
	}

	return true;
}

WCHAR PwCharSet::GetAt(unsigned int uPos) const
{
	if(uPos >= m_vChars.size()) { ASSERT(FALSE); return L'\0'; }
	return m_vChars[uPos];
}

void PwCharSet::Add(WCHAR ch)
{
	if(this->Contains(ch)) return;

	m_vTab[ch / 8] |= ((BYTE)1 << (ch % 8));
	m_vChars.push_back(ch);
}

void PwCharSet::Add(LPCWSTR lpCharacters)
{
	if(lpCharacters == NULL) { ASSERT(FALSE); return; }

	size_t uPos = 0;
	while(lpCharacters[uPos] != L'\0')
	{
		this->Add(lpCharacters[uPos]);
		++uPos;
	}
}

void PwCharSet::Add(LPCWSTR lpChars1, LPCWSTR lpChars2, LPCWSTR lpChars3)
{
	if(lpChars1 != NULL) this->Add(lpChars1);
	if(lpChars2 != NULL) this->Add(lpChars2);
	if(lpChars3 != NULL) this->Add(lpChars3);
}

void PwCharSet::AddRange(WCHAR chLow, WCHAR chHigh)
{
	for(WCHAR ch = chLow; ch < chHigh; ++ch)
		this->Add(ch);

	this->Add(chHigh);
}

bool PwCharSet::Remove(WCHAR ch)
{
	if(this->Contains(ch) == false) return false;

	m_vTab[ch / 8] &= ~((WCHAR)1 << (ch % 8));

	for(std::vector<WCHAR>::iterator it = m_vChars.begin();
		it != m_vChars.end(); ++it)
	{
		if((*it) == ch)
		{
			m_vChars.erase(it);
			break;
		}
	}

	return true;
}

bool PwCharSet::Remove(LPCWSTR lpCharacters)
{
	if(lpCharacters == NULL) { ASSERT(FALSE); return false; }

	bool bResult = true;
	size_t uIndex = 0;

	while(lpCharacters[uIndex] != L'\0')
	{
		bResult &= this->Remove(lpCharacters[uIndex]);
		++uIndex;
	}

	return bResult;
}

bool PwCharSet::RemoveIfAllExist(LPCWSTR lpCharacters)
{
	if(this->Contains(lpCharacters) == false) return false;

	return this->Remove(lpCharacters);
}

bool PwCharSet::AddCharSet(WCHAR chCharSetIdentifier)
{
	bool bResult = true;

	switch(chCharSetIdentifier)
	{
		case L'a': this->Add(PDCS_LOWER_CASE, PDCS_NUMERIC); break;
		case L'A': this->Add(PDCS_LOWER_CASE, PDCS_UPPER_CASE,
			PDCS_NUMERIC); break;
		case L'U': this->Add(PDCS_UPPER_CASE, PDCS_NUMERIC); break;
		case L'c': this->Add(PDCS_LOWER_CONSONANTS); break;
		case L'C': this->Add(PDCS_LOWER_CONSONANTS,
			PDCS_UPPER_CONSONANTS); break;
		case L'z': this->Add(PDCS_UPPER_CONSONANTS); break;
		case L'd': this->Add(PDCS_NUMERIC); break; // Digit
		case L'h': this->Add(PDCS_LOWER_HEX); break;
		case L'H': this->Add(PDCS_UPPER_HEX); break;
		case L'l': this->Add(PDCS_LOWER_CASE); break;
		case L'L': this->Add(PDCS_LOWER_CASE, PDCS_UPPER_CASE); break;
		case L'u': this->Add(PDCS_UPPER_CASE); break;
		case L'p': this->Add(PDCS_PUNCTUATION); break;
		case L'b': this->Add(PDCS_BRACKETS); break;
		case L's': this->Add(PDCS_PRINTASCIISPEC); break;
		case L'S': this->Add(PDCS_UPPER_CASE, PDCS_LOWER_CASE);
			this->Add(PDCS_NUMERIC, PDCS_PRINTASCIISPEC); break;
		case L'v': this->Add(PDCS_LOWER_VOWELS); break;
		case L'V': this->Add(PDCS_LOWER_VOWELS, PDCS_UPPER_VOWELS); break;
		case L'Z': this->Add(PDCS_UPPER_VOWELS); break;
		case L'x': this->Add(PwCharSet::GetLatin1SChars().ToString().c_str()); break;
		default: bResult = false; break;
	}

	return bResult;
}

std::basic_string<WCHAR> PwCharSet::ToString() const
{
	const size_t cc = m_vChars.size();

	LPWSTR lp = new WCHAR[cc + 1];
	if(cc != 0) memcpy(lp, &m_vChars[0], cc * sizeof(WCHAR));
	lp[cc] = L'\0';

	std::basic_string<WCHAR> str = lp;
	SAFE_DELETE_ARRAY(lp);
	return str;
}

PwCharSet PwCharSet::GetSpecialChars()
{
	PwCharSet pcs;

	pcs.AddRange(L'!', L'/');
	pcs.AddRange(L':', L'@');
	pcs.AddRange(L'[', L'`');
	pcs.Add(L"|~");
	pcs.Remove(L"-_ ");
	pcs.Remove(PDCS_BRACKETS);

	return pcs;
}

PwCharSet PwCharSet::GetLatin1SChars()
{
	PwCharSet pcs;

	// [U+0080, U+009F] are C1 control characters,
	// U+00A0 is non-breaking space
	pcs.AddRange(L'\x00A1', L'\x00AC');
	// U+00AD is soft hyphen (format character)
	pcs.AddRange(L'\x00AE', L'\x00FF');

	return pcs;
}

USHORT PwCharSet::PackAndRemoveCharRanges()
{
	USHORT us = 0;

	if(this->RemoveIfAllExist(PDCS_UPPER_CASE)) us |= 1;
	us <<= 1;
	if(this->RemoveIfAllExist(PDCS_LOWER_CASE)) us |= 1;
	us <<= 1;
	if(this->RemoveIfAllExist(PDCS_NUMERIC)) us |= 1;
	us <<= 1;
	if(this->RemoveIfAllExist(PwCharSet::GetSpecialChars().ToString().c_str())) us |= 1;
	us <<= 1;
	if(this->RemoveIfAllExist(PDCS_PUNCTUATION)) us |= 1;
	us <<= 1;
	if(this->RemoveIfAllExist(L"-")) us |= 1;
	us <<= 1;
	if(this->RemoveIfAllExist(L"_")) us |= 1;
	us <<= 1;
	if(this->RemoveIfAllExist(L" ")) us |= 1;
	us <<= 1;
	if(this->RemoveIfAllExist(PDCS_BRACKETS)) us |= 1;
	us <<= 1;
	if(this->RemoveIfAllExist(PwCharSet::GetLatin1SChars().ToString().c_str())) us |= 1;

	return us;
}

void PwCharSet::UnpackCharRanges(USHORT usRanges)
{
	if((usRanges & 1) != 0) this->Add(PwCharSet::GetLatin1SChars().ToString().c_str());
	if((usRanges & 2) != 0) this->Add(PDCS_BRACKETS);
	if((usRanges & 4) != 0) this->Add(L' ');
	if((usRanges & 8) != 0) this->Add(L'_');
	if((usRanges & 16) != 0) this->Add(L'-');
	if((usRanges & 32) != 0) this->Add(PDCS_PUNCTUATION);
	if((usRanges & 64) != 0) this->Add(PwCharSet::GetSpecialChars().ToString().c_str());
	if((usRanges & 128) != 0) this->Add(PDCS_NUMERIC);
	if((usRanges & 256) != 0) this->Add(PDCS_LOWER_CASE);
	if((usRanges & 512) != 0) this->Add(PDCS_UPPER_CASE);
}
