// Remove Duplicate Entries


#include "pch.h"
#include "RemoveDups.h"
#include "utility.h"




/*
KP_SHARE DWORD GetNumberOfEntries(void* pMgr);          // Returns number of entries in database
KP_SHARE PW_ENTRY* GetEntry(          void* pMgr, DWORD dwIndex);
*/

int RemoveDups::operator() () {

  noDeleted = 0;   nEntries = GetNumberOfEntries(pwMgr);

  for (index = 0; index < nEntries; index++) findAllDups(index);

  return noDeleted;
  }


void RemoveDups::findAllDups(int tgtIndex) {
KpEntry* kpEntry = GetEntry(pwMgr, tgtIndex);    if (!kpEntry) return;
String   tgt     = kpEntry->pszTitle;
int      dupX;
int      delIndex;

  for (dupX = findTitle(tgt, tgtIndex+1); dupX >= 0; dupX = findTitle(tgt, ++dupX)) {

    delIndex = compare(tgtIndex, dupX);

    if (delIndex >= 0) {
      DeleteEntry(pwMgr, delIndex);   noDeleted++;   nEntries--;    dupX--;

      if (delIndex == tgtIndex) {tgtIndex = dupX;   if (index >= 0) index--;}
      }
    }
  }


// returns index of entry to delete or -1

int RemoveDups::compare(int index, int dupIndex) {
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

