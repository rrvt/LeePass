// KeePass ComboBox Field


#include "pch.h"
#include "KpCBxFld.h"


bool KpCBxFld::get(CComboBox& ctl) {
Cstring cs;
bool    dirty;

  ctl.GetWindowText(cs);   if (lbl == cs) return false;

  dirty |= fld != cs;   fld = cs;   return dirty;
  }


bool KpCBxFld::isProhibited(CComboBox& ctl)
                                {Cstring cs;   ctl.GetWindowText(cs);   return ::isProhibited(cs);}



