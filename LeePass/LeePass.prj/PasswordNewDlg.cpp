// PasswordNewDlg.cpp : implementation file
//

#include "pch.h"
#include "PasswordNewDlg.h"
#include "Resource.h"


static TCchar BulletCh = 0x25CF;

IMPLEMENT_DYNAMIC(PasswordNewDlg, CDialogEx)


PasswordNewDlg::PasswordNewDlg(CWnd* pParent) :
                                              CDialogEx(IDD_NewPassword, pParent), toggle(true) { }


BOOL PasswordNewDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();
  pswdCtl.SetPasswordChar(BulletCh);   confirmCtl.SetPasswordChar(BulletCh);
  return TRUE;
  }


void PasswordNewDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_Password,        pswdCtl);
  DDX_Control(pDX, IDC_ConfirmPassword, confirmCtl);
  }


BEGIN_MESSAGE_MAP(PasswordNewDlg, CDialogEx)
  ON_BN_CLICKED(  IDC_SeePassword,     &onToggleVisible)
  ON_EN_KILLFOCUS(IDC_ConfirmPassword, &onLeaveConfirmPswd)
END_MESSAGE_MAP()


// PasswordNewDlg message handlers

void PasswordNewDlg::onToggleVisible() {
Tchar ch = toggle ? 0 : BulletCh;

  toggle ^= true;
     pswdCtl.SetPasswordChar(ch);      pswdCtl.Invalidate();
  confirmCtl.SetPasswordChar(ch);   confirmCtl.Invalidate();
  }


void PasswordNewDlg::onLeaveConfirmPswd() {
Cstring cfrm;

  pswdCtl.GetWindowText(password);
  confirmCtl.GetWindowText(cfrm);

  if (password != cfrm) {password.expunge();   password.clear();   pswdCtl.SetFocus();}
  }


void PasswordNewDlg::OnOK() {pswdCtl.Clear();   confirmCtl.Clear();   CDialogEx::OnOK();}
