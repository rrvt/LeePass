// Record Field Lengths


#pragma once

class Record;


class KpFldLngs {
public:

int groupNameMax;
int titleMax;
int urlMax;
int userNameMax;
int passwordMax;
int notesMax;
int binaryDescMax;

  KpFldLngs() : groupNameMax(0), titleMax(0), urlMax(0), userNameMax(0), passwordMax(0),
                                                                  notesMax(0), binaryDescMax(0) { }

  void get(Record& rcd);

private:

  void getMax(int& max, int nextLng) {if (nextLng > max) max = nextLng;}
  };

