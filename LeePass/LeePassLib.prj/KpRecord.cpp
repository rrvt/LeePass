// KeePass Record Interface


#include "framework.h"
#include "KpRecord.h"
#include "Groups.h"
#include "KpSDK.h"
#include "MessageBox.h"


KpRecord::KpRecord() : kpEntry(0), imageId(0) {}


void KpRecord::clear() {
  kpEntry = 0;        kpId.clear();      group.expunge();      imageId = 0;
  title.expunge();    url.expunge();     userName.expunge();   password.expunge();
  extra.expunge();    creation.clear();  lastMod.clear();      lastAccess.clear();
  binDesc.expunge();  binData.clear();
  }


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

KpRecord& KpRecord::operator= (KpEntry* kEntry) {        //

  clear();   if (!kEntry) return *this;

  kpEntry = kEntry;

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
  binData.get(kpEntry->pBinaryData, kpEntry->uBinaryDataLen);

  LockEntryPassword(kpMgr, kpEntry);

  return *this;
  }



//PW_ENTRY* CreateEntry(void* pMgr, DWORD dwGroupID, LPCTSTR lpTitle,
//                         LPCTSTR lpUserName, LPCTSTR lpURL, LPCTSTR lpPassword, LPCTSTR lpNotes);
//BOOL      AddEntry(   void* pMgr, const PW_ENTRY* pTemplate);

bool KpRecord::add() {
uint grpId;

  if (title.isEmpty()) {
    if (!url.isEmpty())                          title += url;
    if (!title.isEmpty() && !userName.isEmpty()) title += _T(", ");
    if (!userName.isEmpty())                     title += userName;
    }

  if (title.isEmpty()) {messageBox(_T("No Title, Entry NOT SAVED"));  return false;}

  if (group.isEmpty()) group = GeneralGrp;

  grpId = groups.getID(group);   if (!grpId) grpId = groups.add(group);   if (!grpId) return false;

  try {kpEntry = CreateEntry(kpMgr, grpId, title, userName, url, password, extra);}
  catch (...) {kpEntry = 0;}

  if (!kpEntry) return false;

  kpId         = kpEntry->uuid;
  creation     = kpEntry->tCreation;
  lastMod      = kpEntry->tLastMod;
  lastAccess   = kpEntry->tLastAccess;

  return PE_SetBinaryDesc(kpEntry, binDesc);
  }


bool KpRecord::update(KpEntry* kpEntry) {
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


uint KpRecord::getGroupId(TCchar* name) {
uint   id = groups.getID(name);   if (id) return id;
String q  = _T("Add "); q += (TCchar*) name;  q += _T(" to Groups?");

  return msgYesNoBox(q) == IDYES ? groups.add(group) : 0;
  }


String& KpRecord::getEntryDsc() {
static String dsc;

  dsc.clear();                                                      dsc  = title;
  if (!url.isEmpty())      {if (!dsc.isEmpty()) dsc += _T(", ");    dsc += url;}

  return dsc;
  }


String& KpRecord::getLongEntryDsc() {
static String dsc;

  dsc.clear();                                                      dsc  = title;
  if (!url.isEmpty())      {if (!dsc.isEmpty()) dsc += _T(", ");    dsc += url;}
  if (!userName.isEmpty()) {if (!dsc.isEmpty()) dsc += _T(", ");    dsc += userName;}
  if (!group.isEmpty())    {if (!dsc.isEmpty()) dsc += _T(", ");    dsc += group;}

  return dsc;
  }




//KP_SHARE DWORD Find(void* pMgr, const TCHAR* pszFindString,
//                                              BOOL bCaseSensitive, DWORD fieldFlags, DWORD nStart);

bool KpRecord::find(KpEntry*& kpEntry) {
int index;

  for (index = findTitle(title, 0); index >= 0; index = findTitle(title, index+1)) {
    kpEntry = GetEntry(kpMgr, index);    if (!kpEntry) continue;

    if (url == _T("http://sn")) {
      if (userName == kpEntry->pszUserName && extra == kpEntry->pszAdditional) return true;
      continue;
      }

    if (url == kpEntry->pszURL && userName == kpEntry->pszUserName) return true;
    }

  return false;
  }


int KpRecord::findTitle(TCchar* title, int i) {return Find(kpMgr, title, true, PWMF_TITLE, i);}

