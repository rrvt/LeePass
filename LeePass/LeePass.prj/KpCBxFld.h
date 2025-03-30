// KeePass ComboBox Field


#pragma once
#include "Groups.h"
#include "Utility.h"


class KpCBxFld {

String lbl;

public:
String fld;

  KpCBxFld(TCchar* label) : lbl(label) { }

  void         clear()   {fld.clear();}
  void         expunge() {fld.expunge();   clear();}

  void         setLabel(CComboBox& ctl) {groups.install(ctl, _T("")); ctl.SetWindowText(lbl);}
  void         clrLabel(CComboBox& ctl) {clrLbl(ctl, lbl);}

  void         set(CComboBox& ctl)      {groups.install(ctl, fld);}
  bool         get(CComboBox& ctl);

  bool         isEmpty() {return fld.isEmpty();}

  int          length()  {return fld.length();}

  KpCBxFld& operator= (TCchar* tc) {fld = tc;   return *this;}
            operator TCchar*() const {return fld;}

private:

  KpCBxFld() { }
  };

