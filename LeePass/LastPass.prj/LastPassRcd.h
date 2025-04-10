// Last Pass Password entry data


#pragma once
#include "CSVLexF.h"



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



extern TCchar* NotesURL;


                                         // database entry contains fields with these concepts
                                         // These are temporary fields used during import and
class LastPassRcd {                      // export.
public:

// url,username,password,totp,extra,name,grouping,fav

String  url;
String  userName;
String  password;
String  totp;
String  extra;
String  name;
String  grouping;
String  fav;

String  desc;               // Describes the extra flavor: Address, Bank Account, Credit Card,
                            // Wi-Fi Password, Raw Note

CSVLexF lex;
String  errorField;

  LastPassRcd() {}
 ~LastPassRcd();

  void   clear();

  bool   open(String& path);

  bool   readRecord();

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


