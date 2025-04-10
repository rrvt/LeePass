// Get New Password Dialog


#pragma once


class PasswordNewDlg : public CDialogEx {

  DECLARE_DYNAMIC(PasswordNewDlg)

bool    toggle;
CEdit   pswdCtl;
CEdit   confirmCtl;
Cstring password;

public:

               PasswordNewDlg(CWnd* pParent = nullptr);   // standard constructor
  virtual     ~PasswordNewDlg() { }
  virtual BOOL OnInitDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_NewPassword };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()

public:

  afx_msg void onToggleVisible();
  afx_msg void onLeaveConfirmPswd();
  virtual void OnOK();
};
