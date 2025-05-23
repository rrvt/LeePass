// Iterator over the KeePass Data


#pragma once
#include "KpDataDef.h"


class KpIter {
int          nEntry;
int          i;
DWORD        bkupID;

public:

enum Dir {Fwd, Rev};

           KpIter() : nEntry(0), i(0), bkupID(0) { }
           KpIter(KpIter& iter)     : nEntry(iter.nEntry), i(iter.i), bkupID(iter.bkupID) { }
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
  };


//////----------------
//class CPwManager;

