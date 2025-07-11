// SearchDlg.cpp : implementation file
//

#include "pch.h"
#include "IniFileEx.h"
#include "SearchDlg.h"
#include "resource.h"


       TCchar* SearchSect     = _T("Search");
static TCchar* GroupKey       = _T("Search Group");
static TCchar* TargetKey      = _T("Target");
static TCchar* CaseSensKey    = _T("CaseSens");
static TCchar* SrchModeKey    = _T("SrchMode");
static TCchar* TitleFldKey    = _T("TitleFld");
static TCchar* UserNameFldKey = _T("UserNameFld");
static TCchar* URLFldKey      = _T("URLFld");
static TCchar* MiscFldKey     = _T("MiscFld");
static TCchar* BinDescFldKey  = _T("BinDescFld");

// SearchDlg dialog

IMPLEMENT_DYNAMIC(SearchDlg, CDialogEx)

SearchDlg::SearchDlg(CWnd* pParent) : CDialogEx(IDD_Search, pParent),
                                      caseSens(FALSE),   srchMode(AnyWhere),
                                      titleFld(true),    userNameFld(true),  urlFld(true),
                                      miscFld(true),     binDescFld(true),
                                      groupMode(AllGrpsMode) { }


SearchDlg::~SearchDlg() { }


BOOL SearchDlg::OnInitDialog() {

  iniFile.read(SearchSect, GroupKey,       groupMode,   AllGrpsMode);
  iniFile.read(SearchSect, TargetKey,      target,      _T(""));
  iniFile.read(SearchSect, CaseSensKey,    caseSens,    false);
  iniFile.read(SearchSect, SrchModeKey,    srchMode,    AnyWhere);
  iniFile.read(SearchSect, TitleFldKey,    titleFld,    true);
  iniFile.read(SearchSect, UserNameFldKey, userNameFld, true);
  iniFile.read(SearchSect, URLFldKey,      urlFld,      true);
  iniFile.read(SearchSect, MiscFldKey,     miscFld,     true);
  iniFile.read(SearchSect, BinDescFldKey,  binDescFld,  true);

  CDialogEx::OnInitDialog();   return TRUE;
  }


void SearchDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_Target,      tagetCtl);
  DDX_Text(   pDX, IDC_Target,      target);
  DDX_Check(  pDX, IDC_CaseSens,    caseSens);
  DDX_Radio(  pDX, IDC_RegExpr,     srchMode);
  DDX_Check(  pDX, IDC_TitleFld,    titleFld);
  DDX_Check(  pDX, IDC_UserNameFld, userNameFld);
  DDX_Check(  pDX, IDC_URLFld,      urlFld);
  DDX_Check(  pDX, IDC_MiscFld,     miscFld);
  DDX_Check(  pDX, IDC_BinDescFld,  binDescFld);
  DDX_Radio(  pDX, IDC_AllGroups,   groupMode);
  }


BEGIN_MESSAGE_MAP(SearchDlg, CDialogEx)
END_MESSAGE_MAP()

void SearchDlg::OnOK() {
  CDialogEx::OnOK();
  iniFile.write(SearchSect, GroupKey,       groupMode);
  iniFile.write(SearchSect, TargetKey,      target);
  iniFile.write(SearchSect, CaseSensKey,    caseSens);
  iniFile.write(SearchSect, SrchModeKey,    srchMode);
  iniFile.write(SearchSect, TitleFldKey,    titleFld);
  iniFile.write(SearchSect, UserNameFldKey, userNameFld);
  iniFile.write(SearchSect, URLFldKey,      urlFld);
  iniFile.write(SearchSect, MiscFldKey,     miscFld);
  iniFile.write(SearchSect, BinDescFldKey,  binDescFld);
  }
