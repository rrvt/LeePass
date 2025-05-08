// Record Field Lengths


#include "pch.h"
#include "KpFldLngs.h"
#include "Record.h"


void KpFldLngs::get(Record& rcd) {
  getMax(groupNameMax,  rcd.group.length());
  getMax(titleMax,      rcd.title.length());
  getMax(urlMax,        rcd.url.length());
  getMax(userNameMax,   rcd.userName.length());
  getMax(passwordMax,   rcd.password.length());
  getMax(extraMax,      rcd.extra.length());
  getMax(binaryDescMax, rcd.binDesc.length());
  }
