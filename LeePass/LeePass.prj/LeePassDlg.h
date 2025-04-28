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
int         noCbxEntries;

String      path;
CPwManager* pwMgr;                              // Path to Kp Database
bool        dbOpen;
bool        newRcd;
bool        saveRcd;

KpLib       kpLib;
bool        dirty;
bool        saveDB;

String      helpPath;


// Dialog Box
CEdit       titleCtl;
CEdit       urlCtl;
CEdit       userNameCtl;
CEdit       pswdCtl;
CEdit       extraCtl;
CStatic     creationCtl;
CStatic     lastModCtl;
CStatic     lastAccessCtl;
CEdit       binaryDescCtl;
CComboBox   groupCtl;

public:

               LeePassDlg(TCchar* helpPth, CWnd* pParent = nullptr);
  virtual     ~LeePassDlg();

  virtual BOOL OnInitDialog();

          void installGroups();
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

  int             findEntry(void* kpe);
  void            loadEntry();

  void            setupToolBar();
  void            setEntrySts(Record& rcd);
  void            setStatus(TCchar* sts);
  void            setDbSts();

  void            setTitle(TCchar* title);
  void            setLabels();
  void            finOpen();

  bool            isLegalRcd(Record& rcd);
  void            shiftDirty() {saveDB |= dirty;   dirty = false;}

public:

  afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg LRESULT OnResetToolBar(WPARAM wParam, LPARAM lParam);
  afx_msg void    onAppAbout();
  afx_msg void    onHelp();

  afx_msg void    OnContextMenu(CWnd* pWnd, CPoint point);
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

  afx_msg void    onMoveLeft();
  afx_msg void    onFindEntry();
  afx_msg void    onFindNext();
  afx_msg void    onMoveRight();

  afx_msg void    onToggleSave();

  afx_msg void    onDeleteMenu() {toolBar.dispatch(ID_DeleteMenu);}
  afx_msg void    onDeleteEntry();
  afx_msg void    onDeleteGroup();

  afx_msg void    onLastPassMenu() {toolBar.dispatch(ID_LastPassMenu);}
  afx_msg void    onImportFile();
  afx_msg void    onExportFile();
  afx_msg void    onExpungeFile();

  afx_msg void    onRemoveDups();
  afx_msg void    onRmvLPImports();
  afx_msg void    onRmvRdndtGrps();
  afx_msg void    onRmvBackups();

  afx_msg void    onFocusUrl();
  afx_msg void    onFocusUserName();
  afx_msg void    onFocusPswd();
  afx_msg void    onFocusNotes();
  afx_msg void    onFocusBinarydesc();
  afx_msg void    onFocusTitle();
  afx_msg void    onFocusGroupUpdt();
  };



