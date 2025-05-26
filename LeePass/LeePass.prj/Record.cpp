// Password LPentry data




#include "pch.h"
#include "Record.h"
#include "Groups.h"
#include "KpDataDef.h"
#include "MessageBox.h"
#include "Utility.h"


typedef struct _PW_ENTRY KpEntry;
       TCchar* TitleLbl      = _T("<Title>");
       TCchar* URLLbl        = _T("<URL>");
       TCchar* UserNameLbl   = _T("<user Name>");
static TCchar* PasswordLbl   = _T("<Password>");
static TCchar* ExtraLbl      = _T("<Extra>");
static TCchar* GroupLbl      = _T("<Group>");
static TCchar* CreationLbl   = _T("<Creation>");
static TCchar* LastModLbl    = _T("<Last Mod>");
static TCchar* LastAccessLbl = _T("<Last Acc>");
static TCchar* ExpireLbl     = _T("Expire Date");
static TCchar* BinDescLbl    = _T("<Binary Desc>");

Record::Record(KpRecord& kpRecord) : kpRcd(kpRecord) {}

Record::~Record() {clear();}


void Record::setTitleLbl(CEdit& ctl)         {setLbl(ctl, TitleLbl);}
void Record::setURLLbl(CEdit& ctl)           {setLbl(ctl, URLLbl);}
void Record::setUserNameLbl(CEdit& ctl)      {setLbl(ctl, UserNameLbl);}
void Record::setPasswordLbl(CEdit& ctl)      {setLbl(ctl, PasswordLbl);}
void Record::setExtraLbl(CEdit& ctl)         {setLbl(ctl, ExtraLbl);}
void Record::setCreationLbl(CStatic& ctl)    {setLbl(ctl, CreationLbl);}
void Record::setLastModLbl(CStatic& ctl)     {setLbl(ctl, LastModLbl);}
void Record::setLastAccessLbl(CStatic& ctl)  {setLbl(ctl, LastAccessLbl);}
void Record::setBinDescLbl(CEdit& ctl)       {setLbl(ctl, BinDescLbl);}
void Record::setGroupLbl(CComboBox& ctl)     {setLbl(ctl, GroupLbl);}

void Record::clrTitleLbl(CEdit& ctl)         {clrLbl(ctl, TitleLbl);}
void Record::clrURLLbl(CEdit& ctl)           {clrLbl(ctl, URLLbl);}
void Record::clrUserNameLbl(CEdit& ctl)      {clrLbl(ctl, UserNameLbl);}
void Record::clrPasswordLbl(CEdit& ctl)      {clrLbl(ctl, PasswordLbl);}
void Record::clrExtraLbl(CEdit& ctl)         {clrLbl(ctl, ExtraLbl);}
void Record::clrCreationLbl(CStatic& ctl)    {clrLbl(ctl, CreationLbl);}
void Record::clrLastModLbl(CStatic& ctl)     {clrLbl(ctl, LastModLbl);}
void Record::clrLastAccessLbl(CStatic& ctl)  {clrLbl(ctl, LastAccessLbl);}
void Record::clrBinDescLbl(CEdit& ctl)       {clrLbl(ctl, BinDescLbl);}
void Record::clrGroupLbl(CComboBox& ctl)     {clrLbl(ctl, GroupLbl);}


void Record::updateCreation(CStatic& ctl)   {kpRcd.creation.today();    setCreation(ctl);}
void Record::updateLastMod(CStatic& ctl)    {kpRcd.lastMod.today();     setLastMod(ctl);}
void Record::updateLastAccess(CStatic& ctl) {kpRcd.lastAccess.today();  setLastAccess(ctl);}


void Record::setTitle(CEdit& ctl)        {set(TitleLbl,    kpRcd.title,    ctl);}
void Record::setURL(CEdit& ctl)          {set(URLLbl,      kpRcd.url,      ctl);}
void Record::setUserName(CEdit& ctl)     {set(UserNameLbl, kpRcd.userName, ctl);}
void Record::setPassword(CEdit& ctl)     {set(PasswordLbl, kpRcd.password, ctl);}
void Record::setExtra(CEdit& ctl)        {set(ExtraLbl,    kpRcd.extra,    ctl);}
void Record::setCreation(CStatic& ctl)   {kpRcd.creation.set(ctl);}
void Record::setLastMod(CStatic& ctl)    {kpRcd.lastMod.set(ctl);}
void Record::setLastAccess(CStatic& ctl) {kpRcd.lastAccess.set(ctl);}
void Record::setBinDesc(CEdit& ctl)      {set(BinDescLbl,  kpRcd.binDesc,  ctl);}
void Record::setGroup(CComboBox& ctl)    {groups.install(ctl, kpRcd.group);}


void Record::set(TCchar* lbl, String& fld, CEdit& ctl) {

  if (fld.isEmpty()) {ctl.SetWindowText(lbl);  return;}

  ctl.SetWindowText(fld);
  }


void Record::clearLabels() {

  clrLbl(kpRcd.group,     GroupLbl);                    // Make sure labels are not stored
  clrLbl(kpRcd.title,     TitleLbl);
  clrLbl(kpRcd.url,       URLLbl);
  clrLbl(kpRcd.userName,  UserNameLbl);
  clrLbl(kpRcd.password,  PasswordLbl);
  clrLbl(kpRcd.extra,     ExtraLbl);
  clrLbl(kpRcd.binDesc,   BinDescLbl);
  }



bool Record::getTitle(     CEdit& ctl) {return get(ctl, TitleLbl,    kpRcd.title);}
bool Record::getURL(       CEdit& ctl) {return get(ctl, URLLbl,      kpRcd.url);}
bool Record::getUserName(  CEdit& ctl) {return get(ctl, UserNameLbl, kpRcd.userName);}
bool Record::getPassword(  CEdit& ctl) {return get(ctl, PasswordLbl, kpRcd.password);}
bool Record::getExtra(     CEdit& ctl) {return get(ctl, ExtraLbl,    kpRcd.extra);}
bool Record::getBinaryDesc(CEdit& ctl) {return get(ctl, BinDescLbl,  kpRcd.binDesc);}


bool Record::get(CEdit& ctl, TCchar* lbl, String& fld) {
Cstring cs;
String  s;
bool    dirty;

  ctl.GetWindowText(cs);   s = cs;   s.trim();   if (s == lbl) return false;

  dirty |= fld != s;   fld = s;    return dirty;
  }


bool Record::getGroup(CComboBox& ctl) {
Cstring cs;
String  s;
bool    dirty;

  ctl.GetWindowText(cs);   s = cs;   s.trim();   if (s == GroupLbl) return false;

  dirty = kpRcd.group != s;   kpRcd.group = s;   return dirty;
  }



//////////------------------------

#if 1
#else
  id = groups.getID(group);

  if (id) {PE_SetGroupID(kpEntry, id);    return true;}

  q = _T("Add "); q += (TCchar*) group;  q += _T(" to Groups?");

  if (msgYesNoBox(q) == IDYES) {PE_SetGroupID(kpEntry, groups.add(group));   return true;}

  return false;
#endif
#if 1
#else
  PE_SetBinaryDesc(kpEntry, binaryDesc);
  PE_SetBinaryData(kpEntry, binaryData, binDataLng);
#endif
#if 0
String& Record::getLongEntryDsc() {
static String dsc;

  dsc.clear();                                                      dsc  = title;
  if (!url.isEmpty())      {if (!dsc.isEmpty()) dsc += _T(", ");    dsc += url;}
  if (!userName.isEmpty()) {if (!dsc.isEmpty()) dsc += _T(", ");    dsc += userName;}
  if (!group.isEmpty())    {if (!dsc.isEmpty()) dsc += _T(", ");    dsc += group;}

  return dsc;
  }
#endif
#if 0
bool Record::updateTitle(CEdit& ctl) {
  if (!kpEntry || !title.get(ctl)) return false;

  PE_SetTitle(kpEntry, title);   return true;
  }


bool Record::updateURL(CEdit& ctl) {
  if (!kpEntry || !url.get(ctl)) return false;

  PE_SetURL(kpEntry, url);   return true;
  }


bool Record::updateUserName(CEdit& ctl) {
  if (!kpEntry || !userName.get(ctl)) return false;

  PE_SetUserName(kpEntry, userName);   return true;
  }


bool Record::updatePassword(CEdit& ctl) {
  if (!kpEntry || !password.get(ctl)) return false;

  PE_SetPasswordAndLock(kpMgr, kpEntry, password);   return true;
  }


bool Record::updateExtra(CEdit& ctl) {
  if (!kpEntry || !extra.get(ctl)) return false;

  return PE_SetNotes(kpEntry, extra);
  }
#endif
#if 0
bool Record::updateBinaryDesc(CEdit& ctl) {
  if (!kpEntry || !binDesc.get(ctl)) return false;

  PE_SetBinaryDesc(kpEntry, binDesc);   return true;
  }
#endif
#if 0
//KP_SHARE DWORD Find(void* pMgr, const TCHAR* pszFindString,
//                                            BOOL bCaseSensitive, DWORD fieldFlags, DWORD nStart);

bool Record::find(KpEntry*& kpEntry) {
int       index;

  for (index = findTitle(title, 0); index >= 0; index = findTitle(title, index+1)) {
    kpEntry = GetEntry(kpMgr, index);    if (!kpEntry) continue;

    if (url == _T("http://sn")) {
      if (userName == kpEntry->pszUserName && binDesc == kpEntry->pszBinaryDesc) return true;
      continue;
      }

    if (url == kpEntry->pszURL && userName == kpEntry->pszUserName) return true;
    }

  return false;
  }

//PW_ENTRY* CreateEntry(void* pMgr, DWORD dwGroupID, LPCTSTR lpTitle,
//                         LPCTSTR lpUserName, LPCTSTR lpURL, LPCTSTR lpPassword, LPCTSTR lpNotes);
//BOOL      AddEntry(   void* pMgr, const PW_ENTRY* pTemplate);

bool Record::add() {
uint grpId = groups.getID(group);   if (!grpId) grpId = groups.add(group);

  kpEntry = CreateEntry(kpMgr, grpId, title, userName, url, password, extra);

  if (!kpEntry) return false;

  kpId         = kpEntry->uuid;
  creation     = kpEntry->tCreation;
  lastMod      = kpEntry->tLastMod;
  lastAccess   = kpEntry->tLastAccess;

  return PE_SetBinaryDesc(kpEntry, binDesc);
  }


bool Record::update(KpEntry* kpEntry) {
bool   dirty = false;
uint   grpId;
bool   rslt;

  if (!kpEntry) return false;

  grpId = getGroupId(group);

  if (grpId     != kpEntry->uGroupId)    {PE_SetGroupID(   kpEntry, grpId);      dirty = true;}
  if (title     != kpEntry->pszTitle)    {PE_SetTitle(     kpEntry, title);      dirty = true;}
  if (url       != kpEntry->pszURL)      {PE_SetURL(       kpEntry, url);        dirty = true;}
  if (userName  != kpEntry->pszUserName) {PE_SetUserName(  kpEntry, userName);   dirty = true;}

  UnlockEntryPassword(kpMgr, kpEntry);
  rslt = password != kpEntry->pszPassword;
  LockEntryPassword(kpMgr, kpEntry);
  if (rslt)                 {PE_SetPasswordAndLock(kpMgr,   kpEntry, password);   dirty = true;}
  if (extra    != kpEntry->pszAdditional) {PE_SetNotes(     kpEntry, extra);      dirty = true;}

  if (binDesc  != kpEntry->pszBinaryDesc) {PE_SetBinaryDesc(kpEntry, binDesc);    dirty = true;}

  if (dirty) lastMod.today();
  else      {lastAccess.today();   dirty = true;}

  return dirty;
  }
#endif
#if 0
bool Record::updateGroup(CComboBox& ctl) {
uint   id;
String q;

  if (!kpEntry || !group.get(ctl)) return false;

  id = getGroupId(group);   if (!id) return false;

  return PE_SetGroupID(kpEntry, id);
  }

uint Record::getGroupId(TCchar* name) {
uint   id = groups.getID(name);   if (id) return id;
String q  = _T("Add "); q += (TCchar*) name;  q += _T(" to Groups?");

  return msgYesNoBox(q) == IDYES ? groups.add(group) : 0;
  }
#endif
#if 0
/*
/// Entry structure, containing information about one entry.

typedef struct _PW_ENTRY {
BYTE    uuid[16];             ///< Unique GUID identifying this entry (not only in this database).
DWORD   uGroupId;             ///< ID of the group that contains this entry.
DWORD   uImageId;             ///< Index of the icon in the image list to use for this entry.

TCHAR*  pszTitle;             ///< Title.
TCHAR*  pszURL;               ///< URL.
TCHAR*  pszUserName;          ///< User name.

DWORD   uPasswordLen;         ///< Length of the password (required for memory protection).
TCHAR*  pszPassword;          ///< Password (may be encrypted, use
                                   IKpDatabase::UnlockEntryPassword to decrypt).
TCHAR*  pszAdditional;        ///< Notes.

PW_TIME tCreation;            ///< Time when the entry was created.
PW_TIME tLastMod;             ///< Time when the entry was last modified.
PW_TIME tLastAccess;          ///< Time when the entry was last accessed.
PW_TIME tExpire;              ///< Time when the entry will expire.

TCHAR*  pszBinaryDesc;        ///< A string describing the contents of pBinaryData.
BYTE*   pBinaryData;          ///< Attachment data (of length uBinaryDataLen), may be NULL.
DWORD   uBinaryDataLen;       ///< Length of the attachment data in bytes.
} PW_ENTRY, *PPW_ENTRY, KpEntry;
*/

Record& Record::operator= (KpEntry* kpRcd) {        //

  clear();   if (!kpRcd) return *this;

  kpEntry = kpRcd;

  UnlockEntryPassword(kpMgr, kpEntry);

  kpId         = kpEntry->uuid;
  group        = groups.getName(kpEntry->uGroupId);
  imageId      = kpEntry->uImageId;
  title        = kpEntry->pszTitle;
  url          = kpEntry->pszURL;
  userName     = kpEntry->pszUserName;
  password     = kpEntry->pszPassword;
  extra        = kpEntry->pszAdditional;
  creation     = kpEntry->tCreation;
  lastMod      = kpEntry->tLastMod;
  lastAccess   = kpEntry->tLastAccess;
  binDesc      = kpEntry->pszBinaryDesc;
  binData      = kpEntry->pBinaryData;
  binDataLng   = kpEntry->uBinaryDataLen;

  LockEntryPassword(kpMgr, kpEntry);

  return *this;
  }
#endif

