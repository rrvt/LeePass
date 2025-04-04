// About Dialog


#include "pch.h"
#include "AboutDlg.h"
#include "ExtraResource.h"
#include "ResourceData.h"


static Tchar here;


void AboutDlg::DoDataExchange(CDataExchange* pDX) {CDialogEx::DoDataExchange(pDX);}


BOOL AboutDlg::OnInitDialog() {
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
    nameVer += _T(" (UNI 7)");
  #else
    nameVer += _T(" (Win 7)");
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

  return TRUE;
  }



///-----------------

/*


String ResourceData::getAboutNameVer(bool useProdName) {
  bool rslt;
String s;
String t;

  s.clear();

  if (useProdName) rslt = getProductName(s);
  else             rslt = getInternalName(s);

  if (!rslt) s = _T("Unknown");

  #ifdef WinXP
    s += _T(" (WinXP)");
  #elif defined UNICODE
    s += _T(" (UNI 7)");
  #else
    s += _T(" (Win 7)");
  #endif

  if (!getVersion(t)) t = _T("0/0/0");

  threeSect(t);

  s = s + _T(", Version ") + t;  return s.trim();
  }
*/
