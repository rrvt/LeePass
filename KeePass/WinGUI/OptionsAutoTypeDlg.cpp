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
#include "PwSafe.h"
#include "OptionsAutoTypeDlg.h"

#include "../KeePassLibCpp/Util/TranslateEx.h"

IMPLEMENT_DYNAMIC(COptionsAutoTypeDlg, CDialog)

COptionsAutoTypeDlg::COptionsAutoTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsAutoTypeDlg::IDD, pParent)
	, m_bEnableAT(FALSE)
	, m_bAlternative(FALSE)
	, m_strDefaultSeq(_T(""))
	, m_bIEFix(FALSE)
	, m_bSortAutoTypeSelItems(TRUE)
	, m_bSameKL(TRUE)
	, m_bNormDashes(TRUE)
{
}

COptionsAutoTypeDlg::~COptionsAutoTypeDlg()
{
}

void COptionsAutoTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_ENABLEAT, m_bEnableAT);
	DDX_Check(pDX, IDC_CHECK_ALTERNATIVE, m_bAlternative);
	DDX_Text(pDX, IDC_EDIT_STDSEQ, m_strDefaultSeq);
	DDX_Check(pDX, IDC_CHECK_IEFIX, m_bIEFix);
	DDX_Control(pDX, IDC_CHECK_ALTERNATIVE, m_cbAlternative);
	DDX_Control(pDX, IDC_EDIT_STDSEQ, m_tbDefaultSeq);
	DDX_Control(pDX, IDC_CHECK_IEFIX, m_cbIEFix);
	DDX_Control(pDX, IDC_CHECK_ENABLEAT, m_cbEnableAT);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_HOTKEY_AUTOTYPE, m_hkAutoType);
	DDX_Control(pDX, IDC_CHECK_SORTATITEMS, m_cbSortATItems);
	DDX_Check(pDX, IDC_CHECK_SORTATITEMS, m_bSortAutoTypeSelItems);
	DDX_Check(pDX, IDC_CHECK_SAMEKL, m_bSameKL);
	DDX_Control(pDX, IDC_CHECK_SAMEKL, m_cbSameKL);
	DDX_Check(pDX, IDC_CHECK_NORMDASHES, m_bNormDashes);
	DDX_Control(pDX, IDC_CHECK_NORMDASHES, m_cbNormDashes);
}

BEGIN_MESSAGE_MAP(COptionsAutoTypeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COptionsAutoTypeDlg::OnBtnOK)
	ON_BN_CLICKED(IDCANCEL, &COptionsAutoTypeDlg::OnBtnCancel)
	ON_BN_CLICKED(IDC_CHECK_ENABLEAT, &COptionsAutoTypeDlg::OnCheckEnableAT)
	ON_EN_CHANGE(IDC_EDIT_STDSEQ, &COptionsAutoTypeDlg::OnChangeDefaultSeq)
END_MESSAGE_MAP()

BOOL COptionsAutoTypeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	NewGUI_InitDialog(this);

	NewGUI_XPButton(m_btnOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btnCancel, IDB_CANCEL, IDB_CANCEL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_OPTIONS),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Auto-Type"));
	m_banner.SetCaption(TRL("Here you can configure auto-type."));

	m_hkAutoType.SetRules(HKCOMB_NONE | HKCOMB_S, HOTKEYF_CONTROL | HOTKEYF_ALT);
	m_hkAutoType.SetHotKey((WORD)(m_dwATHotKey & 0x0000FFFF), (WORD)(m_dwATHotKey >> 16));

	const std::vector<WCHAR>& vDashes = SU_GetNormDashes();
	if(vDashes.size() >= 3)
	{
		std::basic_string<WCHAR> strW(L" (-, -, -, ...)");
		strW[2] = vDashes[0];
		strW[5] = vDashes[1];
		strW[8] = vDashes[2];

		CString str;
		m_cbNormDashes.GetWindowText(str);

#ifdef _UNICODE
		str += strW.c_str();
#else
		char* lpA = _StringToAnsi(strW.c_str());
		if(lpA != NULL)
		{
			str += lpA;
			SAFE_DELETE_ARRAY(lpA);
		}
		else { ASSERT(FALSE); }
#endif

		m_cbNormDashes.SetWindowText(str);
	}

	m_bEnableAT = ((m_bDisableAutoType == FALSE) ? TRUE : FALSE);
	m_bAlternative = m_bMinimizeBeforeAT;
	m_strDefaultSeq = m_strDefaultAutoTypeSequence;
	m_bIEFix = m_bAutoTypeIEFix;
	m_bNormDashes = m_bAutoTypeNormDashes;

	UpdateData(FALSE);
	EnableChildControls();

	return TRUE;
}

void COptionsAutoTypeDlg::EnableChildControls()
{
	UpdateData(TRUE);

	BOOL bActive = ((m_cbEnableAT.GetCheck() != FALSE) ? TRUE : FALSE);
	m_cbAlternative.EnableWindow(bActive);
	m_tbDefaultSeq.EnableWindow(bActive);
	m_cbIEFix.EnableWindow(bActive);
	m_hkAutoType.EnableWindow(bActive);
	m_cbSameKL.EnableWindow(bActive);
	m_cbSortATItems.EnableWindow(bActive);
	m_cbNormDashes.EnableWindow(bActive);

	m_btnOK.EnableWindow((m_strDefaultSeq.GetLength() > 0) ? TRUE : FALSE);
}

void COptionsAutoTypeDlg::OnBtnOK()
{
	UpdateData(TRUE);

	m_bDisableAutoType = ((m_bEnableAT == FALSE) ? TRUE : FALSE);
	m_bMinimizeBeforeAT = m_bAlternative;
	m_strDefaultAutoTypeSequence = m_strDefaultSeq;
	m_bAutoTypeIEFix = m_bIEFix;
	m_bAutoTypeNormDashes = m_bNormDashes;

	WORD wVK = 0, wMod = 0;
	m_hkAutoType.GetHotKey(wVK, wMod);
	const DWORD dwNewHotKey = ((DWORD)wMod << 16) | (DWORD)wVK;
	if(!m_pParentDlg->RegisterGlobalHotKey(HOTKEYID_AUTOTYPE,
		((m_bDisableAutoType == FALSE) ? dwNewHotKey : 0), true))
		return;
	m_dwATHotKey = dwNewHotKey;

	CDialog::OnOK();
}

void COptionsAutoTypeDlg::OnBtnCancel()
{
	CDialog::OnCancel();
}

void COptionsAutoTypeDlg::OnCheckEnableAT()
{
	EnableChildControls();
}

void COptionsAutoTypeDlg::OnChangeDefaultSeq()
{
	EnableChildControls();
}
