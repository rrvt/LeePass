// PasswordNewDlg.cpp : implementation file
//

#include "pch.h"
#include "PasswordNewDlg.h"
#include "Resource.h"
#include "Utility.h"


IMPLEMENT_DYNAMIC(PasswordNewDlg, CDialogEx)


PasswordNewDlg::PasswordNewDlg(CWnd* pParent) :
                                            CDialogEx(IDD_NewPassword, pParent), visible(false) { }


BOOL PasswordNewDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();
  setVisible(pswdCtl, visible);   setVisible(confirmCtl, visible);   return TRUE;
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
  visible ^= true;   setVisible(pswdCtl, visible);   setVisible(confirmCtl, visible);
  }


void PasswordNewDlg::onLeaveConfirmPswd() {
Cstring cfrm;

  pswdCtl.GetWindowText(password);
  confirmCtl.GetWindowText(cfrm);

  if (password != cfrm) {password.expunge();   password.clear();   pswdCtl.SetFocus();}
  }


void PasswordNewDlg::OnOK() {pswdCtl.Clear();   confirmCtl.Clear();   CDialogEx::OnOK();}



//////----------------

//static TCchar BulletCh = 0x25CF;

