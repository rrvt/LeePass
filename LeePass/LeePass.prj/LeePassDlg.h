// Main Dialog Box


#pragma once
#include "ClipBoard.h"
#include "KpLib.h"
#include "KpSearch.h"
#include "MyToolBar.h"
#include "Record.h"
#include "Resource.h"
#include "Status.h"
#include "WinPos.h"

class Record;

extern TCchar* GlobalSect;
extern TCchar* KpDbPathKey;
extern TCchar* KpDBDbPathKey;

class StatusBar;



// LeePassDlg dialog

class LeePassDlg : public CDialogEx {

DECLARE_DYNAMIC(LeePassDlg)

MyToolBar   toolBar;                          // Part of the infrastructure of the main dialog
StatusBar   statusBar;
CButton     generateCtl;

bool        isInitialized;                    // OnInitDialog completed successfully

String      path;
bool        dbOpen;
int         nRecords;                         // Count of the number of records (Entries) found
                                              // in KeePass file
Record      rcd;                              // An Entry is stored in a record when saved to DB
bool        newRcd;                           // New Record when true, determines how saved
bool        saveRcd;                          // Save Entry when true, read only when false

bool        dirty;                            // Entry has been changed when true
bool        saveDB;                           // App internal database has been changed when true

KpSearch    kpSrch;                           // Search internal database filtered by current group

Status      status;
String      helpPath;

ClipBoard   clipBoard;                        // Username and password copied to clipboard

private:

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

          void clearDlg();

  virtual BOOL OnInitDialog();

          void installGroupCbx();
          void installEntryCbx();

  virtual void OnOK() {::SendMessage(m_hWnd, WM_NEXTDLGCTL, 0, 0);}

// Dialog Data

#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_LeePass };
#endif

protected:

  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void onExit();

  DECLARE_MESSAGE_MAP()

private:

  void            initMgmt();

  void            saveCurrentRcd();
  void            saveNewRcd(Record& rcd);

  void            saveCurrentDB();
  void            saveDataBase();
  void            savePath();

  bool            isLegalRcd();           // Make sure there is some identification info

  void            installEntry(TCchar* name, void* data);

  int             findCbxGroup(uint grpId);
  int             findCbxEntry(void* kpe);
  void            loadEntry(int i);       // Selects and loads the ith entry in Entry Combo Box
  void            loadEntry();

  void            setupToolBar();

  void            setTitle(TCchar* title);
  void            setLabels();
  void            finOpen();
  void            saveMasterKey(Cstring& masterKey);

  void            shiftDirty() {saveDB |= dirty;   dirty = false;}

  void            showGenerateButton();

  void            finFind(KpEntry* kpEntry);

#ifdef _DEBUG
  TCchar*         getDbPathKey() {return KpDBDbPathKey;}
#else
  TCchar*         getDbPathKey() {return KpDbPathKey;}
#endif

public:

  afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg LRESULT OnResetToolBar(WPARAM wParam, LPARAM lParam);
  afx_msg void    onAppAbout();
  afx_msg void    onHelp();
  afx_msg void    OnClose();

  afx_msg void    OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void    OnMove(int x, int y);
  afx_msg void    OnSize(UINT nType, int cx, int cy);
  afx_msg BOOL    OnTtnNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

  afx_msg void    onchangeMasterKey();

  afx_msg void    onSave();

  afx_msg void    onNewKpDb();
  afx_msg void    onOpenKpDb();
  afx_msg void    onLogin();
  afx_msg void    onGroupCbx();
  afx_msg void    onNewEntry();
  afx_msg void    onEntryCbx();

  afx_msg void    onMoveLeft();
  afx_msg void    onFindEntry();
  afx_msg void    onFindNext();
  afx_msg void    onMoveRight();

  afx_msg void    onToggleSave();

  afx_msg void    onOpenBrowser();
  afx_msg void    onCopyUserName();
  afx_msg void    onCopyPassword();
  afx_msg LRESULT onClearClipBoard(WPARAM wParam, LPARAM lParam);

  afx_msg void    onDeleteMenu() {toolBar.dispatch(ID_DeleteMenu);}
  afx_msg void    onDeleteEntry();
  afx_msg void    onDeleteGroup();

  afx_msg void    onLastPassMenu() {toolBar.dispatch(ID_LastPassMenu);}
  afx_msg void    onImportFile();
  afx_msg void    onExportFile();
  afx_msg void    onExpungeFile();

  afx_msg void    onSaveEntry();
  afx_msg void    onSaveGroup();
  afx_msg void    onRecoverEntries();
  afx_msg void    onRecoverFmrDB();

  afx_msg void    onGeneratePswd();
  afx_msg void    onRemoveDups();

  afx_msg void    onFocusUrl();
  afx_msg void    onFocusUserName();
  afx_msg void    onFocusPswd();
  afx_msg void    onFocusNotes();
  afx_msg void    onFocusBinarydesc();
  afx_msg void    onFocusTitle();
  afx_msg void    onFocusGroupUpdt();
  afx_msg void    onGenerate();
  };



////----------

#if 0
  afx_msg void    onRmvLPImports();
  afx_msg void    onRmvRdndtGrps();
  afx_msg void    onRmvBackups();
#endif

