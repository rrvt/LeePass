// Verify Master Key Dialog


#pragma once
#include "Resource.h"


// VerifyDlg dialog

class VerifyDlg : public CDialogEx {

  DECLARE_DYNAMIC(VerifyDlg)

bool visible;

public:
Cstring curPswd;

           VerifyDlg(CWnd* pParent = nullptr);   // standard constructor
  virtual ~VerifyDlg();

  virtual BOOL OnInitDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_Verify };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()

public:

  afx_msg void onToggleVisible();
  };
