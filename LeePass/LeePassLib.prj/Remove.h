// Remove Duplicate Entries


#pragma once
//#include "KpSDK.h"


class Remove {

int index;
int nEntries;
int noDeleted;

public:

       Remove() { }

  int  duplicates();

private:

  void findAllDups(int tgtIndex);
  int  findTitle(TCchar* title, int index);
  int  compare(int index, int dupIndex);
  };



//////-------------------
#if 0
  int         oldLPImports();
  int         redundantGrps();
  int         backups();
#endif
#if 0
  bool        isOldImport(TCchar* extra);
  bool        isGrpPresent(DWORD id);
#endif
//#include "KpSDK.h"

