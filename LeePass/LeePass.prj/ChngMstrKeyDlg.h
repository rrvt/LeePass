// Change Master Key


#pragma once
#include "Resource.h"
#include "Utility.h"


// ChngMstrKeyDlg dialog

class ChngMstrKeyDlg : public CDialogEx {

  DECLARE_DYNAMIC(ChngMstrKeyDlg)

bool    visible;

public:
Cstring curPswd;
Cstring newPswd;
Cstring confirmPswd;

           ChngMstrKeyDlg(CWnd* pParent = nullptr);        // standard constructor
  virtual ~ChngMstrKeyDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ChangePswd };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()

private:

  void toggleVisibility(int idc) {setVisible((CEdit*) GetDlgItem(idc), visible);}

public:

  afx_msg void onToggleVisible();
  virtual BOOL OnInitDialog();
};
