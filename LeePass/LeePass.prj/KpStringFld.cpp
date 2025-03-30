// KeePass String Field


#include "pch.h"
#include "KpStringFld.h"


bool KpStringFld::get(CEdit& ctl) {
Cstring cs;
bool    dirty;

  ctl.GetWindowText(cs);   if (lbl == cs) return false;

  dirty |= fld != cs;   fld = cs;    return dirty;
  }




