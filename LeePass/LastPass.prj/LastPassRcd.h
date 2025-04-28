// Last Pass Password entry data


#pragma once
#include "CSVLexF.h"
#include "CSVOutF.h"
#include "KpSDK.h"

class CSVOutF;


extern TCchar* FFLineOpen;
extern TCchar* FFLineClose;
extern TCchar* FavOpen;
extern TCchar* FavClose;
extern TCchar* ExtraOpen;
extern TCchar* ExtraClose;
extern TCchar* ToTpOpen;
extern TCchar* ToTpClose;
extern TCchar* TimeOpen;
extern TCchar* TimeClose;



extern TCchar* NoteType;
extern TCchar* AddrType;
extern TCchar* BankType;
extern TCchar* CCType;
extern TCchar* WiFiType;

extern TCchar* NotesURL;


                                         // database entry contains fields with these concepts
                                         // These are temporary fields used during import and
class LastPassRcd {                      // export.

CSVLexF lex;
String  errorField;

CSVOutF csv;

public:

// url,username,password,totp,extra,name,grouping,fav

String  url;
String  userName;
String  password;
String  totp;
String  extra;
String  name;
String  group;
String  fav;

  LastPassRcd() {}
 ~LastPassRcd();

  void   clear();

  bool   open(String& path);

  bool   readRecord();

  // Output KeePass records to a csv file to be imported by LastPass

  void   header(CSVOutF& csvOut);                       // Export KeePass Entries to LastPass
  void   writeRecord(KpEntry* kpEntry, CSVOutF& csv);

private:

  void   clr(String& s);

  bool   parse();
  void   addCR(TCchar* tc);                     // Add Carriage Returns to string
  void   parseAddr();
  void   parseBank();
  void   parseCC();
  void   parseWiFi();

  String extract(TCchar* beg, TCchar* end);     // Extract item between beg & end from extra.
  };


