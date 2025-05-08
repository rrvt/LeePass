// A little class to manage status display at the bottom of the screen


#pragma once
#include "StatusBar.h"

class Record;


class Status {

enum CurStatus {NilSts, RcdSts, NewSts, DBSts};

CurStatus  curSts;
StatusBar& stsBar;
bool&      saveRcd;
String     msg;

public:


  Status(StatusBar& statusBar, bool& saveRecord) :
                                         curSts(NilSts), stsBar(statusBar), saveRcd(saveRecord) { }
 ~Status() { }

  void setDb(TCchar* path, int nRecords);
  void set(Record& rcd);
  void set(TCchar* txt);
  void tmp(TCchar* txt) {try {stsBar.setText(0, txt);} catch (...) { }}
private:

  Status() : stsBar(*(StatusBar*)0), saveRcd(*(bool*)0) { }
  };

