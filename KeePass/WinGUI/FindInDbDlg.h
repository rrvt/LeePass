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

#ifndef AFX_FINDINDBDLG_H__BE7DB380_15E6_11D8_BF16_0050BF14F5CC__INCLUDED_
#define AFX_FINDINDBDLG_H__BE7DB380_15E6_11D8_BF16_0050BF14F5CC__INCLUDED_

#include "NewGUI/KCSideBannerWnd.h"
#include "NewGUI/XPStyleButtonST.h"

/////////////////////////////////////////////////////////////////////////////

class CFindInDbDlg : public CDialog
{
public:
	CFindInDbDlg(CWnd* pParent = NULL);

	CKCSideBannerWnd m_banner;

	LPCTSTR m_lpGroupName;

	//{{AFX_DATA(CFindInDbDlg)
	enum { IDD = IDD_FIND_DLG };
	CXPStyleButtonST	m_btOK;
	CXPStyleButtonST	m_btCancel;
	BOOL	m_bAdditional;
	BOOL	m_bPassword;
	BOOL	m_bTitle;
	BOOL	m_bURL;
	BOOL	m_bUserName;
	BOOL	m_bUUID;
	CString	m_strFind;
	BOOL	m_bCaseSensitive;
	BOOL	m_bGroupName;
	BOOL	m_bExcludeBackups;
	BOOL	m_bRegex;
	BOOL	m_bExcludeExpired;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CFindInDbDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CFindInDbDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // AFX_FINDINDBDLG_H__BE7DB380_15E6_11D8_BF16_0050BF14F5CC__INCLUDED_
