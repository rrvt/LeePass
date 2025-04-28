// PasswordDlg.cpp : implementation file


#include "pch.h"
#include "PasswordDlg.h"
#include "Resource.h"
#include "Utility.h"



IMPLEMENT_DYNAMIC(PasswordDlg, CDialogEx)


BEGIN_MESSAGE_MAP(PasswordDlg, CDialogEx)
  ON_BN_CLICKED(IDC_SeePassword, &PasswordDlg::onToggleVisibility)
END_MESSAGE_MAP()


PasswordDlg::PasswordDlg(CWnd* pParent) : CDialogEx(IDD_Password, pParent), visible(false) { }

PasswordDlg::~PasswordDlg() { }


BOOL PasswordDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();   setVisible(passwordCtl, visible);

  SetWindowText(title);   return true;
  }


void PasswordDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_Password, passwordCtl);
  DDX_Text(   pDX, IDC_Password, password);
  }


void PasswordDlg::onToggleVisibility() {visible ^= true;   setVisible(passwordCtl, visible);}



///////////--------------

//static TCchar BulletCh = 0x25CF;
//Tchar ch = toggle ? 0 : BulletCh; //passwordCtl.SetPasswordChar(ch);   passwordCtl.Invalidate();


