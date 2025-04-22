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
#include "Remove.h"
#include "SearchDlg.h"
#include "StatusBar.h"
#include "Utility.h"


       TCchar* GlobalSect   = _T("Global");
       TCchar* KpDbPathKey  = _T("KpDbPath");

static TCchar* EntryCaption = _T(" Entries ");
static TCchar* NewRecordSts = _T("New Record");


IMPLEMENT_DYNAMIC(LeePassDlg, CDialogEx)


BEGIN_MESSAGE_MAP(LeePassDlg, CDialogEx)

  ON_COMMAND(      ID_NewKpDb,       &onNewKpDb)
  ON_COMMAND(      ID_OpenKpDb,      &onOpenKpDb)
  ON_COMMAND(      ID_Login,         &onLogin)

  ON_CBN_SELCHANGE(ID_GroupCbx,      &onGroupCbx)          // Process selection from list

  ON_COMMAND(      ID_NewPswd,       &onNewPswd)
  ON_CBN_SELCHANGE(ID_EntryCbx,      &onEntryCbx)          // Process selection from list

  ON_COMMAND(      ID_ToggleSave,    &onToggleSave)

  ON_COMMAND(      ID_MoveLeft,      &onMoveLeft)
  ON_COMMAND(      ID_FindEntry,     &onFindEntry)
  ON_COMMAND(      ID_FindNext,      &onFindNext)
  ON_COMMAND(      ID_MoveRight,     &onMoveRight)

  ON_COMMAND(      ID_Save,          &onSave)

  ON_CBN_SELCHANGE(ID_DeleteMenu,    &onDeleteMenu)          // Send Command Message with ID_...
  ON_COMMAND(      ID_DeleteEntry,   &onDeleteEntry)
  ON_COMMAND(      ID_DeleteGroup,   &onDeleteGroup)

  ON_CBN_SELCHANGE(ID_LastPassMenu,  &onLastPassMenu)          // Send Command Message with ID_...
  ON_COMMAND(      ID_ImportFile,    &onImportFile)
  ON_COMMAND(      ID_ExpungeFile,   &onExpungeFile)

  ON_COMMAND(      ID_RmvDuplicates, &onRemoveDups)
  ON_COMMAND(      ID_RmvLPImports,  &onRmvLPImports)
  ON_COMMAND(      ID_RmvRdndtGrps,  &onRmvRdndtGrps)
  ON_COMMAND(      ID_RmvBackups,    &onRmvBackups)

  ON_COMMAND(      ID_Help,          &onHelp)
  ON_COMMAND(      ID_About,         &onAppAbout)
  ON_COMMAND(      ID_Exit,          &onExit)

  ON_EN_SETFOCUS(  IDC_URL,          &onFocusUrl)
  ON_EN_SETFOCUS(  IDC_Name,         &onFocusName)
  ON_EN_SETFOCUS(  IDC_Pswd,         &onFocusPswd)
  ON_EN_SETFOCUS(  IDC_Notes,        &onFocusNotes)
  ON_EN_SETFOCUS(  IDC_BinaryDesc,   &onFocusBinarydesc)
  ON_EN_SETFOCUS(  IDC_Title,        &onFocusTitle)
  ON_CBN_SETFOCUS( IDC_GroupUpdt,    &onFocusGroupUpdt)

  ON_WM_CREATE()
  ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, &OnResetToolBar)
  ON_NOTIFY_EX(         TTN_NEEDTEXT, 0,     &OnTtnNeedText)          // Do ToolTips
  ON_WM_MOVE()
  ON_WM_SIZE()
END_MESSAGE_MAP()


LeePassDlg::LeePassDlg(TCchar* helpPth, CWnd* pParent) :
                       CDialogEx(IDD_LeePass, pParent), toolBar(), statusBar(),
                       isInitialized(false), noCbxEntries(0), pwMgr(0), dbOpen(false),
                       newRcd(true), saveRcd(false), dirty(false), saveDB(false),
                       helpPath(helpPth) { }


LeePassDlg::~LeePassDlg() {winPos.~WinPos();}


int LeePassDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {

  if (CDialogEx::OnCreate(lpCreateStruct) == -1) return -1;

  return 0;
  }


BOOL LeePassDlg::OnInitDialog() {
CRect winRect;
HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  CDialogEx::OnInitDialog();

  GetWindowRect(&winRect);   winPos.setDLUToPxls(winRect, DlgWidth, DlgDepth);

  if (!toolBar.create(this, IDR_TOOLBAR)) return false;

  SetBackgroundColor(RGB(255,255,255));

  SetIcon(hIcon, TRUE);                               // Set the icon in the upper left hand corner

  if (!statusBar.create(this, IDC_StatusBar)) return false;

  statusBar.setReady();

  winPos.initialPos(this, winRect);   toolBar.move(winRect);   statusBar.move(winRect);

  iniFile.read(GlobalSect, KpDbPathKey, path, _T(""));

  if (!path.isEmpty()) {setTitle(getMainName(path));   setDbSts();}

  setStatus(_T("Login"));   isInitialized = true;   return true;
  }


void LeePassDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_Title,      titleCtl);
  DDX_Control(pDX, IDC_URL,        urlCtl);
  DDX_Control(pDX, IDC_Name,       nameCtl);
  DDX_Control(pDX, IDC_Pswd,       pswdCtl);
  DDX_Control(pDX, IDC_Notes,      notesCtl);
  DDX_Control(pDX, IDC_BinaryDesc, binaryDescCtl);
  DDX_Control(pDX, IDC_GroupUpdt,  groupCtl);
  DDX_Control(pDX, IDC_Creation,   creationCtl);
  DDX_Control(pDX, IDC_LastMod,    lastModCtl);
  DDX_Control(pDX, IDC_LastAccess, lastAccessCtl);
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

  try {pwMgr = kpLib.openDatabase(path, dlg.password);   if (!pwMgr) return;}
  catch (...) {return;}

  groups.install(toolBar, ID_GroupCbx);    installEntries();   finOpen();
  }


void LeePassDlg::finOpen() {

  setLabels();   dbOpen = true;   newRcd = true;   iniFile.write(GlobalSect, KpDbPathKey, path);

  setDbSts();   setTitle(getMainName(path));   if (newRcd) setStatus(NewRecordSts);
  }


void LeePassDlg::onGroupCbx() {
String s;
void*  x;

  if (toolBar.getCurSel(ID_GroupCbx, s, x))
                        {saveCurrentRcd();   kpLib.rcd.clear();   setLabels();   installEntries();}
  }



void LeePassDlg::installEntries() {
KpIter   iter(pwMgr);
KpEntry* kpEntry;
String   group;
void*    x;
uint     grpId = 0;

  toolBar.clearCBx(ID_EntryCbx);   noCbxEntries = 0;

  if (toolBar.getCurSel(ID_GroupCbx, group, x)) grpId = (uint) x;

  for (kpEntry = iter(); kpEntry; kpEntry = iter++) if (!grpId || kpEntry->uGroupId == grpId)
       {toolBar.addCbxItemSorted(ID_EntryCbx, kpEntry->pszTitle, (int) kpEntry);   noCbxEntries++;}

  toolBar.setCaption(ID_EntryCbx, EntryCaption);

  toolBar.setWthPercent(ID_EntryCbx, 70);

  toolBar.setWidth(ID_EntryCbx);   toolBar.setHeight(ID_EntryCbx);

  setDbSts();
  }


void LeePassDlg::onNewPswd() {
Record& rcd = kpLib.rcd;

  saveCurrentRcd();   rcd.clear();

  newRcd = saveRcd = true;   setLabels();   setStatus(NewRecordSts);
  }


void LeePassDlg::onMoveLeft() {
int i = toolBar.getCurSel(ID_EntryCbx) - 1;   if (i < 0) return;

  saveCurrentRcd();   toolBar.setCurSel(ID_EntryCbx, i);   loadEntry();
  }


/*
Boolean "or"

    A vertical bar separates alternatives. For example,

      gray|grey can match "gray" or "grey".

Grouping

    Parentheses are used to define the scope and precedence of the operators (among other uses).
    For example,
      gray|grey and gr(a|e)y are equivalent patterns which both describe the set of
      "gray" or "grey".

Quantification

    A quantifier after an element (such as a token, character, or group) specifies how many times
    the preceding element is allowed to repeat. The most common quantifiers are the question mark
    ?, the asterisk * (derived from the Kleene star), and the plus sign + (Kleene plus).

    ?           The question mark indicates zero or one occurrences of the preceding element. For
                example, colou?r matches both "color" and "colour".

    *           The asterisk indicates zero or more occurrences of the preceding element. For
                example, ab*c matches "ac", "abc", "abbc", "abbbc", and so on.
    +           The plus sign indicates one or more occurrences of the preceding element. For
                example, ab+c matches "abc", "abbc", "abbbc", and so on, but not "ac".
    {n}         The preceding item is matched exactly n times.
    {min,}      The preceding item is matched min or more times.
    {,max}      The preceding item is matched up to max times.
    {min,max}   The preceding item is matched at least min times, but not more than max times.

Character Classes

    .                     matches any character. For example,

                            a.b matches any string that contains an "a", and then any character and
                            then "b".
                            a.*b matches any string that contains an "a", and then the character
                            "b" at some later point.

    [ character_group ]   Matches any single character in character_group. By default, the match is
                          case-sensitive.

                            [ae]  "a" in "gray"
                                  "a", "e" in "lane"

    [^ character_group ]  Negation: Matches any single character that is not in character_group.
                          By default, characters in character_group are case-sensitive.

                            [^aei]  "r", "g", "n" in "reign"

    [ first - last ]      Character range: Matches any single character in the range from first to
                          last.

                            [A-Z]   "A", "B" in "AB123"


Anchors
    ^   By default, the match must start at the beginning of the string; in multiline mode, it must
        start at the beginning of the line.

        ^\d{3}  "901" in "901-333-"

    $   By default, the match must occur at the end of the string or before \n at the end of the
        string; in multiline mode, it must occur before the end of the line or before \n at the
        end of the line.

These constructions can be combined to form arbitrarily complex expressions, much like one can
construct arithmetical expressions from numbers and the operations +, ?, ×, and ÷.
*/

void LeePassDlg::onFindEntry() {
SearchDlg dlg;
String    s;
void*     x;
uint      grpId = 0;
KpEntry*  kpEntry;
int       cboBxX;

  kpLib.clrSrch();

  if (dlg.DoModal() != IDOK) return;

  if (toolBar.getCurSel(ID_GroupCbx, s, x)) kpLib.setSrchGrp((uint) x);

  kpEntry = kpLib.find(dlg);   if (!kpEntry) return;

  cboBxX = findEntry(kpEntry);   if (cboBxX < 0) return;

  saveCurrentRcd();   toolBar.setCurSel(ID_EntryCbx, cboBxX);   loadEntry();
  }


void LeePassDlg::onFindNext() {
KpEntry* kpEntry;
int      cboBxX;

  if (!kpLib.isSrchInit()) return;

  kpEntry = kpLib.findNext();

  cboBxX = findEntry(kpEntry);   if (cboBxX < 0) return;

  saveCurrentRcd();   toolBar.setCurSel(ID_EntryCbx, cboBxX);   loadEntry();
  }


int LeePassDlg::findEntry(void* kpe) {
int    n = toolBar.getCbxCount(ID_EntryCbx);
int    i;
String s;
void*  x;

  for (i = 0; i < n; i++) {
    x = toolBar.getCbxData(ID_EntryCbx, i);

    if (x == kpe) return i;
    }

  return -1;
  }



void LeePassDlg::onMoveRight() {
int n = toolBar.getCbxCount(ID_EntryCbx);
int i = toolBar.getCurSel(ID_EntryCbx) + 1;   if (i >= n) return;

  saveCurrentRcd();   toolBar.setCurSel(ID_EntryCbx, i);   loadEntry();
  }


void LeePassDlg::onEntryCbx() {

  saveCurrentRcd();

#if 1
  loadEntry();
#else
String   s;
void*    x;
Record&  rcd = kpLib.rcd;

  if (!toolBar.getCurSel(ID_EntryCbx, s, x)) return;

  newRcd = false;   rcd = (KpEntry*) x;

  rcd.title.set(titleCtl);
  rcd.url.set(urlCtl);
  rcd.name.set(nameCtl);
  rcd.password.set(pswdCtl);
  rcd.notes.set(notesCtl);
  rcd.creation.set(creationCtl);
  rcd.lastMod.set(lastModCtl);
  rcd.lastAccess.set(lastAccessCtl);
  rcd.binDesc.set(binaryDescCtl);
  rcd.group.set(groupCtl);

  setEntrySts(rcd);
#endif
  }


void LeePassDlg::loadEntry() {
String   s;
void*    x;
Record&  rcd = kpLib.rcd;

  if (!toolBar.getCurSel(ID_EntryCbx, s, x)) return;

  newRcd = false;   rcd = (KpEntry*) x;

  rcd.title.set(titleCtl);
  rcd.url.set(urlCtl);
  rcd.name.set(nameCtl);
  rcd.password.set(pswdCtl);
  rcd.notes.set(notesCtl);
  rcd.creation.set(creationCtl);
  rcd.lastMod.set(lastModCtl);
  rcd.lastAccess.set(lastAccessCtl);
  rcd.binDesc.set(binaryDescCtl);
  rcd.group.set(groupCtl);

  setEntrySts(rcd);
  }



void LeePassDlg::saveCurrentRcd() {
Record& rcd = kpLib.rcd;

  if (!dbOpen || !saveRcd || !isLegalRcd(rcd)) {rcd.clear();      return;}
  if (newRcd)                                  {saveNewRcd(rcd);  return;}

  dirty |= rcd.updateTitle(titleCtl);
  dirty |= rcd.updateURL(urlCtl);
  dirty |= rcd.updateName(nameCtl);
  dirty |= rcd.updatePassword(pswdCtl);
  dirty |= rcd.updateNotes(notesCtl);
  dirty |= rcd.updateBinaryDesc(binaryDescCtl);
  dirty |= rcd.updateGroup(groupCtl);

  if (dirty) {rcd.lastMod.today();      rcd.lastMod.set(lastModCtl);}
  else {
    rcd.lastAccess.today();   dirty = rcd.updateLastAccess();   rcd.lastAccess.set(lastAccessCtl);
    }

  shiftDirty();

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
    rcd.creation.set(creationCtl);   rcd.lastMod.set(lastModCtl);
    rcd.lastAccess.set(lastAccessCtl);
    newRcd = false;   setEntrySts(rcd);   groups.install(toolBar, ID_GroupCbx);
    groups.install(groupCtl, rcd.group);   installEntries();   shiftDirty();
    }
  }


bool LeePassDlg::isLegalRcd(Record& rcd) {
int n = 0;

  if (!rcd.isTitleEmpty(titleCtl)) n++;
  if (!rcd.isURLEmpty(urlCtl))     n++;
  if (!rcd.isNameEmpty(nameCtl))   n++;

  return n >= 2;
  }


void LeePassDlg::onExit() {onSave();   CDialogEx::OnOK();}


void LeePassDlg::onSave() {

  saveCurrentRcd();

  if (saveDB) {backupCopy(path, 5);   kpLib.saveDatabase(path);}

  saveDB = dirty = false;
  }


void LeePassDlg::setLabels() {
Record& rcd = kpLib.rcd;

        rcd.title.setLabel(titleCtl);
          rcd.url.setLabel(urlCtl);
         rcd.name.setLabel(nameCtl);
     rcd.password.setLabel(pswdCtl);
        rcd.notes.setLabel(notesCtl);
      rcd.binDesc.setLabel(binaryDescCtl);
     rcd.creation.setLabel(creationCtl);
      rcd.lastMod.setLabel(lastModCtl);
   rcd.lastAccess.setLabel(lastAccessCtl);
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

  q = dsc + _T(" has been deleted");   setStatus(q);   setDbSts();   setLabels();
  }


void LeePassDlg::onRemoveDups() {
Remove remove(pwMgr);
int    noDeleted;
String s;

  saveCurrentRcd();

  noDeleted = remove.duplicates();    saveDB |= noDeleted != 0;    if (!saveDB) return;

  installEntries();

  switch (noDeleted) {
    case 0  : s.format(_T("No duplicate entries were deleted"));                  break;
    case 1  : s.format(_T("One duplicate entry was deleted"));                    break;
    default : s.format(_T("%i duplicate entries have been deleted."), noDeleted); break;
    }

  setStatus(s);   setDbSts();
  }


void LeePassDlg::onRmvLPImports() {
Remove remove(pwMgr);
int    noDeleted;
String s;

  saveCurrentRcd();

  noDeleted = remove.oldLPImports();    saveDB |= noDeleted != 0;    if (!saveDB) return;

  installEntries();

  switch (noDeleted) {
    case 0  : s.format(_T("No Old LP Import entries were deleted"));                  break;
    case 1  : s.format(_T("One Old LP Import entry was deleted"));                    break;
    default : s.format(_T("%i Old LP Import entries have been deleted."), noDeleted); break;
    }

  setStatus(s);   setDbSts();
  }


void LeePassDlg::onRmvRdndtGrps() {
Remove remove(pwMgr);
int    noDeleted;
String s;

  saveCurrentRcd();

  noDeleted = remove.redundantGrps();   saveDB |= noDeleted != 0;   if (!saveDB) return;

  groups.initialize();   groups.install(toolBar, ID_GroupCbx);

  switch (noDeleted) {
    case 0  : s.format(_T("No groups were deleted"));                  break;
    case 1  : s.format(_T("One group was deleted"));                   break;
    default : s.format(_T("%i groups have been deleted."), noDeleted); break;
    }

  setStatus(s);   setDbSts();
  }


void LeePassDlg::onRmvBackups() {
Remove remove(pwMgr);
int    noDeleted;
String s;

  saveCurrentRcd();

  noDeleted = remove.backups();   saveDB |= noDeleted != 0;   if (!saveDB) return;

  groups.initialize();   groups.install(toolBar, ID_GroupCbx);

  switch (noDeleted) {
    case 0  : s.format(_T("No backups were deleted"));                  break;
    case 1  : s.format(_T("One backup was deleted"));                   break;
    default : s.format(_T("%i backups have been deleted."), noDeleted); break;
    }

  setStatus(s);   setDbSts();
  }


void LeePassDlg::onDeleteGroup() {
String group;
void*  x;
uint   grpId;
String q;

  if (!saveRcd) return;

  saveCurrentRcd();

  if (!toolBar.getCurSel(ID_GroupCbx, group, x)) return;

  grpId = (uint) x;

  q = _T("Delete ") + group + _T(" Group and all Entries in the Group");

  if (msgYesNoBox(q) != IDYES) return;

  groups.del(grpId, toolBar, ID_GroupCbx);   saveDB |= true;

  installEntries();

  setLabels();   q = _T("Deleted ") + group + _T(" Group and Entries in the Group");

  setStatus(q);
  }


void LeePassDlg::onImportFile() {
PathDlgDsc dsc;
String     path;
LastPass   lastPass;

  saveCurrentRcd();

  if (!kpLib.importFile(path)) return;

  saveDB |= true;

  groups.initialize();

  groups.install(toolBar, ID_GroupCbx);   groups.install(groupCtl, 0);

  installEntries();
  }


void LeePassDlg::onExpungeFile() {
PathDlgDsc dsc;
String     path;
LastPass   lastPass;

  saveCurrentRcd();

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


void LeePassDlg::setDbSts() {
String s = path;
String t;
int    nGrps = groups.nData();

  if (nGrps)        {s += t.format(_T("   No. of Groups: %i"), nGrps);}
  if (noCbxEntries) {s += t.format(_T("   No. of Entries: %i"), noCbxEntries);}

  statusBar.setText(1, s);
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

  toolBar.setWthPercent(ID_DeleteMenu,   70);
  toolBar.addMenu(ID_DeleteMenu,   IDR_DeleteMenu,   _T("Delete"));
  toolBar.setWthPercent(ID_LastPassMenu, 70);
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


