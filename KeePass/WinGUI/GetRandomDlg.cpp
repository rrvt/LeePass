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
#include "GetRandomDlg.h"

#include "NewGUI/NewGUICommon.h"
#include "../KeePassLibCpp/Util/NewRandom.h"
#include "../KeePassLibCpp/Util/TranslateEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CGetRandomDlg::CGetRandomDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetRandomDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetRandomDlg)
	m_strRandom = _T("");
	//}}AFX_DATA_INIT
}

void CGetRandomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetRandomDlg)
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDC_BEGIN_MOUSEINPUT, m_btMouseInput);
	DDX_Control(pDX, IDC_PROGRESS_MOUSE, m_cMouseProgress);
	DDX_Text(pDX, IDC_EDIT_RANDOMSTRING, m_strRandom);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGetRandomDlg, CDialog)
	//{{AFX_MSG_MAP(CGetRandomDlg)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BEGIN_MOUSEINPUT, OnBeginMouseInput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CGetRandomDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	NewGUI_InitDialog(this);

	m_cMouseProgress.SetRange(0, 100);
	m_cMouseProgress.SetPos(0);

	m_dwCurPoint = 0;
	m_bMouseActive = FALSE;

	NewGUI_XPButton(m_btOK, IDB_OK, IDB_OK);
	NewGUI_XPButton(m_btCancel, IDB_CANCEL, IDB_CANCEL);
	NewGUI_XPButton(m_btMouseInput, IDB_MOUSE_SMALL, IDB_MOUSE_SMALL);

	NewGUI_ConfigSideBanner(&m_banner, this);
	m_banner.SetIcon(AfxGetApp()->LoadIcon(IDI_KEY),
		KCSB_ICON_LEFT | KCSB_ICON_VCENTER);
	m_banner.SetTitle(TRL("Get Random Data for Key Generation"));
	m_banner.SetCaption(TRL("Move the mouse and type some random text to generate random values."));

	CNewRandom cRand;
	cRand.GetRandomBuffer(m_pFinalRandom, 32);
	cRand.GetRandomBuffer((BYTE *)m_points, sizeof(POINT) * 100);

	return TRUE;
}

void CGetRandomDlg::OnOK() 
{
	sha256_ctx ctx;
	int nStringLen;

	UpdateData(TRUE);

	if((m_dwCurPoint == 0) && (m_strRandom.GetLength() == 0))
	{
		MessageBox(TRL("You must generate some random values using the random mouse input box or the edit field before you can close this dialog."),
			TRL("No random input"), MB_OK | MB_ICONWARNING);
		return;
	}

	sha256_begin(&ctx);
	sha256_hash(m_pFinalRandom, 32, &ctx);

	sha256_hash((unsigned char *)m_points, sizeof(POINT) * 100, &ctx);

	nStringLen = m_strRandom.GetLength();
	if(nStringLen != 0)
		sha256_hash((BYTE *)(LPCTSTR)m_strRandom, (unsigned long)nStringLen, &ctx);

	sha256_end((unsigned char *)m_pFinalRandom, &ctx);

	CDialog::OnOK();
}

void CGetRandomDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bMouseActive == TRUE)
	{
		if(PtInRect(&m_rect, point))
		{
			if(m_dwCurPoint < 100)
			{
				if((rand() % 5) == 0)
				{
					m_points[m_dwCurPoint] = (POINT)point;
					m_dwCurPoint++;
					m_cMouseProgress.SetPos(m_dwCurPoint);
				}
			}
			else
			{
				m_bMouseActive = FALSE;

				m_btMouseInput.EnableWindow(TRUE);
				m_btMouseInput.SetFocus();
			}
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CGetRandomDlg::OnBeginMouseInput() 
{
	GetDlgItem(IDC_STATIC_RANDOM)->GetWindowRect(&m_rect);

	const int x = m_rect.left + ((m_rect.right - m_rect.left) / 2);
	const int y = m_rect.top + ((m_rect.bottom - m_rect.top) / 2);

	SetCursorPos(x, y);

	ScreenToClient(&m_rect);

	m_cMouseProgress.SetPos(0);

	m_btMouseInput.EnableWindow(FALSE);

	m_dwCurPoint = 0;
	m_bMouseActive = TRUE;
}

void CGetRandomDlg::OnCancel() 
{
	CDialog::OnCancel();
}
