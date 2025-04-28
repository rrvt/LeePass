// KeePass String Field


#include "pch.h"
#include "KpStringFld.h"


bool KpStringFld::get(CEdit& ctl) {
Cstring cs;
bool    dirty;

  ctl.GetWindowText(cs);   if (lbl == cs) return false;

  dirty |= fld != cs;   fld = cs;    return dirty;
  }


bool KpStringFld::isEmpty(CEdit& ctl) {
Cstring cs;

  ctl.GetWindowText(cs);   if (cs.isEmpty() || lbl == cs) return false;

  return true;
  }



bool KpStringFld::isProhibited(CEdit& ctl)
                                {Cstring cs;   ctl.GetWindowText(cs);   return ::isProhibited(cs);}

