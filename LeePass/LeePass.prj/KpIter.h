// Iterator over the KeePass Data


#pragma once
#include "KpSDK.h"

class CPwManager;


class KpIter {
CPwManager* pwMgr;
int         nEntry;
int         i;
DWORD       bkupID;

public:

enum Dir {Fwd, Rev};

           KpIter(CPwManager* pMgr) : pwMgr(pMgr), nEntry(0), i(0), bkupID(0) { }
           KpIter(KpIter& iter)     : pwMgr(iter.pwMgr), nEntry(iter.nEntry),
                                                                 i(iter.i), bkupID(iter.bkupID) { }
          ~KpIter() { }

  KpEntry* operator() (Dir rev = Fwd);

  KpEntry* operator++ (int);
  KpEntry* operator-- (int);

  int      curIndx() {return i;}

  bool     isLast();
  bool     isFirst();

  bool     remove(Dir dir = Fwd);             // when true Deletes the current entry in database

private:

  KpEntry* next(Dir rev = Fwd);

  KpIter() : pwMgr(0) { }
  };

