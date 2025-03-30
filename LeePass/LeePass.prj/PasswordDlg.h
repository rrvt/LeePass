// Password Dialog


#pragma once


// PasswordDlg dialog

class PasswordDlg : public CDialogEx {

  DECLARE_DYNAMIC(PasswordDlg)

bool    toggle;

public:
String  title;
CEdit   passwordCtl;
Cstring password;

               PasswordDlg(CWnd* pParent = nullptr);   // standard constructor
  virtual     ~PasswordDlg();

  virtual BOOL OnInitDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_Password };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()

public:

  afx_msg void onSeePassword();
  };
