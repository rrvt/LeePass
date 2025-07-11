// LeePassDlg.cpp : implementation file
//

#include "pch.h"
#include "LeePassDlg.h"
#include "AboutDlgKp.h"
#include "ClipBoard.h"
#include "FileName.h"
#include "GeneratorDlg.h"
#include "GetPathDlg.h"
#include "Groups.h"
#include "IniFileEx.h"
#include "KpIter.h"
#include "KpMasterKey.h"
#include "KpSaveRestore.h"
#include "LastPass.h"
#include "LeePass.h"
#include "MessageBox.h"
#include "PasswordDlg.h"
#include "PasswordNewDlg.h"
#include "Remove.h"
#include "Status.h"


       TCchar* GlobalSect    = _T("Global");
       TCchar* KpDbPathKey   = _T("KpDbPath");
       TCchar* KpDBDbPathKey = _T("KpDBDbPath");

static TCchar* GroupCaption  = _T(" Groups ");
static TCchar* EntryCaption  = _T(" Entries ");
static TCchar* NewRecordSts  = _T("New LeePass Entry");
static TCchar* SetSaveQuestion = _T("Should Read Only Status be changed?");

//Thread timerThrd;


IMPLEMENT_DYNAMIC(LeePassDlg, CDialogEx)


BEGIN_MESSAGE_MAP(LeePassDlg, CDialogEx)

  ON_COMMAND(      ID_NewKpDb,       &onNewKpDb)
  ON_COMMAND(      ID_OpenKpDb,      &onOpenKpDb)
  ON_COMMAND(      ID_Login,         &onLogin)

  ON_CBN_SELCHANGE(ID_GroupCbx,      &onGroupCbx)          // Process selection from list

  ON_COMMAND(      ID_NewEntry,      &onNewEntry)
  ON_BN_CLICKED(   IDC_Generate,     &onGenerate)
  ON_CBN_SELCHANGE(ID_EntryCbx,      &onEntryCbx)          // Process selection from list

  ON_COMMAND(      ID_ToggleSave,    &onToggleSave)

  ON_COMMAND(      ID_MoveLeft,      &onMoveLeft)
  ON_COMMAND(      ID_FindEntry,     &onFindEntry)
  ON_COMMAND(      ID_FindNext,      &onFindNext)
  ON_COMMAND(      ID_MoveRight,     &onMoveRight)

  ON_COMMAND(      ID_ChngMasterKey, &onchangeMasterKey)
  ON_COMMAND(      ID_Save,          &onSave)

  ON_CBN_SELCHANGE(ID_DeleteMenu,    &onDeleteMenu)          // Send Command Message with ID_...
  ON_COMMAND(      ID_DeleteEntry,   &onDeleteEntry)
  ON_COMMAND(      ID_DeleteGroup,   &onDeleteGroup)

  ON_CBN_SELCHANGE(ID_LastPassMenu,  &onLastPassMenu)          // Send Command Message with ID_...
  ON_COMMAND(      ID_ImportFile,    &onImportFile)
  ON_COMMAND(      ID_ExportFile,    &onExportFile)
  ON_COMMAND(      ID_ExpungeFile,   &onExpungeFile)

  ON_COMMAND(      ID_SaveEntry,     &onSaveEntry)
  ON_COMMAND(      ID_SaveGroup,     &onSaveGroup)
  ON_COMMAND(      ID_RecoverEntries,&onRecoverEntries)
  ON_COMMAND(      ID_RecoverFmrDB,  &onRecoverFmrDB)

  ON_COMMAND(      ID_GeneratePswd,  &onGeneratePswd)
  ON_COMMAND(      ID_StartURL,      &onOpenBrowser)
  ON_COMMAND(      ID_CopyUserName,  &onCopyUserName)
  ON_COMMAND(      ID_CopyPassword,  &onCopyPassword)
  ON_MESSAGE(      ClearClipBoardMsg,&onClearClipBoard)

  ON_COMMAND(      ID_RmvDuplicates, &onRemoveDups)

  ON_COMMAND(      ID_Help,          &onHelp)
  ON_COMMAND(      ID_About,         &onAppAbout)
  ON_COMMAND(      ID_Exit,          &onExit)

  ON_EN_SETFOCUS(  IDC_URL,          &onFocusUrl)
  ON_EN_SETFOCUS(  IDC_UserName,     &onFocusUserName)
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
  ON_WM_CLOSE()
END_MESSAGE_MAP()


LeePassDlg::LeePassDlg(TCchar* helpPth, CWnd* pParent) :
                       CDialogEx(IDD_LeePass, pParent), toolBar(), statusBar(),
                       isInitialized(false), nRecords(0), dbOpen(false),
                       rcd(kpLib.kpRcd), newRcd(true), saveRcd(false),
                       dirty(false), saveDB(false), status(statusBar, saveRcd),
                       helpPath(helpPth) { }


LeePassDlg::~LeePassDlg() {clipBoard.stop();   clipBoard.clear();   winPos.~WinPos();}


void LeePassDlg::clearDlg() {
  clear(titleCtl);        clear(urlCtl);        clear(userNameCtl);   clear(pswdCtl);
  clear(extraCtl);        clear(creationCtl);   clear(lastModCtl);    clear(lastAccessCtl);
  clear(binaryDescCtl);   clear(groupCtl);
  }


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

  clipBoard.set(m_hWnd, ClearClipBoardMsg);

  generateCtl.SetIcon(theApp.LoadIcon(IDR_Generate));

  if (!statusBar.create(this, IDC_StatusBar)) return false;

  statusBar.setReady();

  winPos.initialPos(this, winRect);   toolBar.move(winRect);   statusBar.move(winRect);

  iniFile.read(GlobalSect, getDbPathKey(), path, _T(""));

  if (!path.isEmpty()) {setTitle(getMainName(path));   status.setDb(path, nRecords);}

  status.set(_T("Login"));   isInitialized = true;   return true;
  }


void LeePassDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_Title,      titleCtl);
  DDX_Control(pDX, IDC_URL,        urlCtl);
  DDX_Control(pDX, IDC_UserName,   userNameCtl);
  DDX_Control(pDX, IDC_Pswd,       pswdCtl);
  DDX_Control(pDX, IDC_Notes,      extraCtl);
  DDX_Control(pDX, IDC_BinaryDesc, binaryDescCtl);
  DDX_Control(pDX, IDC_GroupUpdt,  groupCtl);
  DDX_Control(pDX, IDC_Creation,   creationCtl);
  DDX_Control(pDX, IDC_LastMod,    lastModCtl);
  DDX_Control(pDX, IDC_LastAccess, lastAccessCtl);
  DDX_Control(pDX, IDC_Generate,   generateCtl);
  }


void LeePassDlg::onNewKpDb() {
PasswordNewDlg dlg;
KpMasterKey    mk;

  saveCurrentDB();

  if (dlg.DoModal() != IDOK || !kpLib.newDatabase(path, dlg.password)) return;

  mk.save(dlg.password);   kpLib.dspEncryption();   initMgmt();   finOpen();   saveDB = true;
  }


void LeePassDlg::onOpenKpDb() {
PathDlgDsc pathDlgDsc(_T("LeePass Database"), path, _T("kdb"), _T("*.kdb"));

  saveCurrentDB();

  if (!getOpenDlg(pathDlgDsc, path)) return;

  initMgmt();   savePath();   status.setDb(path, nRecords);
  }


void LeePassDlg::onLogin() {
PasswordDlg dlg;
String      dbName = getMainName(path);
KpMasterKey mk;

  if (dbOpen) return;

  dlg.title = _T("Enter Password for ") + dbName;

  if (dlg.DoModal() != IDOK) return;

  initMgmt();   if (!kpLib.openDatabase(path, dlg.password)) return;

  mk.save(dlg.password);   finOpen();
  }


void LeePassDlg::initMgmt() {

  toolBar.clearCbx(ID_GroupCbx);   toolBar.clearCbx(ID_EntryCbx);

  groups.clear();   rcd.clear();   clearDlg();

  nRecords = 0;   dbOpen = newRcd = saveRcd = dirty = saveDB = false;
  }


void LeePassDlg::finOpen() {

  dbOpen = true;   groups.initialize();   installTbrGroupCbx();   installTbrEntryCbx();

  groups.install(groupCtl, 0);   setLabels();   setTitle(getMainName(path));

  newRcd = true;   status.set(NewRecordSts);   status.setDb(path, nRecords);

  showGenerateButton();
  }


void LeePassDlg::onGroupCbx() {
String s;
void*  x;

  if (toolBar.getCurSel(ID_GroupCbx, s, x))
           {saveCurrentRcd();   rcd.clear();   clearDlg();   setLabels();   installTbrEntryCbx();}

  groups.install(groupCtl, s);   showGenerateButton();
  }


static TCchar* AllGroups = _T(" All Groups ");


void LeePassDlg::installTbrGroupCbx() {
String  s;
void*   x;
GrpIter iter(groups);
Group*  grp;

  if (!groups.isDirty()) return;

  toolBar.getCurSel(ID_GroupCbx, s, x);

  toolBar.clearCbx(ID_GroupCbx);   toolBar.addCbxItemSorted(ID_GroupCbx, AllGroups, 0);

  for (grp = iter(); grp; grp = iter++)
             if (grp->name != MasterKey) toolBar.addCbxItemSorted(ID_GroupCbx, grp->name, grp->id);

  if (!toolBar.setCurSel(ID_GroupCbx, s) && !toolBar.setCurSel(ID_GroupCbx, AllGroups))
                                                                 toolBar.setCurSel(ID_GroupCbx, 0);

  toolBar.setWthPercent(ID_GroupCbx, 70);   toolBar.setWidth(ID_GroupCbx);

  toolBar.setHeight(ID_GroupCbx);
  }


void LeePassDlg::installTbrEntryCbx() {
KpIter   iter;
KpEntry* kpEntry;
String   group;
void*    x;
uint     grpId = 0;

  toolBar.clearCbx(ID_EntryCbx);   nRecords = 0;

  if (toolBar.getCurSel(ID_GroupCbx, group, x)) grpId = (uint) x;

  for (kpEntry = iter(); kpEntry; kpEntry = iter++)
                if (!grpId || kpEntry->uGroupId == grpId) installEntry(kpEntry->pszTitle, kpEntry);

  toolBar.setCurSel(ID_EntryCbx, -1);   toolBar.setCaption(ID_EntryCbx, EntryCaption);

  toolBar.setWthPercent(ID_EntryCbx, 70);

  toolBar.setWidth(ID_EntryCbx);   toolBar.setHeight(ID_EntryCbx);   status.setDb(path, nRecords);
  }


void LeePassDlg::installEntry(TCchar* title, void* data) {
String s = title;   if (s == MasterKey) return;

  toolBar.addCbxItemSorted(ID_EntryCbx, s, (int) data);   nRecords++;
  }


void LeePassDlg::onNewEntry() {
String  s;
void*   x;
TCchar* tc = 0;

  saveCurrentRcd();   rcd.clear();   clearDlg();

  newRcd = saveRcd = true;

  if (toolBar.getCurSel(ID_GroupCbx, s, x)) tc = s.str();

  groups.install(groupCtl, tc);    setLabels();

  status.set(NewRecordSts);    showGenerateButton();
  }


void LeePassDlg::onMoveLeft() {
int i = toolBar.getCurSel(ID_EntryCbx) - 1;   if (i < 0) i = 0;

  saveCurrentRcd();   loadEntry(i);
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
construct arithmetical expressions from numbers and the operations +, ?, �, and �.
*/

void LeePassDlg::onFindEntry() {
String    s;
void*     x;
uint      curGrpId = toolBar.getCurSel(ID_GroupCbx, s, x) ? (uint) x : 0;
KpEntry*  kpEntry  = kpSrch.find(curGrpId);   if (!kpEntry) return;
uint      grpId    = kpEntry->uGroupId;
int       i;

  if (grpId && grpId != curGrpId) {
    i = findCbxGroup(grpId);   if (i >= 0) {toolBar.setCurSel(ID_GroupCbx, i);   installTbrEntryCbx();}
    }

  finFind(kpEntry);
  }


void LeePassDlg::onFindNext() {if (kpSrch.isInitialized()) finFind(kpSrch.next());}


void LeePassDlg::finFind(KpEntry* kpEntry) {
int cboBxX = findCbxEntry(kpEntry);   if (cboBxX < 0) return;

  saveCurrentRcd();   loadEntry(cboBxX);
  }


int LeePassDlg::findCbxGroup(uint grpId) {
int  n = toolBar.getCbxCount(ID_GroupCbx);
int  i;

  for (i = 0; i < n; i++) if (grpId == (uint) toolBar.getCbxData(ID_GroupCbx, i)) return i;

  return -1;
  }


int LeePassDlg::findCbxEntry(void* kpe) {
int n = toolBar.getCbxCount(ID_EntryCbx);
int i;

  for (i = 0; i < n; i++) if (kpe == toolBar.getCbxData(ID_EntryCbx, i)) return i;

  return -1;
  }



void LeePassDlg::onMoveRight() {
int n = toolBar.getCbxCount(ID_EntryCbx);
int i = toolBar.getCurSel(ID_EntryCbx) + 1;   if (i >= n) i = n - 1;

  saveCurrentRcd();   loadEntry(i);
  }


// Selects and loads the ith entry in Entry Combo Box

void LeePassDlg::loadEntry(int i) {
int n = toolBar.getCbxCount(ID_EntryCbx);   if (i >= n) i = n-1;   if (i < 0) i = 0;

  toolBar.setCurSel(ID_EntryCbx, i);   toolBar.Invalidate();   loadEntry();
  }


void LeePassDlg::onEntryCbx() {saveCurrentRcd();   loadEntry();}


void LeePassDlg::loadEntry() {
String    s;
void*     x;

  if (!toolBar.getCurSel(ID_EntryCbx, s, x)) return;

  newRcd = dirty = false;   rcd = (KpEntry*) x;

  rcd.setTitle(titleCtl);
  rcd.setURL(urlCtl);
  rcd.setUserName(userNameCtl);
  rcd.setPassword(pswdCtl);
  rcd.setExtra(extraCtl);
  rcd.setCreation(creationCtl);
  rcd.setLastMod(lastModCtl);
  rcd.setLastAccess(lastAccessCtl);
  rcd.setBinDesc(binaryDescCtl);
  rcd.setGroup(groupCtl);

  showGenerateButton();   status.set(rcd);
  }



void LeePassDlg::onOpenBrowser() {
Cstring url;

  urlCtl.GetWindowText(url);   if (url.isEmpty()) return;

  ShellExecute(0, _T("open"), url, 0, 0, SW_SHOWNORMAL);
  }


void LeePassDlg::onCopyUserName() {
Cstring   userName;
String    s;

  clipBoard.stop();

  userNameCtl.GetWindowText(userName);   if (userName.isEmpty()) return;

  clipBoard.load(userName);

  status.tmp(s.format(_T("User Name: %s copied to ClipBoard"), userName.str()));
  }


void LeePassDlg::onCopyPassword() {
Cstring   password;

  clipBoard.stop();

  pswdCtl.GetWindowText(password);   if (password.isEmpty()) return;

  clipBoard.load(password);   password.expunge();

  clipBoard.start(30);   status.tmp(_T("Password is in ClipBoard for 30 seconds"));
  }


LRESULT LeePassDlg::onClearClipBoard(WPARAM wParam, LPARAM lParam)
                                                  {clipBoard.clear();   status.set(0);   return 0;}


void LeePassDlg::saveCurrentRcd() {
bool titleChg = false;
bool groupChg = false;

  if (!dbOpen || !isLegalRcd()) return;

  if (rcd.isProhibited(titleCtl) || rcd.isProhibited(userNameCtl) || rcd.isProhibited(groupCtl))
                  {messageBox(_T("The text in title, username or group is prohibited"));   return;}

  if (newRcd) {saveNewRcd(rcd);  return;}

  dirty |= titleChg = rcd.getTitle(titleCtl);
  dirty |= rcd.getURL(urlCtl);
  dirty |= rcd.getUserName(userNameCtl);
  dirty |= rcd.getPassword(pswdCtl);
  dirty |= rcd.getExtra(extraCtl);
  dirty |= rcd.getBinaryDesc(binaryDescCtl);
  dirty |= groupChg = rcd.getGroup(groupCtl);

  if (!saveRcd && dirty) {
    String q;   q.format(_T("Entry \"%s\" has been modified, save entries?"), rcd.title());

    if (msgYesNoBox(q) != IDYES) return;

    saveRcd = true;   showGenerateButton();   status.set(0);   toolBar.Invalidate();
    }

  if (dirty) rcd.updateLastMod(lastModCtl);   rcd.updateLastAccess(lastAccessCtl);

  dirty |= rcd.update();

  if (groupChg) installTbrGroupCbx();
  if (titleChg) installTbrEntryCbx();

  shiftDirty();
  }


void LeePassDlg::saveNewRcd(Record& rcd) {
bool groupChg = false;

  dirty |= rcd.getTitle(titleCtl);
  dirty |= rcd.getURL(urlCtl);
  dirty |= rcd.getUserName(userNameCtl);
  dirty |= rcd.getPassword(pswdCtl);
  dirty |= rcd.getExtra(extraCtl);
  dirty |= rcd.getBinaryDesc(binaryDescCtl);
  dirty |= groupChg = rcd.getGroup(groupCtl);

  if (dirty && rcd.add()) {
    rcd.updateCreation(creationCtl);   rcd.updateLastMod(lastModCtl);
    rcd.updateLastAccess(lastAccessCtl);

    if (groupChg) {installTbrGroupCbx();  groups.install(groupCtl, rcd.group());}
    else                               groupCtl.SelectString(-1, rcd.group());

    installTbrEntryCbx();

    newRcd = false;   status.set(rcd);    shiftDirty();
    }
  }


void LeePassDlg::onchangeMasterKey() {KpMasterKey mk;   saveDB = mk.change();}


void LeePassDlg::onExit() {saveCurrentRcd();   saveDataBase();   CDialogEx::OnOK();}


void LeePassDlg::OnClose() {saveCurrentDB();   CDialogEx::OnClose();}


void LeePassDlg::saveCurrentDB() {

  saveCurrentRcd();

  if (saveDB && msgYesNoBox(_T("Save Current Database")) == IDYES) saveDataBase();
  }


void LeePassDlg::onSave() {

  if (isSaveRcdSet()) saveCurrentRcd();

  saveDataBase();
  }


void LeePassDlg::saveDataBase() {

  if (saveDB && kpLib.saveDatabase(path)) {savePath();   saveDB = false;}
  }


void LeePassDlg::savePath() {iniFile.write(GlobalSect, getDbPathKey(), path);}


void LeePassDlg::setLabels() {

  rcd.setTitleLbl(titleCtl);
  rcd.setURLLbl(urlCtl);
  rcd.setUserNameLbl(userNameCtl);
  rcd.setPasswordLbl(pswdCtl);
  rcd.setExtraLbl(extraCtl);
  rcd.setCreationLbl(creationCtl);
  rcd.setLastModLbl(lastModCtl);
  rcd.setLastAccessLbl(lastAccessCtl);
  rcd.setBinDescLbl(binaryDescCtl);
  rcd.setGroupLbl(groupCtl);

  generateCtl.ShowWindow(SW_SHOW);
  }


void LeePassDlg::onFocusTitle()      {rcd.clrTitleLbl(titleCtl);}
void LeePassDlg::onFocusUrl()        {rcd.clrURLLbl(urlCtl);}
void LeePassDlg::onFocusUserName()   {rcd.clrUserNameLbl(userNameCtl);}
void LeePassDlg::onFocusPswd()       {rcd.clrPasswordLbl(pswdCtl);}
void LeePassDlg::onFocusNotes()      {rcd.clrExtraLbl(extraCtl);}
void LeePassDlg::onFocusBinarydesc() {rcd.clrBinDescLbl(binaryDescCtl);}
void LeePassDlg::onFocusGroupUpdt()  {rcd.clrGroupLbl(groupCtl);}


void LeePassDlg::onToggleSave() {saveRcd ^= true;   showGenerateButton();   status.set(0);}


void LeePassDlg::showGenerateButton()
                          {int show = saveRcd ? SW_SHOW : SW_HIDE;   generateCtl.ShowWindow(show);}



void LeePassDlg::onGenerate() {
GeneratorDlg dlg;

  if (dlg.DoModal() == IDOK) {setEdit(pswdCtl, dlg.password);}

  dlg.password.expunge();
  }



void LeePassDlg::onDeleteEntry() {
int      index;
String   s;
void*    x;
String   dsc;
String   q;
KpIter   iter;
KpEntry* kpEntry;

  if (!isSaveRcdSet()) return;

  saveCurrentRcd();

  index = toolBar.getCurSel(ID_EntryCbx);
  if (!toolBar.getCurSel(ID_EntryCbx, s, x)) return;

  kpEntry = (KpEntry*) x;
  rcd     = kpEntry;   dsc = rcd.getLongEntryDsc();

  q = _T("Delete Entry: ") + dsc;   if (msgYesNoBox(q) != IDYES) return;

  for (kpEntry = iter(); kpEntry; kpEntry = iter++)
                                         if (rcd.kpId() == kpEntry->uuid) {iter.remove();   break;}
  installTbrEntryCbx();

  q = dsc + _T(" has been deleted");   status.set(q);   status.setDb(path, nRecords);

  loadEntry(index);
  }


void LeePassDlg::onGeneratePswd() {GeneratorDlg dlg;   dlg.DoModal();   dlg.password.expunge();}


void LeePassDlg::onRemoveDups() {
Remove remove;
int    noDeleted;
String s;

  saveCurrentRcd();

  noDeleted = remove.duplicates();    saveDB |= noDeleted != 0;    if (!saveDB) return;

  installTbrEntryCbx();

  switch (noDeleted) {
    case 0  : s.format(_T("No duplicate entries were deleted"));                  break;
    case 1  : s.format(_T("One duplicate entry was deleted"));                    break;
    default : s.format(_T("%i duplicate entries have been deleted."), noDeleted); break;
    }

  status.set(s);   status.setDb(path, nRecords);
  }


void LeePassDlg::onDeleteGroup() {
String group;
void*  x;
uint   grpId;
String q;

  if (!isSaveRcdSet()) return;

  saveCurrentRcd();

  if (!toolBar.getCurSel(ID_GroupCbx, group, x)) return;

  grpId = (uint) x;

  q = _T("Delete ") + group + _T(" Group and all Entries in the Group");

  if (msgYesNoBox(q) != IDYES) return;

  if (!groups.del(grpId, toolBar, ID_GroupCbx)) return;

  saveDB |= true;   dirty = false;   installTbrGroupCbx();   installTbrEntryCbx();

  setLabels();   q = _T("Deleted ") + group + _T(" Group and Entries in the Group");

  status.set(q);
  }



bool LeePassDlg::isSaveRcdSet() {

  if (saveRcd) return true;

  if (msgYesNoBox(SetSaveQuestion) != IDYES) return false;

  saveRcd = true;    status.set(0);   return true;
  }




bool LeePassDlg::isLegalRcd() {
int n = 0;

  if (!isEmpty(titleCtl,    TitleLbl))    n++;
  if (!isEmpty(urlCtl,      URLLbl))      n++;
  if (!isEmpty(userNameCtl, UserNameLbl)) n++;

  return n >= 2;
  }



void LeePassDlg::onImportFile() {
PathDlgDsc dsc;
String     path;
LastPass   lastPass;

  saveCurrentRcd();

  if (!kpLib.importFile(path)) return;

  saveDB |= true;   dirty = false;

  groups.initialize();

  installTbrGroupCbx();   groups.install(groupCtl, 0);    installTbrEntryCbx();

  lastPass.expungeFile(path);
  }


void LeePassDlg::onExportFile()     {saveCurrentRcd();   kpLib.exportFile();}


void LeePassDlg::onSaveEntry()      {
String        s;
void*         x;
KpSaveRestore svRst;

  saveCurrentRcd();

  if (!toolBar.getCurSel(ID_EntryCbx, s, x)) return;

  if (x) svRst.saveEntry((KpEntry*) x);
  }


void LeePassDlg::onSaveGroup()      {
String        s;
void*         x;
KpSaveRestore svRst;

  saveCurrentRcd();

  if (!toolBar.getCurSel(ID_GroupCbx, s, x)) return;

  if (x) svRst.saveGroup((uint) x);
  }


void LeePassDlg::onRecoverEntries() {
KpSaveRestore svRst;

  saveCurrentRcd();   svRst.recover();
  }


void LeePassDlg::onRecoverFmrDB() {
  saveCurrentRcd();

  }


void LeePassDlg::onExpungeFile() {
PathDlgDsc dsc;
String     path;
LastPass   lastPass;

  saveCurrentRcd();

  dsc(_T("Expunge File"), _T(""), _T("*"), _T("*.*"));

  if (getOpenDlg(dsc, path)) lastPass.expungeFile(path);
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

  toolBar.addCBx(ID_GroupCbx, GroupCaption);
  toolBar.addCBx(ID_EntryCbx, EntryCaption);

  toolBar.setWthPercent(ID_DeleteMenu,   85);
  toolBar.addMenu(      ID_DeleteMenu,   IDR_DeleteMenu,   _T("Delete"));
  toolBar.setWidth(     ID_DeleteMenu);
  toolBar.setWthPercent(ID_LastPassMenu, 75);
  toolBar.addMenu(      ID_LastPassMenu, IDR_LastPassMenu, _T("LastPass"));
  toolBar.setWidth(     ID_LastPassMenu);
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


/////---------

#if 0
  ON_COMMAND(      ID_RmvLPImports,  &onRmvLPImports)
  ON_COMMAND(      ID_RmvRdndtGrps,  &onRmvRdndtGrps)
  ON_COMMAND(      ID_RmvBackups,    &onRmvBackups)
#endif
#if 0
void LeePassDlg::onRmvLPImports() {
Remove remove;
int    noDeleted;
String s;

  saveCurrentRcd();

  noDeleted = remove.oldLPImports();    saveDB |= noDeleted != 0;    if (!saveDB) return;

  installTbrEntryCbx();

  switch (noDeleted) {
    case 0  : s.format(_T("No Old LP Import entries were deleted"));                  break;
    case 1  : s.format(_T("One Old LP Import entry was deleted"));                    break;
    default : s.format(_T("%i Old LP Import entries have been deleted."), noDeleted); break;
    }

  status.set(s);   status.setDb(path, nRecords);
  }


void LeePassDlg::onRmvRdndtGrps() {
Remove remove;
int    noDeleted;
String s;

  saveCurrentRcd();

  noDeleted = remove.redundantGrps();   saveDB |= noDeleted != 0;   if (!saveDB) return;

  groups.initialize();   installTbrGroupCbx();

  switch (noDeleted) {
    case 0  : s.format(_T("No groups were deleted"));                  break;
    case 1  : s.format(_T("One group was deleted"));                   break;
    default : s.format(_T("%i groups have been deleted."), noDeleted); break;
    }

  status.set(s);   status.setDb(path, nRecords);
  }


void LeePassDlg::onRmvBackups() {
Remove remove;
int    noDeleted;
String s;

  saveCurrentRcd();

  noDeleted = remove.backups();   saveDB |= noDeleted != 0;   if (!saveDB) return;

  groups.initialize();   installTbrGroupCbx();

  switch (noDeleted) {
    case 0  : s.format(_T("No backups were deleted"));                  break;
    case 1  : s.format(_T("One backup was deleted"));                   break;
    default : s.format(_T("%i backups have been deleted."), noDeleted); break;
    }

  status.set(s);   status.setDb(path, nRecords);
  }
#endif

//    toolBar.setCurSel(ID_EntryCbx, -1);   toolBar.setCaption(ID_EntryCbx, EntryCaption);
#if 1
#else
//int       cboBxX;

SearchDlg dlg;
uint      grpId;
  kpLib.clrSrch();

  if (dlg.DoModal() != IDOK) return;

  curGrpId = toolBar.getCurSel(ID_GroupCbx, s, x) ? (uint) x : 0;

  grpId = dlg.groupMode == AllGrpsMode ? 0 : curGrpId;

  kpLib.setSrchGrp(grpId);

  kpEntry = kpLib.find(dlg);   if (!kpEntry) return;
#endif
#if 1
#else
  cboBxX = findEntry(kpEntry);   if (cboBxX < 0) return;

  saveCurrentRcd();   loadEntry(cboBxX);
#endif
#if 1
#else
  //KpEntry* kpEntry;
  //
  kpEntry = kpSrch.next();

  cboBxX = findEntry(kpEntry);   if (cboBxX < 0) return;

  saveCurrentRcd();   loadEntry(cboBxX);
#endif
#if 1
#else
    uint x;
    x = (uint) toolBar.getCbxData(ID_GroupCbx, i);

    if (x == grpId) return i;
#endif
#if 1
#else
    String s;
    void*  x;
    x = toolBar.getCbxData(ID_EntryCbx, i);

    if (x == kpe) return i;
#endif
#if 1
#else
  if (dirty) {rcd.lastMod.today();      rcd.lastMod.set(lastModCtl);}
  else {
    rcd.lastAccess.today();   dirty = rcd.updateLastAccess();   rcd.lastAccess.set(lastAccessCtl);
    }
#endif
#if 0
bool LeePassDlg::isLegalRcd(KpRecord& rcd) {
int n = 0;

  if (!isTitleEmpty(titleCtl))         n++;
  if (!isURLEmpty(urlCtl))             n++;
  if (!isUserNameEmpty(userNameCtl))   n++;

  return n >= 2;
  }
#endif
#if 1
#else
  saveDB = true;   toolBar.clearCbx(ID_GroupCbx);   toolBar.clearCbx(ID_EntryCbx);

  groups.clear();   rcd.clear();
#endif
#if 1
#else
  toolBar.clearCbx(ID_GroupCbx);   toolBar.clearCbx(ID_EntryCbx);

  groups.clear();   rcd.clear();   clearDlg();

  dbOpen = newRcd = saveRcd = dirty = saveDB = false;   nRecords = 0;
#endif

