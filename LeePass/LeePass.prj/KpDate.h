// KeePass Date/Time


#pragma once
#include "Date.h"
#include "KpSDK.h"
#include "Utility.h"



class KpDate {

Date   dt;
String lbl;

public:

  KpDate() { }
  KpDate(TCchar* label) : lbl(label) { }

  void     clear() {dt.clear();}

  PwTime&  today() {dt.getToday();   return getPwTime(dt);}

  KpDate&  operator= (PwTime& pwTime);

  void     setLabel(CStatic& ctl) {ctl.SetWindowText(lbl);}
  void     clrLabel(CStatic& ctl) {clrLbl(ctl, lbl);}

  void     set(CStatic& ctl) {String s = dt;   ctl.SetWindowText(s);}

  operator PwTime*() {return &getPwTime(dt);}
  };




//////////////------------------

//  Date&    toDate(PW_TIME& pwTime);
//  PW_TIME& toPTime(Date& dt);

