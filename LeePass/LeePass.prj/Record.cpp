// Password LPentry data




#include "pch.h"
#include "Record.h"
#include "Groups.h"
#include "KpSDK.h"
#include "MessageBox.h"
#include "Utility.h"


static TCchar* TitleLbl    = _T("<Title>");
static TCchar* URLLbl      = _T("<URL>");
static TCchar* NameLbl     = _T("<Name>");
static TCchar* PasswordLbl = _T("<Password>");
static TCchar* NotesLbl    = _T("<Notes>");
static TCchar* GroupLbl    = _T("<Group>");
static TCchar* BinDescLbl  = _T("<Binary or Extra Desc>");

Record::Record() : pwMgr(0), kpEntry(0), aNote(false), group(GroupLbl), imageId(0),
                   title(TitleLbl), url(URLLbl), name(NameLbl), password(PasswordLbl),
                   notes(NotesLbl), binDesc(BinDescLbl), binData(0), binDataLng(0) {}

Record::~Record() {clear();}


void Record::clear() {
  aNote = false; imageId = 0; creation.clear();
     group.expunge();     url.expunge();      title.expunge();     name.expunge();
  password.expunge();   notes.expunge();    binDesc.expunge();
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

Record& Record::operator= (KpEntry* kpRcd) {        //

  clear();   if (!kpRcd) return *this;

  kpEntry = kpRcd;

  UnlockEntryPassword(pwMgr, kpEntry);

  group        = groups.getName(kpEntry->uGroupId);
  imageId      = kpEntry->uImageId;
  title        = kpEntry->pszTitle;
  url          = kpEntry->pszURL;
  name         = kpEntry->pszUserName;
  password     = kpEntry->pszPassword;
  notes        = kpEntry->pszAdditional;
  creation     = kpEntry->tCreation;
  expire       = kpEntry->tExpire;
  binDesc      = kpEntry->pszBinaryDesc;
  binData      = kpEntry->pBinaryData;
  binDataLng   = kpEntry->uBinaryDataLen;

  LockEntryPassword(pwMgr, kpEntry);

  return *this;
  }


bool Record::setTitle(     CEdit& ctl) {return    title.get(ctl);}
bool Record::setURL(       CEdit& ctl) {return      url.get(ctl);}
bool Record::setName(      CEdit& ctl) {return     name.get(ctl);}
bool Record::setPassword(  CEdit& ctl) {return password.get(ctl);}
bool Record::setNotes(     CEdit& ctl) {return    notes.get(ctl);}
bool Record::setBinaryDesc(CEdit& ctl) {return  binDesc.get(ctl);}


//PW_ENTRY* CreateEntry(void* pMgr, DWORD dwGroupID, LPCTSTR lpTitle,
//                         LPCTSTR lpUserName, LPCTSTR lpURL, LPCTSTR lpPassword, LPCTSTR lpNotes);
//BOOL      AddEntry(   void* pMgr, const PW_ENTRY* pTemplate);

bool Record::add() {
uint grpId = groups.getID(group);   if (!grpId) grpId = groups.add(group);

  kpEntry = CreateEntry(pwMgr, grpId, title, name, url, password, notes);

  if (!kpEntry) return false;

  return PE_SetBinaryDesc(kpEntry, binDesc);
  }


bool Record::updateTitle(CEdit& ctl) {
  if (!kpEntry || !title.get(ctl)) return false;

  PE_SetTitle(kpEntry, title);   return true;
  }


bool Record::updateURL(CEdit& ctl) {
  if (!kpEntry || !url.get(ctl)) return false;

  PE_SetURL(kpEntry, url);   return true;
  }


bool Record::updateName(CEdit& ctl) {
  if (!kpEntry || !name.get(ctl)) return false;

  PE_SetUserName(kpEntry, name);   return true;
  }


bool Record::updatePassword(CEdit& ctl) {
  if (!kpEntry || !password.get(ctl)) return false;

  PE_SetPasswordAndLock(pwMgr, kpEntry, password);   return true;
  }


bool Record::updateNotes(CEdit& ctl) {
  if (!kpEntry || !notes.get(ctl)) return false;

  PE_SetNotes(kpEntry, notes);   return true;
  }


bool Record::updateBinaryDesc(CEdit& ctl) {
  if (!kpEntry || !binDesc.get(ctl)) return false;

  PE_SetBinaryDesc(kpEntry, binDesc);   return true;
  }


String& Record::getEntryDsc() {
bool          urlEmpty = url.isEmpty();
static String dsc;

  dsc.clear();
  if (!title.isEmpty()) {dsc = title;   if (!urlEmpty) dsc += _T(", ");}
  if (!urlEmpty) dsc += url;

  return dsc;
  }



bool Record::setGroup( CComboBox& ctl) {
bool dirty = group.get(ctl);
uint id    = getGroupId(group);   if (!id) messageBox(_T("Group Not Available!"));

  return dirty;
  }


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

