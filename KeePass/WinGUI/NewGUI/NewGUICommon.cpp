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
#include "NewGUICommon.h"
#include "../Resource.h"
#include "BCMenu.h"
#include "BtnST.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "QualityProgressCtrl.h"
#include "KCSideBannerWnd.h"
#include "XHyperLink.h"
#include "XPStyleButtonST.h"
#include "../PwSafeDlg.h"
#include "../Util/WinUtil.h"
#include "../Plugins/KpApiImpl.h"
#include <algorithm>
#include <gdiplus.h>
#include <boost/static_assert.hpp>

#include "../../KeePassLibCpp/Util/PwQualityEst.h"
#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/MemUtil.h"
#include "../../KeePassLibCpp/Util/AppUtil.h"

static bool g_gdiplusInitialized = false;
static ULONG_PTR g_gdiplusToken = 0;

static HMODULE g_hUser32 = NULL;

static BOOL g_bImgButtons = FALSE;
static CThemeHelperST* g_pThemeHelper = NULL;

static COLORREF m_crBannerStart = RGB(160, 160, 176); // = RGB(240, 240, 255);
static COLORREF m_crBannerEnd = RGB(16, 16, 32); // = RGB(192, 192, 255);
static COLORREF m_crBannerText = RGB(255, 255, 255); // RGB(0, 0, 0);
static bool m_bBannerFlip = true; // = false;
static CFont* m_pfBannerTitleFont = NULL;

static BOOL g_bPreventScreenCapture = FALSE;
static bool g_bDisplayAffChanged = false;

typedef BOOL(WINAPI *LPSETWINDOWDISPLAYAFFINITY)(HWND hWnd, DWORD dwAffinity);
static LPSETWINDOWDISPLAYAFFINITY g_lpSetWindowDisplayAffinity = NULL;

void NewGUI_Init()
{
  NewGUI_CleanUp();

  g_hUser32 = LoadLibrary(_T("User32.dll"));
  if(g_hUser32 != NULL)
    g_lpSetWindowDisplayAffinity = (LPSETWINDOWDISPLAYAFFINITY)GetProcAddress(
      g_hUser32, "SetWindowDisplayAffinity");
  else { ASSERT(FALSE); }
}

void NewGUI_CleanUp()
{
  if(m_pfBannerTitleFont != NULL)
  {
    m_pfBannerTitleFont->DeleteObject();
    SAFE_DELETE(m_pfBannerTitleFont);
  }

  if(g_hUser32 != NULL)
  {
    VERIFY(FreeLibrary(g_hUser32));

    g_lpSetWindowDisplayAffinity = NULL;
    g_hUser32 = NULL;
  }
}

BOOL* NewGUI_GetPreventScreenCapturePtr()
{
  return &g_bPreventScreenCapture;
}

void NewGUI_InitDialog(CDialog* pDlg, bool bTranslate)
{
  if(pDlg == NULL) { ASSERT(FALSE); return; }

  HWND hWnd = pDlg->m_hWnd;
  if(hWnd == NULL) { ASSERT(FALSE); return; }

  if(bTranslate)
  {
    NewGUI_TranslateCWnd(pDlg);
    EnumChildWindows(hWnd, NewGUI_TranslateWindowCb, 0);
  }

  NewGUI_CustomizeWindow(pDlg, true);
}

void NewGUI_SetWin32Banner()
{
  m_crBannerStart = RGB(240, 240, 255);
  m_crBannerEnd = RGB(192, 192, 255);
  m_crBannerText = RGB(0, 0, 0);
  m_bBannerFlip = false;
}

COLORREF NewGUI_GetBgColor()
{
  // HDC hDC = NULL;
  // int nBitsPerPixel = 0;

  // hDC = GetDC(NULL);
  // nBitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
  // ReleaseDC(NULL, hDC);

  // if(nBitsPerPixel <= 8) return GetSysColor(COLOR_BTNFACE);

  // return(CR_BACK);

  return GetSysColor(COLOR_BTNFACE);
}

COLORREF NewGUI_GetBtnColor()
{
  return GetSysColor(COLOR_BTNFACE);
}

void NewGUI_SetImgButtons(BOOL bImageButtons)
{
  ASSERT((bImageButtons == TRUE) || (bImageButtons == FALSE));
  g_bImgButtons = bImageButtons;
}

/* void NewGUI_Button(void *pButton, int nBitmapIn, int nBitmapOut, BOOL bForceImage)
{
  CButtonST *p = (CButtonST *)pButton;
  ASSERT(p != NULL); if(p == NULL) return;

  p->SetFlat(FALSE);

  p->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0), TRUE);
  p->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0), TRUE);

  if((g_bImgButtons == FALSE) && (bForceImage == FALSE)) return;

  if((nBitmapIn != -1) && (nBitmapOut != -1))
    p->SetBitmaps(nBitmapIn, RGB(255, 0, 255), nBitmapOut, RGB(255, 0, 255));
} */

void NewGUI_SetThemeHelper(void *pThemeHelper)
{
  g_pThemeHelper = (CThemeHelperST *)pThemeHelper;

  if(g_pThemeHelper != NULL)
  {
    if(g_pThemeHelper->IsAppThemed() == FALSE)
      g_pThemeHelper = NULL;
  }
}

void NewGUI_XPButton(CXPStyleButtonST& rButton, int nBitmapIn,
  int nBitmapOut, BOOL bForceImage)
{
  if(g_pThemeHelper != NULL) rButton.SetFlat(TRUE);
  else rButton.SetFlat(FALSE);

  if(!((g_bImgButtons == FALSE) && (bForceImage == FALSE)))
  {
    if((nBitmapIn != -1) && (nBitmapOut != -1))
      rButton.SetBitmaps(nBitmapIn, RGB(255, 0, 255),
        nBitmapOut, RGB(255, 0, 255));
  }

  if(g_pThemeHelper != NULL) rButton.SetThemeHelper(g_pThemeHelper);
}

/* Old shade button code:

  CShadeButtonST *p = (CShadeButtonST *)pButton;

  p->DrawFlatFocus(TRUE);
  p->SetFlat(FALSE);

  if(g_bImgButtons == TRUE)
    p->SetShade(CShadeButtonST::SHS_DIAGSHADE);
  else
    p->SetShade(CShadeButtonST::SHS_HBUMP, 8, 0, 255, NewGUI_GetBtnColor());

  p->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0), TRUE);
  p->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 0), TRUE);

  // Pre 0.86 buttons
  // p->SetFlat(TRUE);
  // p->SetShade(CShadeButtonST::SHS_SOFTBUMP);
  // p->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(0, 0, 0), TRUE);
  // p->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 0, 255), TRUE);
  // p->DrawFlatFocus(TRUE);

  if((g_bImgButtons == FALSE) && (bForceImage == FALSE)) return;

  // if(g_bImgButtons == FALSE) return;

  if((nBitmapIn != -1) && (nBitmapOut != -1))
    p->SetBitmaps(nBitmapIn, RGB(255, 0, 255), nBitmapOut, RGB(255, 0, 255));
*/

/*
COLORREF NewGUI_LightenColor(COLORREF crColor, double dblFactor)
{
  BYTE byRed, byGreen, byBlue, byLightRed, byLightGreen, byLightBlue;

  byRed = GetRValue(crColor);
  byGreen = GetGValue(crColor);
  byBlue = GetBValue(crColor);

  byLightRed = (BYTE)(dblFactor * (double)byRed);
  byLightGreen = (BYTE)(dblFactor * (double)byGreen);
  byLightBlue = (BYTE)(dblFactor * (double)byBlue);

  crColor = RGB(byLightRed, byLightGreen, byLightBlue);

  return crColor;
}
*/

void NewGUI_ToolBarButton(void *pButton, int nBitmapIn, int nBitmapOut)
{
  CXPStyleButtonST *p = (CXPStyleButtonST *)pButton;
  CString strToolTip;

  ASSERT(p != NULL); if(p == NULL) return;

  p->SetFlat(TRUE);
  // p->DrawTransparent();

  if((nBitmapIn != -1) && (nBitmapOut != -1))
    p->SetBitmaps(nBitmapIn, RGB(255,0,255), nBitmapOut, RGB(255,0,255));

  p->GetWindowText(strToolTip);
  p->SetWindowText(_T(""));

  strToolTip = TRL_VAR(strToolTip);
  RemoveAcceleratorTip(&strToolTip);
  p->SetTooltipText(strToolTip);

  if(g_pThemeHelper == NULL)
  {
    COLORREF crHighlight;
    BYTE byRed, byGreen, byBlue;
    p->GetColor(CButtonST::BTNST_COLOR_BK_IN, &crHighlight);
    byRed = (BYTE)((GetRValue(crHighlight) < 240) ? GetRValue(crHighlight) + 15 : 255);
    byGreen = (BYTE)((GetGValue(crHighlight) < 240) ? GetGValue(crHighlight) + 15 : 255);
    byBlue = (BYTE)((GetBValue(crHighlight) < 240) ? GetBValue(crHighlight) + 15 : 255);
    crHighlight = RGB(byRed, byGreen, byBlue);
    p->SetColor(CButtonST::BTNST_COLOR_BK_IN, crHighlight, TRUE);
  }

  p->SetThemeHelper(g_pThemeHelper);
  p->DrawAsToolbar(TRUE);
}

void NewGUI_TranslateCWnd(CWnd *pWnd)
{
  if(pWnd == NULL) { ASSERT(FALSE); return; }

  CString str;
  pWnd->GetWindowText(str);

  LPCTSTR lpEng = str;
  LPCTSTR lpTrl = TRL_VAR(lpEng);
  if(lpTrl != lpEng) pWnd->SetWindowText(lpTrl);

#ifdef _DEBUG
  LPCTSTR lpTest = _T("9E8FE196-55C7-4540-A37E-D1B326202073");
  ASSERT(TRL_VAR(lpTest) == lpTest); // Should return same pointer
#endif
}

BOOL CALLBACK NewGUI_TranslateWindowCb(HWND hWnd, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  if(hWnd == NULL) { ASSERT(FALSE); return TRUE; }

  const int cc = 512;
  TCHAR sz[cc];
  sz[cc - 1] = _T('\0');

  sz[0] = _T('\0');
  GetClassName(hWnd, sz, 16);
  if(_tcsicmp(sz, _T("Edit")) == 0) return TRUE;
  if(_tcsicmp(sz, _T("RICHEDIT")) == 0) return TRUE;
  if(_tcsicmp(sz, _T("ComboBox")) == 0) return TRUE;
  if(_tcsicmp(sz, _T("ComboBoxEx32")) == 0) return TRUE;

  sz[0] = _T('\0');
  const int ccText = GetWindowText(hWnd, sz, cc - 1);
  if((ccText < 0) || (ccText > (cc - 1))) { ASSERT(FALSE); return TRUE; }

  if(ccText != 0)
  {
    sz[ccText] = _T('\0');

    LPCTSTR lpTrl = TRL_VAR(sz);
    if(lpTrl != sz) SetWindowText(hWnd, lpTrl);
  }
  return TRUE;
}

void NewGUI_ConfigQualityMeter(CQualityProgressCtrl *pWnd)
{
  if(pWnd == NULL) { ASSERT(FALSE); return; }
  // p->SetStartColor(RGB(255, 128, 0));
  // p->SetEndColor(RGB(0, 255, 0));
  pWnd->SetStep(1);
  pWnd->SetRange(0, 128);
  pWnd->SetPos(0);
}

void NewGUI_ShowQualityMeter(CQualityProgressCtrl *pProgress, CWnd *pStatic,
  LPCTSTR pszPassword)
{
  if(pProgress == NULL) { ASSERT(FALSE); return; }
  if(pStatic == NULL) { ASSERT(FALSE); return; }
  if(pszPassword == NULL) { ASSERT(FALSE); return; }

  const size_t uLength = _tcslen(pszPassword);
  DWORD dwBits = CPwQualityEst::EstimatePasswordBits(pszPassword);
  if(dwBits > 9999) dwBits = 9999; // 4 characters display limit

  CString strQuality;
  strQuality.Format(_T("%u"), dwBits);
  strQuality += _T(" ");
  strQuality += TRL("bits");

  CString strLength;
  strLength.Format(_T("%u"), uLength);
  strLength += _T(" ");
  strLength += TRL("ch.");

  if(dwBits > 128) dwBits = 128;
  pProgress->SetProgressText(strQuality);
  pProgress->SetPos((int)dwBits);

  pStatic->SetWindowText(strLength);
}

void NewGUI_ConfigSideBanner(void *pBanner, void *pParentWnd)
{
  CKCSideBannerWnd *p = (CKCSideBannerWnd *)pBanner;
  CWnd *pParent = (CWnd *)pParentWnd;
  ASSERT(pBanner != NULL); if(pBanner == NULL) return;
  ASSERT(pParentWnd != NULL); if(pParentWnd == NULL) return;

  p->Attach(pParent, KCSB_ATTACH_TOP);

  const int nSize = p->GetSize();
  const int nNewSize = NewGUI_Scale(nSize, pParent);
  if(nNewSize != nSize) p->SetSize(nNewSize);

  const CSize sEdgeOffset = p->GetEdgeOffset();
  CSize sNewEdgeOffset = NewGUI_Scale(sEdgeOffset, pParent);
  if(sNewEdgeOffset != sEdgeOffset) p->SetEdgeOffset(sNewEdgeOffset);

  const CSize sCaptionOffset = p->GetCaptionOffset();
  CSize sNewCaptionOffset = NewGUI_Scale(sCaptionOffset, pParent);
  if(sNewCaptionOffset != sCaptionOffset) p->SetCaptionOffset(sNewCaptionOffset);

  p->SetColBkg(m_crBannerStart);
  p->SetColBkg2(m_crBannerEnd);

  p->SetColTxtTitle(m_crBannerText);
  p->SetColTxtCaption(m_crBannerText);

  p->SetColEdge(RGB(0, 0, 0));

  if(m_bBannerFlip) p->SetSwapGradientDirection(true);

  if(m_pfBannerTitleFont != NULL) p->SetTitleFont(m_pfBannerTitleFont);
  else
  {
    LOGFONT lf;
    ZeroMemory(&lf, sizeof(LOGFONT));
    p->GetTitleFont(&lf);
    lf.lfWeight = FW_BOLD;

    m_pfBannerTitleFont = new CFont();
    VERIFY(m_pfBannerTitleFont->CreateFontIndirect(&lf));

    p->SetTitleFont(m_pfBannerTitleFont);
  }
}

BOOL NewGUI_GetHeaderOrder(HWND hwListCtrl, INT *pOrder, INT nColumnCount)
{
  HWND hHeader;

  ASSERT(hwListCtrl != NULL); if(hwListCtrl == NULL) return FALSE;
  ASSERT(pOrder != NULL); if(pOrder == NULL) return FALSE;

  hHeader = (HWND)SendMessage(hwListCtrl, LVM_GETHEADER, 0, 0);
  ASSERT(hHeader != NULL); if(hHeader == NULL) return FALSE;

  if(SendMessage(hHeader, HDM_GETORDERARRAY, (WPARAM)nColumnCount, (LPARAM)pOrder) == FALSE)
  {
    ASSERT(FALSE); return FALSE;
  }

  return TRUE;
}

BOOL NewGUI_SetHeaderOrder(HWND hwListCtrl, INT *pOrder, INT nColumnCount)
{
  HWND hHeader;

  ASSERT(hwListCtrl != NULL); if(hwListCtrl == NULL) return FALSE;
  ASSERT(pOrder != NULL); if(pOrder == NULL) return FALSE;

  hHeader = (HWND)SendMessage(hwListCtrl, LVM_GETHEADER, 0, 0);
  ASSERT(hHeader != NULL); if(hHeader == NULL) return FALSE;

  if(SendMessage(hHeader, HDM_SETORDERARRAY, (WPARAM)nColumnCount, (LPARAM)pOrder) == FALSE)
  {
    ASSERT(FALSE); return FALSE;
  }

  return TRUE;
}

void NewGUI_MakeHyperLink(void *pXHyperLink)
{
  CXHyperLink *p = reinterpret_cast<CXHyperLink *>(pXHyperLink);
  ASSERT(p != NULL); if(p == NULL) return;

  p->SetVisited(FALSE);
  p->SetAutoSize(TRUE);
  p->SetUnderline(CXHyperLink::ulAlways);
  p->SetColours(RGB(0,0,255), RGB(0,0,255), RGB(100,100,255));
}

void NewGUI_DisableHideWnd(CWnd *pWnd)
{
  ASSERT(pWnd != NULL); if(pWnd == NULL) return;

  pWnd->EnableWindow(FALSE);
  pWnd->ShowWindow(SW_HIDE);
}

void NewGUI_MoveWnd(CWnd *pWnd, long lMoveRightPixels, long lMoveDownPixels,
  CWnd *pParent)
{
  ASSERT(pWnd != NULL); if(pWnd == NULL) return;
  ASSERT(pParent != NULL); if(pParent == NULL) return;

  RECT rect;
  pWnd->GetWindowRect(&rect);
  pParent->ScreenToClient(&rect);

  rect.left += lMoveRightPixels;
  rect.right += lMoveRightPixels;
  rect.top += lMoveDownPixels;
  rect.bottom += lMoveDownPixels;

  pWnd->MoveWindow(&rect);
}

void NewGUI_Resize(CWnd *pWnd, long lAddX, long lAddY, CWnd *pParent)
{
  ASSERT(pWnd != NULL); if(pWnd == NULL) return;

  if(pParent == pWnd) { ASSERT(FALSE); pParent = NULL; }

  RECT rect;
  pWnd->GetWindowRect(&rect);

  if(pParent != NULL) pParent->ScreenToClient(&rect);

  rect.right += lAddX;
  rect.bottom += lAddY;

  pWnd->MoveWindow(&rect);
}

void NewGUI_SetBannerColors(COLORREF crStart, COLORREF crEnd, COLORREF crText)
{
  ASSERT(sizeof(COLORREF) == sizeof(DWORD));

  if(crStart != DWORD_MAX)
    m_crBannerStart = crStart;
  if(crEnd != DWORD_MAX)
    m_crBannerEnd = crEnd;
  if(crText != DWORD_MAX)
    m_crBannerText = crText;
}

void NewGUI_SetBannerParams(bool bBannerFlip)
{
  m_bBannerFlip = bBannerFlip;
}

BOOL NewGUI_RemoveMenuCommand(BCMenu *pMenu, UINT uCommandID)
{
  ASSERT(pMenu != NULL); if(pMenu == NULL) return FALSE;

  return pMenu->DeleteMenu(uCommandID, MF_BYCOMMAND);
}

void NewGUI_RemoveInvalidSeparators(BCMenu *pMenu, BOOL bIsTopLevel)
{
  ASSERT(pMenu != NULL); if(pMenu == NULL) return;

  int nSub = 0;
  while(true)
  {
    BCMenu *pSub = NewGUI_GetBCMenu(pMenu->GetSubMenu(nSub));
    if(pSub == NULL) break;

    NewGUI_RemoveInvalidSeparators(pSub, FALSE);
    ++nSub;
  }

  BOOL bSepAllowed = FALSE;
  UINT uCount = pMenu->GetMenuItemCount();
  for(UINT uItem = 0; uItem < uCount; ++uItem)
  {
    if(uItem == (uCount - 1)) bSepAllowed = FALSE;

    const UINT uState = pMenu->GetMenuState(uItem, MF_BYPOSITION);
    if((bIsTopLevel == FALSE) && ((uState & MF_SEPARATOR) != 0) &&
      ((uState & MF_POPUP) == 0))
    {
      if(bSepAllowed == FALSE)
      {
        pMenu->DeleteMenu(uItem, MF_BYPOSITION);
        --uCount;
        --uItem;

        if((uItem == (uCount - 1)) && (uItem >= 1)) --uItem;
      }

      bSepAllowed = FALSE;
    }
    else bSepAllowed = TRUE;
  }
}

BCMenu *NewGUI_GetBCMenu(CMenu *pMenu)
{
  if(pMenu == NULL) return NULL; // No assert

  BCMenu *p = NULL;
  try { p = dynamic_cast<BCMenu *>(pMenu); }
  catch(...) { ASSERT(FALSE); p = NULL; }

  return p;
}

CSize NewGUI_GetWndBasePosDiff(CWnd *pWnd1, CWnd *pWnd2)
{
  ASSERT(pWnd1 != NULL); if(pWnd1 == NULL) return CSize(0, 0);
  ASSERT(pWnd2 != NULL); if(pWnd2 == NULL) return CSize(0, 0);

  RECT rect1, rect2;
  pWnd1->GetWindowRect(&rect1);
  pWnd2->GetWindowRect(&rect2);

  return CSize(abs(rect1.left - rect2.left), abs(rect1.top - rect2.top));
}

void NewGUI_SetCueBanner_TB(HWND hTextBox, LPCTSTR lpText)
{
  ASSERT(lpText != NULL); if(lpText == NULL) return;

  // On Windows XP there's a drawing bug at the left border (text is
  // not displayed correctly), therefore prepend a space on Windows XP
  CString strSearchTr = ((AU_IsAtLeastWinVistaSystem() == FALSE) ? _T(" ") : _T(""));
  strSearchTr += lpText;

#ifndef _UNICODE
  LPCWSTR pSearchUni = _StringToUnicode(strSearchTr);
#else // Unicode
  LPCWSTR pSearchUni = strSearchTr;
#endif

  ::SendMessage(hTextBox, EM_SETCUEBANNER, 0, (LPARAM)pSearchUni);

#ifndef _UNICODE
  SAFE_DELETE_ARRAY(pSearchUni);
#endif
}

void NewGUI_SetCueBanner_CB(HWND hComboBox, LPCTSTR lpText)
{
  HWND hTextBox = NULL;
  NewGUI_ComboBox_GetInfo(hComboBox, NULL, &hTextBox, NULL);
  NewGUI_SetCueBanner_TB(hTextBox, lpText);
}

void NewGUI_ComboBox_GetInfo(HWND hComboBox, HWND* phComboBox,
  HWND* phEditBox, HWND* phListBox)
{
  COMBOBOXINFO cbi;
  ZeroMemory(&cbi, sizeof(COMBOBOXINFO));
  cbi.cbSize = sizeof(COMBOBOXINFO);
  VERIFY(GetComboBoxInfo(hComboBox, &cbi));

  if(phComboBox != NULL) *phComboBox = cbi.hwndCombo;
  if(phEditBox != NULL) *phEditBox = cbi.hwndItem;
  if(phListBox != NULL) *phListBox = cbi.hwndList;
}

bool NewGUI_ComboBox_HasFocus(HWND hComboBox, HWND hCurrentFocus)
{
  HWND hCombo = NULL;
  HWND hTextBox = NULL;
  HWND hListBox = NULL;
  NewGUI_ComboBox_GetInfo(hComboBox, &hCombo, &hTextBox, &hListBox);

  return ((hCombo == hCurrentFocus) || (hTextBox == hCurrentFocus) ||
    (hListBox == hCurrentFocus));
}

void NewGUI_ComboBox_UpdateHistory(CComboBox& comboBox,
  const std::basic_string<TCHAR>& strNew,
  std::vector<std::basic_string<TCHAR> >* pvHistoryItems,
  size_t dwMaxHistoryItems)
{
  ASSERT(pvHistoryItems != NULL); if(pvHistoryItems == NULL) return;

  if(strNew.size() > 0)
  {
    std::vector<std::basic_string<TCHAR> >::iterator itExists =
      std::find(pvHistoryItems->begin(), pvHistoryItems->end(), strNew);

    if(itExists != pvHistoryItems->end())
      pvHistoryItems->erase(itExists);

    size_t dwCurSize = pvHistoryItems->size();
    ASSERT(dwCurSize <= dwMaxHistoryItems);
    while(dwCurSize >= dwMaxHistoryItems)
    {
      pvHistoryItems->erase(pvHistoryItems->begin());

      if(dwCurSize == pvHistoryItems->size()) { ASSERT(FALSE); break; }
      dwCurSize = pvHistoryItems->size();
    }

    pvHistoryItems->push_back(strNew);
  }

  ASSERT(pvHistoryItems->size() <= dwMaxHistoryItems);

  const int nOrgCount = comboBox.GetCount();
  for(int n = 0; n < nOrgCount; ++n)
  {
    const UINT uIndex = static_cast<UINT>(nOrgCount - n - 1);
    VERIFY(comboBox.DeleteString(uIndex) != CB_ERR);
  }

  for(size_t i = 0; i < pvHistoryItems->size(); ++i)
  {
    const size_t iIndex = pvHistoryItems->size() - i - 1;
    comboBox.AddString(pvHistoryItems->at(iIndex).c_str());
  }

  if(comboBox.GetCount() > 0)
  {
    comboBox.AddString(HCMBX_SEPARATOR);
    comboBox.AddString(HCMBX_CLEARLIST);
  }
}

BOOL NewGUI_SetIcon(BCMenu& rMenu, UINT uCommand, int nResourceID)
{
  CString strMenuItemText;
  if(rMenu.GetMenuText(uCommand, strMenuItemText, MF_BYCOMMAND) == FALSE)
    return FALSE;
  if(strMenuItemText.GetLength() == 0) return FALSE;

  if(rMenu.ModifyODMenu(NULL, uCommand, nResourceID) == FALSE)
    return FALSE;

  return TRUE;
}

int CALLBACK NgslCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  std::pair<CListCtrl*, int>* pParam = (std::pair<CListCtrl*, int>*)lParamSort;
  if(pParam == NULL) { ASSERT(FALSE); return 0; }

  CListCtrl* pCtrl = pParam->first;

  ASSERT(pCtrl->GetHeaderCtrl()->GetItemCount() == pParam->second);
  for(int iColumn = 0; iColumn < pParam->second; ++iColumn)
  {
    CString x = pCtrl->GetItemText((int)lParam1, iColumn);
    CString y = pCtrl->GetItemText((int)lParam2, iColumn);

    const int nCmp = x.Compare(y);
    if(nCmp != 0) return nCmp;
  }

  return 0;
}

void NewGUI_SortList(CListCtrl* pListCtrl)
{
  if(pListCtrl == NULL) { ASSERT(FALSE); return; }

  std::pair<CListCtrl*, int> spParam(pListCtrl,
    pListCtrl->GetHeaderCtrl()->GetItemCount());

  VERIFY(ListView_SortItemsEx(pListCtrl->m_hWnd, NgslCompareFunc,
    (LPARAM)&spParam) != FALSE);
}

void NewGUI_AppendToRichEditCtrl(CAutoRichEditCtrlFx* pCtrl, LPCTSTR lpAppend,
  bool bScrollToBottom)
{
  if(pCtrl == NULL) { ASSERT(FALSE); return; }
  if(lpAppend == NULL) { ASSERT(FALSE); return; }
  if(lpAppend[0] == 0) return;

  CString strText;
  pCtrl->GetWindowText(strText);
  if(strText.GetLength() > 0) strText += _T("\r\n\r\n");

  strText += lpAppend;

  pCtrl->SetRTF(strText, SF_TEXT);

  if(bScrollToBottom) pCtrl->SendMessage(WM_VSCROLL, SB_BOTTOM, NULL);
}

void NewGUI_DeselectAllItems(CListCtrl* pCtrl)
{
  if(pCtrl == NULL) { ASSERT(FALSE); return; }

  for(int i = 0; i < pCtrl->GetItemCount(); ++i)
  {
    const UINT uState = pCtrl->GetItemState(i, LVIS_SELECTED);
    if((uState & LVIS_SELECTED) != 0)
    {
      VERIFY(pCtrl->SetItemState(i, 0, LVIS_SELECTED));
    }
  }
}

void NewGUI_EnableSortHeader(CListCtrl* pCtrl, bool bEnable)
{
  if(pCtrl == NULL) { ASSERT(FALSE); return; }

  CHeaderCtrl* pH = pCtrl->GetHeaderCtrl();
  if(pH == NULL) { ASSERT(FALSE); return; }
  HWND h = pH->m_hWnd;
  if(h == NULL) { ASSERT(FALSE); return; }

  const LONG_PTR lStyle = GetWindowLongPtr(h, GWL_STYLE);
  const bool bCur = ((lStyle & HDS_BUTTONS) != 0);

  if(bCur != bEnable)
    SetWindowLongPtr(h, GWL_STYLE, lStyle ^ HDS_BUTTONS);
}

void NewGUI_SetShield(CButton& rButton, BOOL bSetShield)
{
  rButton.SendMessage(BCM_SETSHIELD, 0, bSetShield);
}

int NewGUI_Scale(int nPixels, CWnd* pParentWindow)
{
  if(pParentWindow == NULL) { ASSERT(FALSE); return nPixels; }

  CDC* pDC = pParentWindow->GetDC();
  CFont* pFont = pParentWindow->GetFont();
  if((pDC == NULL) || (pFont == NULL)) { ASSERT(FALSE); return nPixels; }

  const double dDpi = static_cast<double>(GetDeviceCaps(pDC->m_hDC, LOGPIXELSY));
  const double dScaleDpi = (dDpi / 96.0);

  LOGFONT lf;
  ZeroMemory(&lf, sizeof(LOGFONT));
  if(pFont->GetLogFont(&lf) == 0) { ASSERT(FALSE); return nPixels; }

  const double dy = fabs((72.0 * static_cast<double>(lf.lfHeight)) / dDpi);
  const double dScaleFont = (dy / 8.25);

  return static_cast<int>(floor((static_cast<double>(nPixels) *
    dScaleDpi * dScaleFont) + 0.5));
}

CSize NewGUI_Scale(const CSize& rSize, CWnd* pParentWindow)
{
  CSize sNew;
  sNew.cx = NewGUI_Scale(rSize.cx, pParentWindow);
  sNew.cy = NewGUI_Scale(rSize.cy, pParentWindow);
  return sNew;
}

COLORREF NewGUI_ColorToGrayscale(COLORREF clr)
{
  int l = static_cast<int>((0.3f * static_cast<float>(GetRValue(clr))) +
    (0.59f * static_cast<float>(GetGValue(clr))) +
    (0.11f * static_cast<float>(GetBValue(clr))));

  if(l < 0) l = 0;
  else if(l >= 256) l = 255;

  return RGB(l, l, l);
}

COLORREF NewGUI_ColorMid(COLORREF clrA, COLORREF clrB)
{
  const int r = ((int)GetRValue(clrA) + (int)GetRValue(clrB)) / 2;
  const int g = ((int)GetGValue(clrA) + (int)GetGValue(clrB)) / 2;
  const int b = ((int)GetBValue(clrA) + (int)GetBValue(clrB)) / 2;
  return RGB(r, g, b);
}

void NewGUI_InitGDIPlus()
{
  Gdiplus::GdiplusStartupInput si;
  if(Gdiplus::GdiplusStartup(&g_gdiplusToken, &si, NULL) == Gdiplus::Ok)
    g_gdiplusInitialized = true;
}

void NewGUI_TerminateGDIPlus()
{
  if(g_gdiplusInitialized)
  {
    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    g_gdiplusInitialized = false;
  }
}

void NewGUI_PumpMessages(HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
  MSG msg;
  while(::PeekMessage(&msg, hWnd, wMsgFilterMin, wMsgFilterMax, PM_REMOVE))
  {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
}

static CCriticalSection g_csDoModalRoot;
INT_PTR NewGUI_DoModal(CDialog* pDlg)
{
  if(pDlg == NULL) { ASSERT(FALSE); return IDCANCEL; }

  CPwSafeDlg* pRootDlg = (CPwSafeDlg*)KPMI_GetMainDialog();
  ASSERT(pRootDlg != NULL);

  VERIFY(g_csDoModalRoot.Lock() != FALSE);
  if(pRootDlg != NULL)
  {
    pRootDlg->NotifyUserActivity();
    pRootDlg->_SetDisplayDialog(true);
  }
  VERIFY(CGlobalWindowManager::AddDialog(pDlg) == S_OK);
  VERIFY(g_csDoModalRoot.Unlock() != FALSE);

  const INT_PTR r = pDlg->DoModal();

  VERIFY(g_csDoModalRoot.Lock() != FALSE);
  VERIFY(CGlobalWindowManager::RemoveDialog(pDlg) == S_OK);
  if(pRootDlg != NULL)
  {
    pRootDlg->NotifyUserActivity();
    pRootDlg->_SetDisplayDialog(false);
  }
  VERIFY(g_csDoModalRoot.Unlock() != FALSE);

  return r;
}

SIZE NewGUI_GetWindowContentSize(HWND hWnd)
{
  SIZE sz;
  ZeroMemory(&sz, sizeof(SIZE));

  if(hWnd == NULL) { ASSERT(FALSE); return sz; }

  RECT rect;
  ZeroMemory(&rect, sizeof(RECT));
  if(GetClientRect(hWnd, &rect) != FALSE)
  {
    sz.cx = rect.right;
    sz.cy = rect.bottom;
  }
  if((sz.cx <= 0) || (sz.cy <= 0)) { ASSERT(FALSE); return sz; }

  MENUBARINFO mbi;
  ZeroMemory(&mbi, sizeof(MENUBARINFO));
  mbi.cbSize = sizeof(MENUBARINFO);
  if(GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi) != FALSE)
  {
    if(mbi.hMenu != NULL)
    {
      const int dx = mbi.rcBar.right - mbi.rcBar.left;
      const int dy = mbi.rcBar.bottom - mbi.rcBar.top;

      if(dx >= dy) sz.cy += dy; // Horizontal menu bar
      else sz.cx += dx; // Vertical menu bar
    }
  }

  return sz;
}

/* void NewGUI_ResetFocus(CWnd* pToFocus)
{
  if(pToFocus == NULL) { ASSERT(FALSE); return; }

  HWND h = pToFocus->m_hWnd;
  if(h == NULL) { ASSERT(FALSE); return; }

  HWND hPre = GetFocus();

  bool bStdSetFocus = true;
  if(h == hPre)
  {
    // Special reset for password text boxes that
    // can show a Caps Lock balloon tip;
    // https://sourceforge.net/p/keepass/feature-requests/1905/
    TCHAR tszClass[256];
    ZeroMemory(tszClass, 256 * sizeof(TCHAR));
    if(GetClassName(h, tszClass, 254) > 0)
    {
      const LONG_PTR lStyle = GetWindowLongPtr(h, GWL_STYLE);
      const LONG_PTR lReqSet = ES_PASSWORD;
      const LONG_PTR lReqUnset = ES_READONLY;

      const bool bCapsLock = ((GetKeyState(VK_CAPITAL) & 1) != 0);

      if((_tcsicmp(tszClass, WC_EDIT) == 0) &&
        ((lStyle & lReqSet) == lReqSet) &&
        ((lStyle & lReqUnset) == 0) && bCapsLock)
      {
        SendMessage(h, WM_KILLFOCUS, 0, 0);
        SendMessage(h, WM_SETFOCUS, 0, 0);

        bStdSetFocus = false;
      }
    }
  }

  if(bStdSetFocus) pToFocus->SetFocus();
} */

BOOL NewGUI_GetNonClientMetrics(NONCLIENTMETRICS* p)
{
  if(p == NULL) { ASSERT(FALSE); return FALSE; }

  UINT cbSize = sizeof(NONCLIENTMETRICS);
  ZeroMemory(p, cbSize);

  // See the documentation of the NONCLIENTMETRICS structure:
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ff729175.aspx
  // https://www.codeproject.com/Messages/3989684/Compile-with-dev-studio-running-on-Windows-XP.aspx
#if (WINVER >= 0x0600)                                              // rrvt 7/5/25
  if(AU_IsAtLeastWinVistaSystem() == FALSE)
  {
    BOOST_STATIC_ASSERT(sizeof(p->iPaddedBorderWidth) == sizeof(int));
    cbSize -= sizeof(int);
  }
#else
  // Verify that the size computation above is still correct with the
  // latest NONCLIENTMETRICS definition, then update the WINVER comparison
  BOOST_STATIC_ASSERT(false);
#endif

  p->cbSize = cbSize;
  return SystemParametersInfo(SPI_GETNONCLIENTMETRICS, cbSize, p, 0);
}

void NewGUI_CustomizeWindow(CWnd* pWnd, bool bInit)
{
  UNREFERENCED_PARAMETER(bInit);
  if(pWnd == NULL) { ASSERT(FALSE); return; }

  HWND hWnd = pWnd->m_hWnd;
  if(hWnd == NULL) { ASSERT(FALSE); return; }

  if(g_lpSetWindowDisplayAffinity != NULL)
  {
    const bool bP = (g_bPreventScreenCapture != FALSE);
    if(bP || g_bDisplayAffChanged)
    {
      g_lpSetWindowDisplayAffinity(hWnd, (bP ? WDA_MONITOR : WDA_NONE));
      g_bDisplayAffChanged = true;
    }
  }
  else { ASSERT(AU_IsAtLeastWin7System() == FALSE); }
}
