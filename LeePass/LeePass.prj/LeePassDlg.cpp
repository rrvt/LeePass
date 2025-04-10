// LeePassDlg.cpp : implementation file
//

#include "pch.h"
#include "LeePassDlg.h"
#include "AboutDlgKp.h"
#include "CopyFile.h"
#include "FileName.h"
#include "GetPathDlg.h"
#include "Groups.h"
#include "History.h"
#include "IniFile.h"
#include "KpID.h"
#include "KpIter.h"
#include "LastPass.h"
#include "MessageBox.h"
#include "PasswordDlg.h"
#include "PasswordNewDlg.h"
#include "Record.h"
#include "StatusBar.h"
#include "Utility.h"


       TCchar* GlobalSect   = _T("Global");
       TCchar* KpDbPathKey  = _T("KpDbPath");

static TCchar* EntryCaption = _T(" Entries ");
static TCchar* NewRecordSts = _T("New Record");


IMPLEMENT_DYNAMIC(LeePassDlg, CDialogEx)


BEGIN_MESSAGE_MAP(LeePassDlg, CDialogEx)

  ON_COMMAND(      ID_NewKpDb,      &onNewKpDb)
  ON_COMMAND(      ID_OpenKpDb,     &onOpenKpDb)
  ON_COMMAND(      ID_Login,        &onLogin)

  ON_CBN_SELCHANGE(ID_GroupCbx,     &onGroupCbx)          // Process secelection from list

  ON_COMMAND(      ID_NewPswd,      &onNewPswd)
  ON_CBN_SELCHANGE(ID_EntryCbx,     &onEntryCbx)          // Process secelection from list

  ON_COMMAND(      ID_ToggleSave,   &onToggleSave)

  ON_COMMAND(      ID_Save,         &onSave)

  ON_CBN_SELCHANGE(ID_DeleteMenu,   &onDeleteMenu)          // Send Command Message with ID_...
  ON_COMMAND(      ID_DeleteEntry,  &onDeleteEntry)
  ON_COMMAND(      ID_DeleteGroup,  &onDeleteGroup)

  ON_CBN_SELCHANGE(ID_LastPassMenu, &onLastPassMenu)          // Send Command Message with ID_...
  ON_COMMAND(      ID_ImportFile,   &onImportFile)
  ON_COMMAND(      ID_ExpungeFile,  &onExpungeFile)

  ON_COMMAND(      ID_Help,         &onHelp)
  ON_COMMAND(      ID_About,        &onAppAbout)
  ON_COMMAND(      ID_Exit,         &onExit)

  ON_EN_SETFOCUS(  IDC_URL,         &onFocusUrl)
  ON_EN_SETFOCUS(  IDC_Name,        &onFocusName)
  ON_EN_SETFOCUS(  IDC_Pswd,        &onFocusPswd)
  ON_EN_SETFOCUS(  IDC_Notes,       &onFocusNotes)
  ON_EN_SETFOCUS(  IDC_BinaryDesc,  &onFocusBinarydesc)
  ON_EN_SETFOCUS(  IDC_Title,       &onFocusTitle)
  ON_CBN_SETFOCUS( IDC_GroupUpdt,   &onFocusGroupUpdt)

  ON_WM_CREATE()
  ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, &OnResetToolBar)
  ON_NOTIFY_EX(         TTN_NEEDTEXT, 0,     &OnTtnNeedText)          // Do ToolTips
  ON_WM_MOVE()
  ON_WM_SIZE()
END_MESSAGE_MAP()


LeePassDlg::LeePassDlg(TCchar* helpPth, CWnd* pParent) :
                       CDialogEx(IDD_LeePass, pParent), toolBar(), statusBar(),
                       isInitialized(false), pwMgr(0), dbOpen(false), newRcd(true),
                       saveRcd(false), dirty(false), helpPath(helpPth) { }


LeePassDlg::~LeePassDlg() {winPos.~WinPos();}


int LeePassDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {

  if (CDialogEx::OnCreate(lpCreateStruct) == -1) return -1;

  return 0;
  }


BOOL LeePassDlg::OnInitDialog() {
CRect winRect;

  CDialogEx::OnInitDialog();

  GetWindowRect(&winRect);   winPos.setDLUToPxls(winRect, DlgWidth, DlgDepth);

  if (!toolBar.create(this, IDR_TOOLBAR)) return false;

  SetBackgroundColor(RGB(255,255,255));               // toolBar.move(winRect);

  if (!statusBar.create(this, IDC_StatusBar)) return false;

  statusBar.setReady();

  winPos.initialPos(this, winRect);   toolBar.move(winRect);   statusBar.move(winRect);

  iniFile.read(GlobalSect, KpDbPathKey, path, _T(""));

  if (!path.isEmpty()) {setTitle(getMainName(path));   statusBar.setText(1, path);}

  setStatus(_T("Login"));   isInitialized = true;   return true;
  }


void LeePassDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_Title, titleCtl);
    DDX_Control(pDX, IDC_URL, urlCtl);
    DDX_Control(pDX, IDC_Name, nameCtl);
    DDX_Control(pDX, IDC_Pswd, pswdCtl);
    DDX_Control(pDX, IDC_Notes, notesCtl);
    DDX_Control(pDX, IDC_BinaryDesc, binaryDescCtl);
    DDX_Control(pDX, IDC_GroupUpdt, groupCtl);
    }


void LeePassDlg::onNewKpDb() {
PasswordNewDlg dlg;

  if (dlg.DoModal() != IDOK) return;

  pwMgr = kpLib.newDatabase(path, dlg.password);   finOpen();   kpLib.dspEncryption();
  }


void LeePassDlg::onOpenKpDb() {
PathDlgDsc pathDlgDsc(_T("LeePass Database"), path, _T("kdb"), _T("*.kdb"));

  if (!getOpenDlg(pathDlgDsc, path)) return;

  dbOpen = false;
  }


void LeePassDlg::onLogin() {
PasswordDlg dlg;
String      dbName = getMainName(path);

  if (dbOpen) return;

  dlg.title = _T("Enter Password for ") + dbName;

  if (dlg.DoModal() != IDOK) return;

  pwMgr = kpLib.openDatabase(path, dlg.password);   if (!pwMgr) return;

  groups.install(toolBar, ID_GroupCbx);    installEntries();   finOpen();
  }


void LeePassDlg::finOpen() {

  setLabels();   dbOpen = true;   newRcd = true;   iniFile.write(GlobalSect, KpDbPathKey, path);

  statusBar.setText(1, path);   setTitle(getMainName(path));   if (newRcd) setStatus(NewRecordSts);
  }



void LeePassDlg::installEntries() {
KpIter   iter(pwMgr);
KpEntry* kpEntry;
String   group;
void*    x;
uint     grpId = 0;

  toolBar.clearCBx(ID_EntryCbx);

  if (toolBar.getCurSel(ID_GroupCbx, group, x)) grpId = (uint) x;

  for (kpEntry = iter(); kpEntry; kpEntry = iter++) if (!grpId || kpEntry->uGroupId == grpId)
                           toolBar.addCbxItemSorted(ID_EntryCbx, kpEntry->pszTitle, (int) kpEntry);

  toolBar.setCaption(ID_EntryCbx, EntryCaption);

  toolBar.setWidth(ID_EntryCbx);   toolBar.setHeight(ID_EntryCbx);
  }


void LeePassDlg::onGroupCbx() {
String s;
void*  x;
String t;

  if (!toolBar.getCurSel(ID_GroupCbx, s, x)) return;

  installEntries();

  t.format(_T("Item = %s, Data = %i"), s.str(), (int) x);   statusBar.setText(1, t);
  }


void LeePassDlg::onNewPswd() {
Record& rcd = kpLib.rcd;

  saveCurrentRcd();   rcd.clear();

  newRcd = saveRcd = true;   setLabels();   setStatus(NewRecordSts);
  }


void LeePassDlg::onEntryCbx() {
String   s;
void*    x;
Record&  rcd = kpLib.rcd;

  saveCurrentRcd();

  if (!toolBar.getCurSel(ID_EntryCbx, s, x)) return;

  newRcd = false;   rcd = (KpEntry*) x;

  rcd.title.set(titleCtl);
  rcd.url.set(urlCtl);
  rcd.name.set(nameCtl);
  rcd.password.set(pswdCtl);
  rcd.notes.set(notesCtl);
  rcd.binDesc.set(binaryDescCtl);
  rcd.group.set(groupCtl);

  setEntrySts(rcd);
  }


void LeePassDlg::saveCurrentRcd() {
Record& rcd = kpLib.rcd;

  if (!dbOpen || !saveRcd) {rcd.clear();   return;}
  if (newRcd)              {saveNewRcd(rcd);  return;}

  dirty |= rcd.updateTitle(titleCtl);
  dirty |= rcd.updateURL(urlCtl);
  dirty |= rcd.updateName(nameCtl);
  dirty |= rcd.updatePassword(pswdCtl);
  dirty |= rcd.updateNotes(notesCtl);
  dirty |= rcd.updateBinaryDesc(binaryDescCtl);
  dirty |= rcd.updateGroup(groupCtl);

  if (groups.isModified()) groups.install(toolBar, ID_GroupCbx);
  }


void LeePassDlg::saveNewRcd(Record& rcd) {
  dirty |= rcd.setTitle(titleCtl);
  dirty |= rcd.setURL(urlCtl);
  dirty |= rcd.setName(nameCtl);
  dirty |= rcd.setPassword(pswdCtl);
  dirty |= rcd.setNotes(notesCtl);
  dirty |= rcd.setBinaryDesc(binaryDescCtl);
  dirty |= rcd.setGroup(groupCtl);

  if (dirty && rcd.add()) {
    newRcd = false;   setEntrySts(rcd);   groups.install(toolBar, ID_GroupCbx);
    groups.install(groupCtl, rcd.group);   installEntries();
    }
  }


void LeePassDlg::onExit() {onSave();   CDialogEx::OnOK();}


void LeePassDlg::onSave() {

  saveCurrentRcd();

  if (dirty) {backupCopy(path, 5);   kpLib.saveDatabase(path);}

  dirty = false;
  }


void LeePassDlg::setLabels() {
Record& rcd = kpLib.rcd;

  rcd.title.setLabel(titleCtl);
  rcd.url.setLabel(urlCtl);
  rcd.name.setLabel(nameCtl);
  rcd.password.setLabel(pswdCtl);
  rcd.notes.setLabel(notesCtl);
  rcd.binDesc.setLabel(binaryDescCtl);
  rcd.group.setLabel(groupCtl);
  }


void LeePassDlg::onFocusTitle()      {kpLib.rcd.title.clrLabel(titleCtl);}
void LeePassDlg::onFocusUrl()        {kpLib.rcd.url.clrLabel(urlCtl);}
void LeePassDlg::onFocusName()       {kpLib.rcd.name.clrLabel(nameCtl);}
void LeePassDlg::onFocusPswd()       {kpLib.rcd.password.clrLabel(pswdCtl);}
void LeePassDlg::onFocusNotes()      {kpLib.rcd.notes.clrLabel(notesCtl);}
void LeePassDlg::onFocusBinarydesc() {kpLib.rcd.binDesc.clrLabel(binaryDescCtl);}
void LeePassDlg::onFocusGroupUpdt()  {kpLib.rcd.group.clrLabel(groupCtl);}


void LeePassDlg::onToggleSave() {saveRcd ^= true;   setStatus(0);}



void LeePassDlg::onDeleteEntry() {
String   s;
void*    x;
Record&  rcd = kpLib.rcd;
String   dsc = rcd.getEntryDsc();
String   q;
KpIter   iter(pwMgr);
KpEntry* kpEntry;
KpID     kpID;

  if (!saveRcd) return;

  saveCurrentRcd();

  if (!toolBar.getCurSel(ID_EntryCbx, s, x)) return;

  kpEntry = (KpEntry*) x;
  rcd     = kpEntry;   kpID = kpEntry->uuid;

  q = _T("Delete Entry: ") + dsc;   if (msgYesNoBox(q) != IDYES) return;

  for (kpEntry = iter(); kpEntry; kpEntry = iter++)
                                               if (kpID == kpEntry->uuid) {iter.remove();   break;}
  installEntries();

  q = dsc + _T(" has been deleted");   setStatus(q);   setLabels();
  }


void LeePassDlg::onDeleteGroup() {
String   group;
void*    x;
uint     grpId;
String   q;

  if (!saveRcd) return;

  saveCurrentRcd();

  if (!toolBar.getCurSel(ID_GroupCbx, group, x)) return;

  grpId = (uint) x;

  q = _T("Delete ") + group + _T(" Group and all Entries in the Group");

  if (msgYesNoBox(q) != IDYES) return;
#if 1
  groups.del(grpId, toolBar, ID_GroupCbx);
#else
  if (!DeleteGroupById(pwMgr, grpId)) return;

  groups.initialize();   groups.install(toolBar, ID_GroupCbx);
#endif
  installEntries();

  setLabels();   q = _T("Deleted ") + group + _T(" Group and Entries in the Group");

  setStatus(q);
  }


void LeePassDlg::onImportFile() {
PathDlgDsc dsc;
String     path;
LastPass   lastPass;

  if (!kpLib.importFile(path)) return;

  dirty = true;   //onSave();

  groups.initialize();

  groups.install(toolBar, ID_GroupCbx);   groups.install(groupCtl, 0);

  installEntries();   //dirty = false;
  }


void LeePassDlg::onExpungeFile() {
PathDlgDsc dsc;
String     path;
LastPass   lastPass;

  dsc(_T("Expunge File"), _T(""), _T("*"), _T("*.*"));

  if (getOpenDlg(dsc, path)) lastPass.expungeFile(path);
  }


void LeePassDlg::setEntrySts(Record& rcd) {setStatus(rcd.getEntryDsc());}


void LeePassDlg::setStatus(TCchar* sts) {
static String msg;
bool          isPresent = sts && *sts;
String        t         = isPresent ? sts : msg.str();

  if (isPresent) msg = t;

  if (!t.isEmpty()) t += _T(" -- ");

  t +=  saveRcd ? _T("Save Record Changes") : _T("Read Only");

  statusBar.setText(0, t);
  }


void LeePassDlg::setTitle(TCchar* title)
                                    {String s = _T("LeePass -- ");  s += title;  SetWindowText(s);}


void LeePassDlg::OnMove(int x, int y)
      {CRect winRect;   GetWindowRect(&winRect);   winPos.set(winRect);   CDialogEx::OnMove(x, y);}


void LeePassDlg::OnSize(UINT nType, int cx, int cy) {
CRect r;

  GetWindowRect(&r);

  if (!isInitialized) {winPos.setInvBdrs(r, cx, cy);   return;}

  winPos.set(cx, cy);   toolBar.move(r);   statusBar.move(r);    CDialogEx::OnSize(nType, cx, cy);
  }


// MainFrame message handlers

LRESULT LeePassDlg::OnResetToolBar(WPARAM wParam, LPARAM lParam) {setupToolBar();  return 0;}


void LeePassDlg::setupToolBar() {
CRect winRect;   GetWindowRect(&winRect);   toolBar.set(winRect);

  toolBar.addCBx(   ID_GroupCbx,  _T(" Groups "));
  toolBar.addCBx(   ID_EntryCbx, EntryCaption);

  toolBar.addMenu(ID_DeleteMenu,   IDR_DeleteMenu,   _T("Delete"));
  toolBar.addMenu(ID_LastPassMenu, IDR_LastPassMenu, _T("LastPass"));
  }


// Do ToolTips

BOOL LeePassDlg::OnTtnNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
                                                            {return toolBar.OnTtnNeedText(pNMHDR);}

void LeePassDlg::onHelp() {
String topic = helpPath; topic += _T(">Introduction");

  ::HtmlHelp(GetSafeHwnd(), topic,  HH_DISPLAY_TOC, 0);
  }


void LeePassDlg::onAppAbout()
        {AboutDlgKp aboutDlg;   aboutDlg.keePassLibVer = kpLib.getVersion();   aboutDlg.DoModal();}



///////-----------------

#ifdef Examples
  toolBar.addEditBox(ID_EditBox, 20);
  toolBar.addMenu(ID_PopupMenu,  IDR_DeleteMenu, _T("My Caption"));
  toolBar.addMenu(ID_PopupMenu1, PopupItems1, noElements(PopupItems1), _T("My Caption #1"));
#endif
#ifdef Examples
static TCchar* CbxCaption =   _T("Greeks +");

static CbxItem CbxText[]  = {{_T("Zeta"),    1},
                             {_T("Beta"),    2},
                             {_T("Alpha"),   3},
                             {_T("Omega"),   4},
                             {_T("Phi"),     5},
                             {_T("Mu"),      6},
                             {_T("Xi"),      7},
                             {_T("Omicron"), 8},
                             {_T("Pi"),      9},
                             {_T("Rho"),    10},
                             {_T("Sigma"),  11},
                             {_T("Nu"),     12},
                             {_T("Kappa"),  13},
                             {_T("Iota"),   14}
                             };


static CbxItem PopupItems1[] = {{_T("Option11"), ID_Option11},
                                {_T("Option12"), ID_Option12}
                                };
#endif

#ifdef Examples


  ON_COMMAND(      ID_ChangeReady,  &changeReady)

  ON_EN_KILLFOCUS( ID_EditBox,      &onTBEditBox)           // Process content of edit box


  ON_CBN_SELCHANGE(ID_PopupMenu,    &onDispatch)            // Send Command Message with ID_...
  ON_COMMAND(      ID_Option01,     &onOption01)
  ON_COMMAND(      ID_Option02,     &onOption02)

  ON_CBN_SELCHANGE(ID_PopupMenu1,   &onDispatch1)           // Send Command Message with ID_...
  ON_COMMAND(      ID_Option11,     &onOption11)
  ON_COMMAND(      ID_Option12,     &onOption12)

#endif
#ifdef Examples

void LeePassDlg::changeReady() {
bool status = statusBar.isReady();

  statusBar.setReady(!status);
  }
#endif
#ifdef Examples

void LeePassDlg::onDispatch()  {toolBar.dispatch(ID_PopupMenu);}


void LeePassDlg::onOption00() {SetFocus();   onOption01();}


void LeePassDlg::onOption01() {
  statusBar.setText(1, _T("Option 0.1"));
  }


void LeePassDlg::onOption02() {
  statusBar.setText(1, _T("Option 0.2"));
  }


void LeePassDlg::onDispatch1() {toolBar.dispatch(ID_PopupMenu1);}


void LeePassDlg::onOption11() {
  statusBar.setText(1, _T("Option 1.1"));
  }


void LeePassDlg::onOption12() {
  statusBar.setText(1, _T("Option 1.2"));
  }


void LeePassDlg::onTBEditBox() {
CString s = toolBar.getText(ID_EditBox);   statusBar.setText(1, s);
}
#endif
#ifdef Examples
void LeePassDlg::onTBChange(NMHDR* pNMHDR, LRESULT* pResult) {

  LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

  // TODO: Add your control notification handler code here

  *pResult = 0;
  }
#endif
#if 0
void LeePassDlg::installGroups() {
GrpIter iter(groups);
Group*  grp;

  for (grp = iter(); grp; grp = iter++) toolBar.addCbxItemSorted(ID_GroupCbx, grp->name, grp->id);

  toolBar.addCbxItemSorted(ID_GroupCbx, _T(" All Groups "), 0);

  toolBar.setWidth(ID_GroupCbx);   toolBar.setHeight(ID_GroupCbx);
  }
#endif
#if 0
    DDX_Text(pDX, IDC_Title, title);
    DDX_Text(pDX, IDC_URL, url);
    DDX_Text(pDX, IDC_Name, name);
    DDX_Text(pDX, IDC_Pswd, pwsd);
    DDX_Text(pDX, IDC_Notes, notes);
    DDX_Text(pDX, IDC_BinaryDesc, binaryDesc);
    DDX_CBString(pDX, IDC_GroupUpdt, group);
#endif

#if 1
#else
  if (!rcd.title.isEmpty()) q += _T(' ')  + rcd.title;
  if (!rcd.url.isEmpty())   q += _T(", ") + rcd.url;
#endif
//  toolBar.addButton(ID_Login, _T(" Login "));
#if 0
bool     urlEmpty = rcd.url.isEmpty();
String   t;

  if (!rcd.title.isEmpty()) {t = rcd.title;   if (!urlEmpty) t += _T(", ");}
  if (!urlEmpty) t += rcd.url;
#endif
#if 1
#else
  if (!rcd.title.isEmpty()) {q += rcd.title;   if (!rcd.url.isEmpty()) q += _T(", ");}

  if (!rcd.url.isEmpty())   q += rcd.url;
  q += _T(" has been deleted");    setStatus(q);
#endif
#if 1
#else
  setLabels();   dbOpen = true;   newRcd = true;   iniFile.write(GlobalSect, KpDbPathKey, path);

  statusBar.setText(1, path);   setTitle(getMainName(path));   if (newRcd) setStatus(NewRecordSts);
#endif
#if 1
#else
  setLabels();   dbOpen = true;   newRcd = true;   iniFile.write(GlobalSect, KpDbPathKey, path);

  statusBar.setText(1, path);   setTitle(dbName);   if (newRcd) setStatus(NewRecordSts);
#endif

