// Record Field Lengths


#include "pch.h"
#include "KpFldLngs.h"
#include "Record.h"


void KpFldLngs::get(Record& rcd) {
  getMax(groupNameMax,  rcd.group.length());
  getMax(titleMax,      rcd.title.length());
  getMax(urlMax,        rcd.url.length());
  getMax(userNameMax,   rcd.name.length());
  getMax(passwordMax,   rcd.password.length());
  getMax(notesMax,      rcd.notes.length());
  getMax(binaryDescMax, rcd.binDesc.length());
  }
