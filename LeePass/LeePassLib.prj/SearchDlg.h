// Search Dialog


#pragma once


enum SrchMode    {RegExpr, WholeWord, WholeField, Prefix, AnyWhere};
enum SrchGrpMode {AllGrpsMode, CurGrpMode};

extern TCchar* SearchSect;


// SearchDlg dialog

class SearchDlg : public CDialogEx {

  DECLARE_DYNAMIC(SearchDlg)

CEdit   tagetCtl;

public:
int     groupMode;              // Current Group or All Groups

Cstring target;
BOOL    caseSens;           // Search attributes

int     srchMode;           // Search Mode (

BOOL    titleFld;           // Fields to search
BOOL    userNameFld;
BOOL    urlFld;
BOOL    miscFld;
BOOL    binDescFld;

               SearchDlg(CWnd* pParent = nullptr);   // standard constructor
  virtual     ~SearchDlg();
  virtual BOOL OnInitDialog();
  virtual void OnOK();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_Search };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
  };
