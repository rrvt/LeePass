// Remove Duplicate Entries


#pragma once
#include "KpSDK.h"


class RemoveDups {

CPwManager* pwMgr;
int         index;
int         nEntries;
int         noDeleted;

public:

              RemoveDups(CPwManager* pMgr) : pwMgr(pMgr) { }

  int         operator() ();

private:

  void        findAllDups(int tgtIndex);
  int         findTitle(TCchar* title, int index)
                                              {return Find(pwMgr, title, true, PWMF_TITLE, index);}
  int         compare(int index, int dupIndex);

              RemoveDups() : pwMgr(0) { }
  };

