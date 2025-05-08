// Remove Duplicate Entries


#include "pch.h"
#include "Groups.h"
#include "Remove.h"
#include "utility.h"

#include "MessageBox.h"


/*
KP_SHARE DWORD GetNumberOfEntries(void* pMgr);          // Returns number of entries in database
KP_SHARE PW_ENTRY* GetEntry(          void* pMgr, DWORD dwIndex);
*/

int Remove::duplicates() {

  noDeleted = 0;   nEntries = GetNumberOfEntries(pwMgr);

  for (index = 0; index < nEntries; index++) findAllDups(index);

  return noDeleted;
  }


void Remove::findAllDups(int tgtIndex) {
KpEntry* kpEntry = GetEntry(pwMgr, tgtIndex);    if (!kpEntry) return;
String   tgt     = kpEntry->pszTitle;
int      dupX;
int      delIndex;

  for (dupX = findTitle(tgt, tgtIndex+1); dupX >= 0; dupX = findTitle(tgt, ++dupX)) {

    delIndex = compare(tgtIndex, dupX);

    if (delIndex >= 0 && DeleteEntry(pwMgr, delIndex)) {

      noDeleted++;   nEntries--;    dupX--;

      if (delIndex == tgtIndex) {tgtIndex = dupX;   if (index >= 0) index--;}
      }
    }
  }


// returns index of entry to delete or -1

int Remove::compare(int index, int dupIndex) {
KpEntry* kpEntry  = GetEntry(pwMgr, index);     if (!kpEntry)  return -1;
KpEntry* dupEntry = GetEntry(pwMgr, dupIndex);  if (!dupEntry) return -1;
String   s;
Date     dt;
Date     dupDt;
bool     oldI;
bool     dupOldI;

  s = kpEntry->pszURL;         if (s != dupEntry->pszURL)      return -1;
  s = kpEntry->pszUserName;    if (s != dupEntry->pszUserName) return -1;

  dt   = getDate(kpEntry->tCreation);           dupDt   = getDate(dupEntry->tCreation);
  oldI = isOldImport(kpEntry->pszAdditional);   dupOldI = isOldImport(dupEntry->pszAdditional);

  if (oldI ^ dupOldI) return oldI ? index : dupIndex;

  return dt > dupDt ? dupIndex : index;
  }


int Remove::oldLPImports() {
KpEntry* kpEntry;
String   s;
String   title;

  noDeleted = 0;   nEntries = GetNumberOfEntries(pwMgr);

  for (index = 0; index < nEntries; index++) {

    kpEntry  = GetEntry(pwMgr, index);     if (!kpEntry) continue;

    if (isOldImport(kpEntry->pszAdditional))
                              {DeleteEntry(pwMgr, index);   noDeleted++;   nEntries--;    index--;}
    }

  return noDeleted;
  }




TCchar* FavOpen     = _T("<Fav~>");
TCchar* ExtraOpen   = _T("<Extra~>");
TCchar* ToTpOpen    = _T("<ToTP~>");
TCchar* TimeOpen    = _T("<Time~>");
TCchar* FFLineOpen  = _T("<FFLine~>");


bool Remove::isOldImport(TCchar* extra) {
String s = extra ? extra : _T("");

  if (s.find(FavOpen)   >= 0) return true;
  if (s.find(ExtraOpen) >= 0) return true;
  if (s.find(TimeOpen)  >= 0) return true;

  return false;
  }


int Remove::redundantGrps() {
GrpIter iter(groups);
Group*  grp;
String  s;
String  backup = _T("Backup");

  noDeleted = 0;

  for (grp = iter(); grp; grp = iter++) {

    if (!isGrpPresent(grp->id)) {groups.del(grp->id);   noDeleted++;}
    }
  return noDeleted;
  }


bool Remove::isGrpPresent(DWORD id) {
int      n = GetNumberOfEntries(pwMgr);
int      i;
KpEntry* kpEntry;

  for (i = 0; i < n; i++) {
    kpEntry  = GetEntry(pwMgr, i);     if (!kpEntry) continue;

    if (kpEntry->uGroupId == id) return true;
    }

  return false;
  }


int Remove::backups() {
KpEntry* kpEntry;
DWORD    backupId = groups.bkupID();

  noDeleted = 0;   nEntries = GetNumberOfEntries(pwMgr);

  for (index = 0; index < nEntries; index++) {
    kpEntry  = GetEntry(pwMgr, index);     if (!kpEntry) continue;

    if (kpEntry->uGroupId == backupId) {DeleteEntry(pwMgr, index);   noDeleted++;    index--;}
    }

  groups.del(backupId);    noDeleted++;   return noDeleted;
  }



/////--------------------

//      title += _T(" -- ") + s;   messageBox(title);

#if 0
    title.format(_T("%i: %s"), index, kpEntry->pszTitle);
    title += _T(", ");   title += kpEntry->pszURL;
    title += _T(", ");   title += kpEntry->pszUserName;
#endif

