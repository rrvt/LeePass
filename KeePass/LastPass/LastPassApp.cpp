// LastPass.cpp : Defines the initialization routines for the DLL.
//

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//    any functions exported from this DLL which call into
//    MFC must have the AFX_MANAGE_STATE macro added at the
//    very beginning of the function.
//
//    For example:
//
//    extern "C" BOOL PASCAL EXPORT ExportedFunction()
//    {
//      AFX_MANAGE_STATE(AfxGetStaticModuleState());
//      // normal function body here
//    }
//
//    It is very important that this macro appear in each
//    function, prior to any calls into MFC.  This means that
//    it must appear as the first statement within the
//    function, even before any object variable declarations
//    as their constructors may generate calls into the MFC
//    DLL.
//
//    Please see MFC Technical Notes 33 and 58 for additional
//    details.
//

#include "pch.h"
#include "LastPassApp.h"


LastPassApp theApp;                          // The one and only CLastPassApp object


BEGIN_MESSAGE_MAP(LastPassApp, CWinApp)
END_MESSAGE_MAP()


// LastPassApp initialization

BOOL LastPassApp::InitInstance() {CWinApp::InitInstance(); return TRUE;}

