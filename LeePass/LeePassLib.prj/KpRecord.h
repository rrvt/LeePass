// KeePass Record Interface


#pragma once
#include "KpBinData.h"
#include "KpDate.h"
#include "KpID.h"


class KpRecord {

protected:

KpEntry*  kpEntry;                // KeePass record pointer

public:                           // The fields below mirror the KeePass Entry but are easier
                                  // to use in this App.
KpID      kpId;                   // Unique ID of KeePass record
String    group;                  // Group Name that corresponds to groupId
uint      imageId;                // Icon id -- Ignored in this implementation
String    title;                  // Entry title
String    url;
String    userName;               // User Name
String    password;               // Password
String    extra;                  // used to hold notes (KP field: pszAdditional)
KpDate    creation;               // date created
KpDate    lastMod;                // Last Modification
KpDate    lastAccess;             // Last Access
String    binDesc;                // Description of binary data
KpBinData binData;                // Byte block represented by pointer and no of bytes

            KpRecord();
           ~KpRecord() { }

  void      clear();

  KpRecord& operator= (KpEntry*  kEntry);       // Copy an entry from KeePass database to this
                                                // class
  bool      add();                              // Add the fields above to KeePass database
  bool      update() {return update(kpEntry);}  // Update if necessary fields from here to KeePass
  bool      update(KpEntry* kpEntry);           // database

  String&   getEntryDsc();
  String&   getLongEntryDsc();

  bool     find(KpEntry*& kpEntry);             // Find using three fields in this record

private:

  uint    getGroupId(TCchar* name);

  int     findTitle(TCchar* title, int i);
  };



////--------------
//Byte*    bita;               // ptr to binary data or zero
//uint     binDataLng;            // Number of bytes in binaryData
#if 0
  bool    isTitleEmpty()    {return     title.isEmpty();}
  bool    isURLEmpty()      {return       url.isEmpty();}
  bool    isUserNameEmpty() {return  userName.isEmpty();}
#endif
//  KpRecord& operator= (KpRecord& kpRcd);

