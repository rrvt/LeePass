// About Dialog


#pragma once
#include "Resource.h"
#include "ResourceExtra.h"


// AboutDlgKp dialog used for App About

class AboutDlgKp : public CDialogEx {
bool useProdName;
public:
Cstring keePassLibVer;


enum { IDD = IDD_AboutBox };     // Dialog Data

  AboutDlgKp(bool useInternalName) : useProdName(!useInternalName), CDialogEx(AboutDlgKp::IDD) { }
  AboutDlgKp()                     : useProdName(true),             CDialogEx(AboutDlgKp::IDD) { }

  virtual BOOL OnInitDialog();

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:

  DECLARE_MESSAGE_MAP()

  afx_msg void onOk();
  afx_msg void onChangePdfapps();
  };

