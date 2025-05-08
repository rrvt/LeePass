// Groups are used to organize KeePass passwords


#pragma once
#include "Date.h"
#include "Expandable.h"
#include "IterT.h"
#include "KpDate.h"

class                    CPwManager;
typedef struct _PW_GROUP PW_GROUP;
class                    MyToolBar;


/*
/// Group structure, containing information about one group.
typedef struct _PW_GROUP {
DWORD   uGroupId;           ///< ID of the group. The ID is a unique identifier in one database.
DWORD   uImageId;           ///< Index of the icon in the image list to use for this group.
TCHAR*  pszGroupName;       ///< Name of the group.

PW_TIME tCreation;          ///< Time when the group was created.
PW_TIME tLastMod;           ///< Time when the group was last modified.
PW_TIME tLastAccess;        ///< Time when the group was last accessed.
PW_TIME tExpire;            ///< Time when the group will expire.

USHORT  usLevel;            ///< Indentation/depth level in the group tree.

#ifdef VPF_ALIGN
USHORT  usDummy;
#endif

  DWORD dwFlags; ///< Used by KeePass internally, don't use (set to 0 for a new structure).
} PW_GROUP, *PPW_GROUP;
*/

class Group {
public:
String name;
DWORD  id;
DWORD  imageID;
ushort level;
KpDate creation;
KpDate lastMod;

  Group() { }
  Group(Group& g) {copy(g);}
 ~Group() { }

  Group& operator= (Group& g) {copy(g); return *this;}
  Group& operator= (PW_GROUP* pwg);

private:

  void copy(Group& g);
  };


class Groups;
typedef IterT<Groups, Group> GrpIter;


class Groups {
CPwManager*           pwMgr;
DWORD                 backupID;
Expandable<Group, 16> data;
bool                  dirty;

public:

          Groups() : pwMgr(0), backupID(0), dirty(true) {}
         ~Groups() { }

  void    setPwMgr(CPwManager* pMgr) {pwMgr = pMgr;}
  void    initialize();

  bool    isDirty() {return dirty;}

  TCchar* getName(uint grpID);
  uint    getID(TCchar* grpName);


  uint    add(TCchar* grpName);
  bool    del(uint grpId, MyToolBar& toolBar, uint id);     // Del group and all entries in group,
                                                            // initialize data and toolbar ComboBox
  bool    del(uint grpId);                                  // Del group from keePass group list
//  bool    isModified()    {return dirty;}

  DWORD   bkupID() {initialize(); return backupID;}

//  void    install(MyToolBar& toolBar, uint id);
  void    install(CComboBox& cbx, TCchar* name);

  void    display();

  int     nData() {return data.end();}                // Returns number of data items in array

private:

  // returns either a pointer to datum at index i in array or zero

  Group* datum(int i) {return 0 <= i && i < nData() ? &data[i] : 0;}

  void   removeDatum(int i) {if (0 <= i && i < nData()) data.del(i);}

  friend typename GrpIter;
  };


extern Groups groups;



//--------------


//Groups() : kpdb(*(IKpDatabase*)0) { }

