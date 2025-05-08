// Remove Duplicate Entries


#pragma once
#include "KpSDK.h"


class Remove {

CPwManager* pwMgr;
int         index;
int         nEntries;
int         noDeleted;

public:

              Remove(CPwManager* pMgr) : pwMgr(pMgr) { }

  int         duplicates();
  int         oldLPImports();
  int         redundantGrps();
  int         backups();

private:

  void        findAllDups(int tgtIndex);
  int         findTitle(TCchar* title, int index)
                                              {return Find(pwMgr, title, true, PWMF_TITLE, index);}
  int         compare(int index, int dupIndex);
  bool        isOldImport(TCchar* extra);

  bool        isGrpPresent(DWORD id);

              Remove() : pwMgr(0) { }
  };

