// Groups are used to organize KeePass passwords

/*
/// Group structure, containing information about one group.

typedef struct _PW_GROUP  {
DWORD   uGroupId;             ///< ID of the group. The ID is a unique identifier in one database.
DWORD   uImageId;             ///< Index of the icon in the image list to use for this group.
TCHAR*  pszGroupName;         ///< Name of the group.

PW_TIME tCreation;            ///< Time when the group was created.
PW_TIME tLastMod;             ///< Time when the group was last modified.
PW_TIME tLastAccess;          ///< Time when the group was last accessed.
PW_TIME tExpire;              ///< Time when the group will expire.

USHORT  usLevel;              ///< Indentation/depth level in the group tree.

#ifdef VPF_ALIGN
USHORT  usDummy;
#endif

DWORD   dwFlags;              ///< Used by KeePass internally, don't use (set to 0 for a new
                              ///  structure).
} PW_GROUP, *PPW_GROUP;
*/

#include "pch.h"
#include "Groups.h"
#include "KpDate.h"
#include "KpSDK.h"
#include "MyToolBar.h"
#include "Utility.h"


Groups groups;


static TCchar* GeneralGrp = _T("General");


void Groups::initialize() {
uint   cnt = GetNumberOfGroups(pwMgr);   if (cnt == nData()) return;
uint   i;
String name;

  data.clear();

  for (i = 0; i < cnt; i++) {

    data.nextData() = GetGroup(pwMgr, i);

    if (data[i].name == _T("Backup")) backupID = data[i].id;
    }
  }



uint Groups::getID(TCchar* grpName) {
GrpIter iter(*this);
Group*  group;

  initialize();   if (!grpName || !*grpName) grpName = GeneralGrp;

  for (group = iter(); group; group = iter++) if (group->name == grpName) return group->id;

  return 0;
  }


uint Groups::add(TCchar* grpName) {
PW_GROUP grp;    ZeroMemory(&grp, sizeof(PW_GROUP));
KpDate   kpDate;

  grp.pszGroupName = (Tchar*) grpName;
  grp.tCreation    = kpDate.today();      //toPWTime(today);
  grp.tLastAccess  = kpDate.today();      //toPWTime(today);
  grp.tLastMod     = kpDate.today();      //toPWTime(today);

  if (!AddGroup(pwMgr, &grp)) return false;

  initialize();   dirty = true;   return getID(grpName);
  }


bool Groups::del(uint grpId, MyToolBar& toolBar, uint id) {

  if (!del(grpId)) return false;

  initialize();   install(toolBar, id);   return true;
  }


bool Groups::del(uint grpId)
                        {bool rslt = DeleteGroupById(pwMgr, grpId);  dirty |= rslt;   return rslt;}



TCchar* Groups::getName(uint grpID) {
GrpIter iter(*this);
Group*  group;
static TCchar* NullName = _T("");

  initialize();

  for (group = iter(); group; group = iter++) if (group->id == grpID) return group->name;

  return NullName;
  }


static TCchar* AllGroups = _T(" All Groups ");


void Groups::install(MyToolBar& toolBar, uint id) {
String  s;
void*   x;
GrpIter iter(*this);
Group*  grp;

  if (!dirty) return;

  toolBar.getCurSel(id, s, x);

  toolBar.clearCBx(id);   toolBar.addCbxItemSorted(id, AllGroups, 0);

  for (grp = iter(); grp; grp = iter++) toolBar.addCbxItemSorted(id, grp->name, grp->id);

  if (!toolBar.setCurSel(id, s) && !toolBar.setCurSel(id, AllGroups)) toolBar.setCurSel(id, 0);

  toolBar.setWthPercent(id, 70);   toolBar.setWidth(id);   toolBar.setHeight(id);   dirty = false;
  }


void Groups::install(CComboBox& cbx, TCchar* name) {
GrpIter iter(*this);
Group*  grp;
int     i;

  cbx.ResetContent();

  for (grp = iter(); grp; grp = iter++)
                                     {i = cbx.AddString(grp->name);   cbx.SetItemData(i, grp->id);}

  if (name) cbx.SetCurSel(cbx.FindString(-1, name));
  }


Group& Group::operator= (PW_GROUP* pwg) {
  if (!pwg) return *this;

  name     = pwg->pszGroupName;
  id       = pwg->uGroupId;
  imageID  = pwg->uImageId;
  level    = pwg->usLevel;
  creation = pwg->tCreation;          // toDate(pwg->tCreation);
  lastMod  = pwg->tLastMod;           // toDate(pwg->tLastMod);

  return *this;
  }


void Group::copy(Group& g) {
  name     = g.name;
  id       = g.id;
  imageID  = g.imageID;
  level    = g.level;
  creation = g.creation;
  lastMod  = g.lastMod;
  }

