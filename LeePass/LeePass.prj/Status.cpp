// A little class to manage status display at the bottom of the screen


#include "pch.h"
#include "Status.h"
#include "Groups.h"
#include "Record.h"




void Status::set(Record& rcd) {set(rcd.getEntryDsc());}


void Status::set(TCchar* txt) {
bool          isPresent = txt && *txt;
String        t         = isPresent ? txt : msg.str();

  if (isPresent) msg = t;

  if (!t.isEmpty()) t += _T(" -- ");

  t +=  saveRcd ? _T("Save Record Changes") : _T("Read Only");

  tmp(t);
  }





void Status::setDb(TCchar* path, int nRecords) {
String s = path;
String t;
int    nGrps = groups.nData();

  if (nGrps)    {s += t.format(_T("   No. of Groups: %i"),  nGrps);}
  if (nRecords) {s += t.format(_T("   No. of Entries: %i"), nRecords);}

  stsBar.setText(1, s);
  }

