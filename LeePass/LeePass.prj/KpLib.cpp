// Interface to KpLib Dll

/*
// Interface to KeePass Library DLL

KP_SHARE void  InitManager(void** pMgr, BOOL bIsFirstInstance);
KP_SHARE void  DeleteManager(void* pMgr);

KP_SHARE INT   SetMasterKey(void* pMgr, LPCTSTR pszMasterKey, BOOL bDiskDrive,
                            LPCTSTR pszSecondKey, const CNewRandomInterface *pARI,
                            BOOL bOverwrite);

KP_SHARE void  NewDatabase( void* pMgr);
KP_SHARE int   OpenDatabase(void* pMgr, const TCHAR* pszFile, PWDB_REPAIR_INFO* pRepair);
KP_SHARE int   SaveDatabase(void* pMgr, const TCHAR* pszFile);

KP_SHARE DWORD GetNumberOfEntries(void* pMgr);          // Returns number of entries in database
KP_SHARE DWORD GetNumberOfGroups( void* pMgr);          // Returns number of groups in database

// Count items in groups

KP_SHARE DWORD GetNumberOfItemsInGroup( void* pMgr, LPCTSTR pszGroup);
KP_SHARE DWORD GetNumberOfItemsInGroupN(void* pMgr, DWORD   idGroup);

// Access entry information

KP_SHARE PW_ENTRY* GetEntry(          void* pMgr, DWORD dwIndex);
KP_SHARE BOOL      GetEntryStruct(    void* pMgr, DWORD dwIndex, PW_ENTRY* pe);
KP_SHARE PW_ENTRY* GetEntryByGroup(   void* pMgr, DWORD idGroup, DWORD dwIndex);
KP_SHARE DWORD     GetEntryByGroupN(  void* pMgr, DWORD idGroup, DWORD dwIndex);
KP_SHARE PW_ENTRY* GetEntryByUuid(    void* pMgr, const BYTE* pUuid);
KP_SHARE DWORD     GetEntryByUuidN(   void* pMgr, const BYTE* pUuid);
                                                       // Returns the index of the item with pUuid
KP_SHARE DWORD     GetEntryPosInGroup(void* pMgr, const PW_ENTRY* pEntry);
KP_SHARE PW_ENTRY* GetLastEditedEntry(void* pMgr);

// Access group information

KP_SHARE PW_GROUP* GetGroup(         void* pMgr, DWORD dwIndex);
KP_SHARE BOOL      GetGroupStruct(   void* pMgr, DWORD dwIndex, PW_GROUP* pGroup);
KP_SHARE PW_GROUP* GetGroupById(     void* pMgr, DWORD idGroup);
KP_SHARE DWORD     GetGroupByIdN(    void* pMgr, DWORD idGroup);
KP_SHARE DWORD     GetGroupId(       void* pMgr, const TCHAR* pszGroupName);
KP_SHARE DWORD     GetGroupIdByIndex(void* pMgr, DWORD uGroupIndex);
KP_SHARE DWORD     GetLastChildGroup(void* pMgr, DWORD dwParentGroupIndex);
KP_SHARE BOOL      GetGroupTree(     void* pMgr, DWORD idGroup, DWORD* pGroupIndexes);

// Add entries and groups

KP_SHARE BOOL      AddGroup(         void* pMgr, const PW_GROUP* pTemplate);
KP_SHARE BOOL      AddEntry(         void* pMgr, const PW_ENTRY* pTemplate);
KP_SHARE BOOL      BackupEntry(      void* pMgr, const PW_ENTRY* pe, BOOL* pbGroupCreated);
                                         // pe must be unlocked already, pbGroupCreated is optional

// Delete entries and groups

KP_SHARE BOOL      DeleteEntry(      void* pMgr, DWORD dwIndex);
KP_SHARE BOOL      DeleteGroupById(  void* pMgr, DWORD uGroupId);

KP_SHARE BOOL      SetGroup(         void* pMgr, DWORD dwIndex, const PW_GROUP* pTemplate);
KP_SHARE BOOL      SetEntry(         void* pMgr, DWORD dwIndex, const PW_ENTRY* pTemplate);

// Use these functions to make passwords in PW_ENTRY structures readable

KP_SHARE void    LockEntryPassword(  void* pMgr, PW_ENTRY* pEntry);   // Lock password, encrypt it
KP_SHARE void    UnlockEntryPassword(void* pMgr, PW_ENTRY* pEntry);   // Make password readable

// Move entries and groups

KP_SHARE void      MoveEntry(        void* pMgr, DWORD idGroup, DWORD dwFrom, DWORD dwTo);
KP_SHARE BOOL      MoveGroup(        void* pMgr, DWORD dwFrom,  DWORD dwTo);

// Sort entry and group lists

KP_SHARE void      SortGroup(        void* pMgr, DWORD idGroup, DWORD dwSortByField);
KP_SHARE void      SortGroupList(    void* pMgr);

KP_SHARE BOOL MemAllocCopyEntry(const PW_ENTRY* pExisting, PW_ENTRY* pDestination);
KP_SHARE void MemFreeEntry(           PW_ENTRY* pEntry);

KP_SHARE void MergeIn(void* pMgr, VPA_MODIFY CPwManager* pDataSource, BOOL bCreateNewUUIDs,
                                                                              BOOL bCompareTimes);

// Find an item

KP_SHARE DWORD Find(void* pMgr, const TCHAR* pszFindString,
                                              BOOL bCaseSensitive, DWORD fieldFlags, DWORD nStart);

// Get and set the algorithm used to encrypt the database

KP_SHARE BOOL SetAlgorithm(void* pMgr, int nAlgorithm);
KP_SHARE int  GetAlgorithm(void* pMgr);

KP_SHARE DWORD GetKeyEncRounds(void* pMgr);
KP_SHARE void  SetKeyEncRounds(void* pMgr, DWORD dwRounds);

// Convert PW_TIME to 5-byte compressed structure and the other way round

KP_SHARE void TimeToPwTime(const BYTE*    pCompressedTime, PW_TIME* pPwTime);
KP_SHARE void PwTimeToTime(const PW_TIME* pPwTime,         BYTE*    pCompressedTime);

// Get the never-expire time

KP_SHARE void GetNeverExpireTime(PW_TIME* pPwTime);

// Checks and corrects the group tree (level order, etc.)

KP_SHARE void FixGroupTree(      void* pMgr);

KP_SHARE void SubstEntryGroupIds(void* pMgr, DWORD dwExistingId, DWORD dwNewId);

KP_SHARE BOOL AttachFileAsBinaryData(PW_ENTRY* pEntry, const TCHAR* lpFile);
KP_SHARE BOOL SaveBinaryData(  const PW_ENTRY* pEntry, const TCHAR* lpFile);
KP_SHARE BOOL RemoveBinaryData(      PW_ENTRY* pEntry);

KP_SHARE BOOL IsAllowedStoreGroup(void* pMgr, LPCTSTR lpGroupName, LPCTSTR lpSearchGroupName);

KP_SHARE void GetRawMasterKey(void* pMgr,       BYTE* pStorage);
KP_SHARE void SetRawMasterKey(void* pMgr, const BYTE* pNewKey);

KP_SHARE BOOL IsZeroUUID(const BYTE* pUUID);

KP_SHARE PW_GROUP* CreateGroup(void* pMgr, LPCTSTR lpName, DWORD dwImageID);
KP_SHARE PW_ENTRY* CreateEntry(void* pMgr, DWORD dwGroupID, LPCTSTR lpTitle,
                           LPCTSTR lpUserName, LPCTSTR lpURL, LPCTSTR lpPassword, LPCTSTR lpNotes);

// Extract group information

KP_SHARE DWORD          PG_GetID(            PW_GROUP* pGroup);
KP_SHARE DWORD          PG_GetImageID(       PW_GROUP* pGroup);
KP_SHARE LPCTSTR        PG_GetName(          PW_GROUP* pGroup);
KP_SHARE const PW_TIME* PG_GetCreationTime(  PW_GROUP* pGroup);
KP_SHARE const PW_TIME* PG_GetLastModTime(   PW_GROUP* pGroup);
KP_SHARE const PW_TIME* PG_GetLastAccessTime(PW_GROUP* pGroup);
KP_SHARE const PW_TIME* PG_GetExpireTime(    PW_GROUP* pGroup);
KP_SHARE USHORT         PG_GetLevel(         PW_GROUP* pGroup);

// Set group information

KP_SHARE BOOL           PG_SetID(            PW_GROUP* pGroup, DWORD          dwID);
KP_SHARE BOOL           PG_SetImageID(       PW_GROUP* pGroup, DWORD          dwImageID);
KP_SHARE BOOL           PG_SetName(          PW_GROUP* pGroup, LPCTSTR        lpName);
KP_SHARE BOOL           PG_SetCreationTime(  PW_GROUP* pGroup, const PW_TIME* pTime);
KP_SHARE BOOL           PG_SetLastModTime(   PW_GROUP* pGroup, const PW_TIME* pTime);
KP_SHARE BOOL           PG_SetLastAccessTime(PW_GROUP* pGroup, const PW_TIME* pTime);
KP_SHARE BOOL           PG_SetExpireTime(    PW_GROUP* pGroup, const PW_TIME* pTime);
KP_SHARE BOOL           PG_SetLevel(         PW_GROUP* pGroup, USHORT         usLevel);

// Extract entry information

KP_SHARE const BYTE*    PE_GetUUID(            PW_ENTRY* pEntry);
KP_SHARE DWORD          PE_GetGroupID(         PW_ENTRY* pEntry);
KP_SHARE DWORD          PE_GetImageID(         PW_ENTRY* pEntry);
KP_SHARE LPCTSTR        PE_GetTitle(           PW_ENTRY* pEntry);
KP_SHARE LPCTSTR        PE_GetURL(             PW_ENTRY* pEntry);
KP_SHARE LPCTSTR        PE_GetUserName(        PW_ENTRY* pEntry);
KP_SHARE LPCTSTR        PE_GetPasswordPtr(     PW_ENTRY* pEntry);
KP_SHARE LPCTSTR        PE_GetNotes(           PW_ENTRY* pEntry);
KP_SHARE const PW_TIME* PE_GetCreationTime(    PW_ENTRY* pEntry);
KP_SHARE const PW_TIME* PE_GetLastModTime(     PW_ENTRY* pEntry);
KP_SHARE const PW_TIME* PE_GetLastAccessTime(  PW_ENTRY* pEntry);
KP_SHARE const PW_TIME* PE_GetExpireTime(      PW_ENTRY* pEntry);
KP_SHARE LPCTSTR        PE_GetBinaryDesc(      PW_ENTRY* pEntry);
KP_SHARE const BYTE*    PE_GetBinaryData(      PW_ENTRY* pEntry);
KP_SHARE DWORD          PE_GetBinaryDataLength(PW_ENTRY* pEntry);

// Set entry information

KP_SHARE BOOL PE_SetUUID(                       PW_ENTRY* pEntry, const BYTE*    pUUID);
KP_SHARE BOOL PE_SetGroupID(                    PW_ENTRY* pEntry, DWORD          dwGroupID);
KP_SHARE BOOL PE_SetImageID(                    PW_ENTRY* pEntry, DWORD          dwImageID);
KP_SHARE BOOL PE_SetTitle(                      PW_ENTRY* pEntry, LPCTSTR        lpTitle);
KP_SHARE BOOL PE_SetURL(                        PW_ENTRY* pEntry, LPCTSTR        lpURL);
KP_SHARE BOOL PE_SetUserName(                   PW_ENTRY* pEntry, LPCTSTR        lpUserName);
KP_SHARE BOOL PE_SetPasswordAndLock(void* pMgr, PW_ENTRY* pEntry, LPCTSTR        lpPassword);
KP_SHARE BOOL PE_SetNotes(                      PW_ENTRY* pEntry, LPCTSTR        lpNotes);
KP_SHARE BOOL PE_SetCreationTime(               PW_ENTRY* pEntry, const PW_TIME* pTime);
KP_SHARE BOOL PE_SetLastModTime(                PW_ENTRY* pEntry, const PW_TIME* pTime);
KP_SHARE BOOL PE_SetLastAccessTime(             PW_ENTRY* pEntry, const PW_TIME* pTime);
KP_SHARE BOOL PE_SetExpireTime(                 PW_ENTRY* pEntry, const PW_TIME* pTime);
KP_SHARE BOOL PE_SetBinaryDesc(                 PW_ENTRY* pEntry, LPCTSTR        lpDesc);
KP_SHARE BOOL PE_SetBinaryData(                 PW_ENTRY* pEntry, const BYTE*    lpData,
                                                                  DWORD          dwDataLength);
KP_SHARE DWORD   GetKeePassVersion();
KP_SHARE LPCTSTR GetKeePassVersionString();

KP_SHARE DWORD   GetLibraryBuild();

KP_SHARE BOOL   TransformKey256(UINT8* pBuffer256, const UINT8* pKeySeed256, UINT64 qwRounds);
KP_SHARE UINT64 TransformKeyBenchmark256(DWORD dwTimeMs);

KP_SHARE void   ProtectProcessWithDacl();
*/





#include "pch.h"
#include "KpLib.h"
#include "ChngMstrKeyDlg.h"
#include "FileName.h"
#include "GetPathDlg.h"
#include "Groups.h"
#include "KpFldLngs.h"
#include "KpIter.h"
#include "LastPassRcd.h"
#include "LibraryAPI.h"
#include "MessageBox.h"
#include "Record.h"
#include "SearchDlg.h"
#include "Utility.h"
#include "VerifyDlg.h"




CPwManager* KpLib::newDatabase(String& path, Cstring& password) {
PathDlgDsc pathDlgDsc(_T("LeePass Database"), path, _T("kdb"), _T("*.kdb"));

  NewDatabase(pwMgr);

  if (!setPassword(password)) return 0;

  if (!path.isEmpty()) path = getPath(path);

  if (getSaveAsPathDlg(pathDlgDsc, path)) SaveDatabase(pwMgr, path);

  groups.setPwMgr(pwMgr);   rcd.setPwMgr(pwMgr);   return pwMgr;
  }


CPwManager* KpLib::openDatabase(TCchar* path, Cstring& password) {
PWDB_REPAIR_INFO info;   ZeroMemory(&info, sizeof(PWDB_REPAIR_INFO));
bool             rslt = true;

  try {

    if (!setPassword(password)) return 0;

    rslt = chk(OpenDatabase(pwMgr, path, &info));
    } catch (...) {rslt = false;}

    if (!rslt) {password.expunge();   return 0;}

  groups.setPwMgr(pwMgr);   groups.initialize();   rcd.setPwMgr(pwMgr);   kpSrch.setPwMgr(pwMgr);

  return pwMgr;
  }


bool KpLib::setPassword(Cstring& password) {
bool rslt = chk(SetMasterKey(pwMgr, password, false, 0, 0, false));

  if (!rslt) password.expunge();   return rslt;
  }


bool KpLib::changeMasterKey() {
ChngMstrKeyDlg dlg;
bool           rslt = true;

  rslt &= dlg.DoModal() == IDOK;

  rslt &= isMasterKey(dlg.curPswd);

  rslt &= dlg.newPswd == dlg.confirmPswd;

  if (rslt) {

    rslt = chk(SetMasterKey(pwMgr, dlg.newPswd, false, 0, 0, true));

    if (rslt) saveMasterKey(dlg.newPswd);
    else      dlg.newPswd.expunge();
    }

  dlg.confirmPswd.expunge();   return rslt;
  }



void KpLib::saveMasterKey(Cstring& masterKey) {
KpEntry* kpEntry = kpSrch.findMasterKey();
uint     grpId   = groups.getID(MasterKey);   if (!grpId) grpId = groups.add(MasterKey);
bool     rslt;

  if (kpEntry) {
    UnlockEntryPassword(pwMgr, kpEntry);
      rslt = masterKey != kpEntry->pszPassword;
    LockEntryPassword(pwMgr, kpEntry);

    if (rslt) {PE_SetPasswordAndLock(pwMgr, kpEntry, masterKey);}
    }

  else CreateEntry(pwMgr, grpId, MasterKey, MasterKey, 0, masterKey, 0);

  masterKey.expunge();
  }


bool KpLib::verifyMasterKey() {
VerifyDlg dlg;
bool      rslt;

  rslt = dlg.DoModal() == IDOK;

  if (rslt) rslt &= isMasterKey(dlg.curPswd);
  else      dlg.curPswd.expunge();

  return rslt;
  }


bool KpLib::isMasterKey(Cstring& tgt) {
KpEntry* kpEntry = kpSrch.findMasterKey();   if (!kpEntry) return false;
bool     rslt;

  UnlockEntryPassword(pwMgr, kpEntry);
    rslt = tgt == kpEntry->pszPassword;
  LockEntryPassword(pwMgr, kpEntry);

  tgt.expunge();   return rslt;
  }


static TCchar* algoTxt[] {_T("ALGO_AES"), _T("ALGO_TWOFISH")};

void KpLib::dspEncryption() {
int    algo;
uint    n;
String s;

  if (!pwMgr) return;

  algo = GetAlgorithm(pwMgr);     n = GetKeyEncRounds(pwMgr);

  s.format(_T("Algorithm: %s, Rounds: %u"), algoTxt[algo], n);

  messageBox(s);
  }


String KpLib::getVersion() {
String kpVer  = GetKeePassVersionString();
uint   libBld = GetLibraryBuild();
String s;

  s.format(_T("KeePass Version: %s, Library Build: %u"), kpVer.str(), libBld);   return s;
  }


void KpLib::saveDatabase(TCchar* path) {if (pwMgr) SaveDatabase(pwMgr, path);}




bool KpLib::importFile(String& path) {
LastPassRcd lpRcd;
int         n;

  if (!lpRcd.open(path)) return false;

  lpRcd.readRecord();

  for (n = 0; lpRcd.readRecord(); n++) {
    store(lpRcd);
    }

  return n > 0;
  }


bool KpLib::store(LastPassRcd& lpRcd) {
bool     rslt;
KpEntry* kpEntry;

  if (isProhibited(lpRcd.title) || isProhibited(lpRcd.userName) || isProhibited(lpRcd.group)) {
    String s = lpRcd.url + Comma + lpRcd.userName + Comma + lpRcd.title + Comma + lpRcd.group;
    s += _T(" prohibited");   messageBox(s);   return false;
    }

  rcd.clear();

  rcd.group      = lpRcd.group;
  rcd.title      = lpRcd.title;
  rcd.url        = lpRcd.url;
  rcd.userName   = lpRcd.userName;
  rcd.password   = lpRcd.password;
  rcd.extra      = lpRcd.extra;
  rcd.binDesc    = lpRcd.totp + _T(',') + lpRcd.fav;

  if (rcd.find(kpEntry)) {rslt = rcd.update(kpEntry);   rcd.clear();   return rslt;}

  rslt = rcd.add();   rcd.clear();   return rslt;
  }


// url,username,password,totp,extra,name,grouping,fav

void KpLib::exportFile() {
PathDlgDsc  dsc;
CSVOutF     csv;
KpIter      iter(pwMgr);
KpEntry*    kpEntry;
LastPassRcd lpRcd;
#if 0
int         n;
bool        passwordSent = false;
bool        addrNoteSent = false;
bool        bankNoteSent = false;
bool        ccNoteSent   = false;
bool        wifiNoteSent = false;
#endif

  if (!verifyMasterKey()) return;

  dsc(_T("Export LandPass File"), _T("foo"), _T("csv"), _T("*.csv"));

  if (!csv.open(dsc)) return;

  lpRcd.header(csv);

  for (kpEntry = iter(); kpEntry; kpEntry = iter++) writeRecord(kpEntry, csv);
  }


void KpLib::writeRecord(KpEntry* kpEntry, CSVOutF& csv) {
LastPassRcd lpRcd;
  UnlockEntryPassword(pwMgr, kpEntry);
    lpRcd.writeRecord(kpEntry, csv);
  LockEntryPassword(pwMgr, kpEntry);
  }


static TCchar* ErrorCodes[] = {_T("UNKNOWN"),                _T("SUCCESS"),
                               _T("INVALID_PARAM"),          _T("NO_MEM"),
                               _T("INVALID_KEY"),            _T("NOFILEACCESS_READ"),
                               _T("NOFILEACCESS_WRITE"),     _T("FILEERROR_READ"),
                               _T("FILEERROR_WRITE"),        _T("INVALID_RANDOMSOURCE"),
                               _T("INVALID_FILESTRUCTURE"),  _T("CRYPT_ERROR"),
                               _T("INVALID_FILESIZE"),       _T("INVALID_FILESIGNATURE"),
                               _T("INVALID_FILEHEADER"),     _T("NOFILEACCESS_READ_KEY"),
                               _T("KEYPROV_INVALID_KEY"),    _T("FILEERROR_VERIFY"),
                               _T("UNSUPPORTED_KDBX"),       _T("GETLASTERROR"),
                               _T("DB_EMPTY"),               _T("ATTACH_TOOLARGE")
                               };


bool KpLib::chk(int err) {
String s;

  if (err == PWE_SUCCESS) return true;

#ifdef _DEBUG

  s = 0 <= err && err < noElements(ErrorCodes) ? ErrorCodes[err] :
                                                  s.format(_T("Err: %i"), err).str();
  messageBox(s);
#endif

  return false;
  }





///////--------------------

#if 0
void KpLib::addEntry() {
String  sampleBlock;
Date    today;      today.getToday();


  rcd.add();
  }
#endif
#if 0
/*
KP_SHARE DWORD GetNumberOfEntries(void* pMgr);          // Returns number of entries in database
KP_SHARE PW_ENTRY* GetEntry(          void* pMgr, DWORD dwIndex);
*/

void KpLib::removeDups() {
int nEntries = GetNumberOfEntries(pwMgr);
int index;

  for (index = 0; index < nEntries; index++) findAllDups(index);
  }


void KpLib::findAllDups(int tgtIndex) {
KpEntry* kpEntry = GetEntry(pwMgr, tgtIndex);    if (!kpEntry) return;
String   tgt     = kpEntry->pszTitle;
int      dupIndex;
int      delIndex;

  for (dupIndex = findTitle(tgt, tgtIndex+1); dupIndex >= 0; dupIndex = findTitle(tgt, dupIndex)) {

    delIndex = compare(tgtIndex, dupIndex);

    if (delIndex >= 0)
                 {DeleteEntry(pwMgr, delIndex);   if (delIndex == tgtIndex) tgtIndex = dupIndex-1;}
    }
  }


// returns index of entry to delete or -1

int KpLib::compare(int index, int dupIndex) {
KpEntry* kpEntry  = GetEntry(pwMgr, index);     if (!kpEntry)  return -1;
KpEntry* dupEntry = GetEntry(pwMgr, dupIndex);  if (!dupEntry) return -1;
String   s;
Date     dt;
Date     dupDt;

  s = kpEntry->pszURL;         if (s != dupEntry->pszURL)      return -1;
  s = kpEntry->pszUserName;    if (s != dupEntry->pszUserName) return -1;

  dt = getDate(kpEntry->tCreation);   dupDt = getDate(dupEntry->tCreation);

  return dt > dupDt ? dupIndex : index;
  }
#endif
#if 1
#else
    String extra = kpEntry->pszAdditional ? kpEntry->pszAdditional : _T("");

    if (!passwordSent) {
      n = 0;
      if (!isEmpty(kpEntry->pszURL))      ++n;
      if (!isEmpty(kpEntry->pszUserName)) ++n;
      if (!isEmpty(kpEntry->pszTitle))    ++n;
      if (n >= 3) {writeRecord(kpEntry, csv);   passwordSent = true;   continue;}
      }

    if (!addrNoteSent && extra.find(AddrType) >= 0)
                                    {writeRecord(kpEntry, csv);   addrNoteSent = true;   continue;}

    if (!bankNoteSent && extra.find(BankType) >= 0)
                                    {writeRecord(kpEntry, csv);   bankNoteSent = true;   continue;}


    if (!ccNoteSent && extra.find(CCType) >= 0)
                                    {writeRecord(kpEntry, csv);   ccNoteSent = true;   continue;}


    if (!wifiNoteSent && extra.find(WiFiType) >= 0)
                                    {writeRecord(kpEntry, csv);   wifiNoteSent = true;   continue;}
#endif

