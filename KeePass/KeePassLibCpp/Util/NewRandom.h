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

#ifndef ___NEW_RANDOM_H___
#define ___NEW_RANDOM_H___

#include "../SysDefEx.h"
#include <vector>
#include <boost/utility.hpp>
#include "../Crypto/SHA2/SHA2.h"

class CNewRandom : boost::noncopyable
{
public:
	CNewRandom();
	virtual ~CNewRandom();

	void GetRandomBuffer(_Out_bytecap_(dwSize) BYTE *pBuf, DWORD dwSize);
	UINT64 GetRandomUInt64(UINT64 uMaxExcl);

	void AddToUserEntropyPool(const BYTE *pData, DWORD dwSize);
	void ClearUserEntropyPool();

private:
	void Initialize();
	void AddRandomObject(_In_bytecount_(uSize) const void *pObj, size_t uSize);

	static void SysCryptGetRandom(BYTE *pBuf, DWORD dwSize);

	std::vector<BYTE> m_vPseudoRandom;
	std::vector<BYTE> m_vUserRandom;
	DWORD m_dwCounter;
};

class CNewRandomInterface
{
public:
	virtual ~CNewRandomInterface() { }

	virtual BOOL GenerateRandomSequence(unsigned long uRandomSeqSize,
		unsigned char *pBuffer) const = 0;
};

// Seed is 128 bits = 4 32-bit DWORDs
void srandXorShift(unsigned long *pSeed128);
unsigned long randXorShift();

// Must be able to hold at least 16 bytes
void randCreateUUID(BYTE *pUUID16, CNewRandom *pRandomSource);

#endif
