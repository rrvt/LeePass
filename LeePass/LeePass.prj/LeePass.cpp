// LeePass.h -- Template for a Dialog Box Application


#include "pch.h"
#include "LeePass.h"
#include "LeePassDlg.h"
#include "IniFileEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


LeePass   theApp;                         // The one and only LeePass object
IniFileEx iniFile(theApp);


// LeePass construction

LeePass::LeePass() noexcept : CDialogApp(this) { }


// LeePass initialization -- Start the MFC Dialog Box

BOOL LeePass::InitInstance() {
LeePassDlg dlg(m_pszHelpFilePath);

  CWinApp::InitInstance();

  iniFile.setAppDataPath(m_pszHelpFilePath);

  dlg.DoModal();   m_pMainWnd = 0;   return 0;
  }


int LeePass::ExitInstance() {
ClipBoard clipBoard;

  clipBoard.clear();   iniFile.deleteSection(SearchSect);

  return CWinApp::ExitInstance();
  }



///////-----------------

//#include "SearchDlg.h"

