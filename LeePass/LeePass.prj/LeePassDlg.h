// Main Dialog Box


#pragma once
#include "KpLib.h"
#include "MyToolBar.h"
#include "Resource.h"
#include "StatusBar.h"
#include "WinPos.h"

class Record;

extern TCchar* GlobalSect;
extern TCchar* KpDbPathKey;

class StatusBar;


// LeePassDlg dialog

class LeePassDlg : public CDialogEx {

DECLARE_DYNAMIC(LeePassDlg)

MyToolBar   toolBar;                            // Part of the infrastructure of the main dialog
StatusBar   statusBar;
bool        isInitialized;                      // OnInitDialog completed successfully

String      path;
CPwManager* pwMgr;                              // Path to Kp Database
bool        dbOpen;
bool        newRcd;
bool        saveRcd;

KpLib       kpLib;
bool        dirty;

String      helpPath;


// Dialog Box
CEdit       titleCtl;
CEdit       urlCtl;
CEdit       nameCtl;
CEdit       pswdCtl;
CEdit       notesCtl;
CEdit       binaryDescCtl;
CComboBox   groupCtl;

public:

               LeePassDlg(TCchar* helpPth, CWnd* pParent = nullptr);
  virtual     ~LeePassDlg();

  virtual BOOL OnInitDialog();

          void installEntries();

          void saveCurrentRcd();
          void saveNewRcd(Record& rcd);

// Dialog Data

#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_LeePass };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void onExit();

  DECLARE_MESSAGE_MAP()

private:

  void            setupToolBar();
  void            setEntrySts(Record& rcd);
  void            setStatus(TCchar* sts);
  void            setTitle(TCchar* title);
  void            setLabels();

public:

  afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg LRESULT OnResetToolBar(WPARAM wParam, LPARAM lParam);
  afx_msg void    onAppAbout();
  afx_msg void    onHelp();

  afx_msg void    OnMove(int x, int y);
  afx_msg void    OnSize(UINT nType, int cx, int cy);
  afx_msg BOOL    OnTtnNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void    onSave();

  afx_msg void    onNewKpDb();
  afx_msg void    onOpenKpDb();
  afx_msg void    onLogin();
  afx_msg void    onGroupCbx();
  afx_msg void    onNewPswd();
  afx_msg void    onEntryCbx();
  afx_msg void    onToggleSave();

  afx_msg void    onDelete() {toolBar.dispatch(ID_DeleteMenu);}
  afx_msg void    onDeleteEntry();
  afx_msg void    onDeleteGroup();

  afx_msg void    onFocusUrl();
  afx_msg void    onFocusName();
  afx_msg void    onFocusPswd();
  afx_msg void    onFocusNotes();
  afx_msg void    onFocusBinarydesc();
  afx_msg void    onFocusTitle();
  afx_msg void    onFocusGroupUpdt();
};




////--------------

#ifdef Examples
  afx_msg void    changeReady();

  afx_msg void    onTBChange(NMHDR* pNMHDR, LRESULT* pResult);

  afx_msg void    onDispatch();
  afx_msg void    onOption00();
  afx_msg void    onOption01();
  afx_msg void    onOption02();
  afx_msg void    onDispatch1();
  afx_msg void    onOption11();
  afx_msg void    onOption12();


  afx_msg void    onTBEditBox();
#endif
  //Cstring     title;
  //Cstring     url;
  //Cstring     name;
  //Cstring     pwsd;
  //Cstring     notes;
  //Cstring     binaryDesc;
  //Cstring     group;

  //        void installGroups();

