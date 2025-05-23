// Iterator over the KeePass Data


#include "pch.h"
#include "KpIter.h"
#include "KpSDK.h"
#include "groups.h"


KpEntry* KpIter::operator() (Dir rev) {
  nEntry = GetNumberOfEntries(kpMgr);   bkupID = groups.bkupID();   i = rev ? nEntry-1 : 0;

  return next(rev);
  }


KpEntry* KpIter::operator++ (int) {i++; return next();}
KpEntry* KpIter::operator-- (int) {i--; return next();}


KpEntry* KpIter::next(Dir rev) {
int       delta = rev ? -1 : 1;
KpEntry* rcd;

  for (; -1 < i && i < nEntry; i += delta) {

    rcd = GetEntry(kpMgr, i);   if (!rcd) continue;       //(KpEntry*)

    if (rcd->uGroupId == bkupID) continue;

    return rcd;
    }

  return 0;
  }


bool KpIter::remove(Dir dir) {return DeleteEntry(kpMgr, i);}



