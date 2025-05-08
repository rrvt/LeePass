// About Dialog


#include "pch.h"
#include "AboutDlgKp.h"
#include "ResourceData.h"
#include "ResourceExtra.h"


void AboutDlgKp::DoDataExchange(CDataExchange* pDX) {CDialogEx::DoDataExchange(pDX);}


BOOL AboutDlgKp::OnInitDialog() {
ResourceData res;
String       copyright;
String       companyName;
String       name;
String       prodName;
String       nameVer;
String       version;
bool         rslt;

  CDialogEx::OnInitDialog();

  if (useProdName) rslt = res.getProductName(prodName);
  else             rslt = res.getInternalName(prodName);

  if (!rslt) prodName = _T("Product");

  nameVer = prodName.trim();   prodName = _T("About ") + prodName;   SetWindowText(prodName);

  #ifdef WinXP
    nameVer += _T(" (WinXP)");
  #elif defined UNICODE
    nameVer += _T(" (Unicode)");
  #else
    nameVer += _T(" (Win 11)");
  #endif

  if (!res.getVersion(version)) version = _T("0/0/0");

  res.threeSect(version);

  nameVer += _T(", Version ") + version;  nameVer.trim();

  if (!res.getCompanyName(name)) name = _T("Bob -- K6RWY");   name.trim();
  companyName = _T("Author:  ") + name;

  // Copyright (c) Bob -- K6RWY.  All rights reserved.
  copyright = _T("Copyright (c) ") + name + _T(".  All rights reserved.");

  SetDlgItemText(IDC_NameVer,     nameVer);
  SetDlgItemText(IDC_Copyright,   copyright);
  SetDlgItemText(IDC_CompanyName, companyName);
  SetDlgItemText(IDC_KeePassVer,  keePassLibVer);

  return TRUE;
  }

BEGIN_MESSAGE_MAP(AboutDlgKp, CDialogEx)
  ON_BN_CLICKED(IDOK, &AboutDlgKp::onOk)
END_MESSAGE_MAP()


void AboutDlgKp::onOk() {CDialogEx::OnOK();}

