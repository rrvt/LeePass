// Generator Dialog


#pragma once
#include "Resource.h"


// GeneratorDlg dialog

class GeneratorDlg : public CDialogEx {

  DECLARE_DYNAMIC(GeneratorDlg)

String  set;

public:
CEdit   passwordCtl;
Cstring password;
Cstring length;
Cstring otherChars;
BOOL    upperCase;
BOOL    lowerCase;
BOOL    digits;
BOOL    Punctuation;
BOOL    quotes;
BOOL    otherSpecial;

CStatic entropyCtl;           // Output entropy here

           GeneratorDlg(CWnd* pParent = nullptr);   // standard constructor
  virtual ~GeneratorDlg();

  virtual BOOL OnInitDialog();

          void integrate(TCchar* tc);

  virtual void OnOK();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_Generator };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()

private:

  void addOtherChars();
  void dspEntropy(int pswdLng, int setLng);

public:
  afx_msg void onGenerate();
  };



//////------------------

//CEdit   lengthCtl;
//CButton upperCaseCtl;
//CButton lowerCaseCtl;
//CButton digitsCtl;
//CButton PunctuationCtl;
//CButton quotesCtl;
//CButton otherSpecialCtl;

