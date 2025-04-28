// VerifyDlg.cpp : implementation file
//

#include "pch.h"
#include "VerifyDlg.h"
#include "KpLib.h"
#include "Utility.h"


// VerifyDlg dialog

IMPLEMENT_DYNAMIC(VerifyDlg, CDialogEx)


BEGIN_MESSAGE_MAP(VerifyDlg, CDialogEx)
    ON_BN_CLICKED(IDC_ToggleVisible, &VerifyDlg::onToggleVisible)
END_MESSAGE_MAP()


VerifyDlg::VerifyDlg(CWnd* pParent) : CDialogEx(IDD_Verify, pParent), visible(true) { }

VerifyDlg::~VerifyDlg() { }


BOOL VerifyDlg::OnInitDialog() {CDialogEx::OnInitDialog();   onToggleVisible();   return TRUE;}


void VerifyDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_CurrentPswd, curPswd);
  }


// VerifyDlg message handlers

void VerifyDlg::onToggleVisible()
                    {visible ^= true;   setVisible((CEdit*) GetDlgItem(IDC_CurrentPswd), visible);}

