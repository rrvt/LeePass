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

#ifndef ___IMAGE_UTIL_EX_H___
#define ___IMAGE_UTIL_EX_H___

#pragma once

#include "../../KeePassLibCpp/SysDefEx.h"
#include <gdiplus.h>
#include <vector>

std::vector<BYTE> NewGUI_SaveIcon(HICON hIcon);

bool NewGUI_ExtractVistaIcon(HICON hIcon, Gdiplus::Bitmap** pOutBmp);
bool NewGUI_ExtractVistaIcon(const std::vector<BYTE>& v, Gdiplus::Bitmap** pOutBmp);

#endif // ___IMAGE_UTIL_EX_H___
