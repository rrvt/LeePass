// LeePass.h -- Template for a Dialog Box Application


#include "pch.h"
#include "LeePass.h"
#include "LeePassDlg.h"
#include "IniFile.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LeePass theApp;                         // The one and only LeePass object
IniFile   iniFile;


// LeePass construction

LeePass::LeePass() noexcept : CDialogApp(this) { }


// LeePass initialization -- Start the MFC Dialog Box

BOOL LeePass::InitInstance() {
LeePassDlg dlg(m_pszHelpFilePath);

  CWinApp::InitInstance();

  iniFile.setAppDataPath(m_pszHelpFilePath, *this);

  dlg.DoModal();   m_pMainWnd = 0;   return 0;
  }


int LeePass::ExitInstance() {return CWinApp::ExitInstance();}

