// KeePass String Field


#pragma once
#include "Utility.h"


class KpStringFld {

String lbl;

public:
String fld;

  KpStringFld(TCchar* label) : lbl(label) { }

  void         clear()   {fld.clear();}
  void         expunge() {fld.expunge();   clear();}

  bool         isEmpty() {return fld.isEmpty();}

  int          length()  {return fld.length();}

  void         setLabel(CEdit& ctl) {ctl.SetWindowText(lbl);}
  void         clrLabel(CEdit& ctl) {clrLbl(ctl, lbl);}

  void         set(CEdit& ctl) {ctl.SetWindowText(fld);}
  bool         get(CEdit& ctl);

  KpStringFld& operator= (TCchar* tc) {fld = tc;   return *this;}

  operator TCchar*() const {return fld;}

private:

  KpStringFld() { }
  };

