// Last Pass Password entry data

/*
/// Entry structure, containing information about one entry.

typedef struct _PW_ENTRY {
BYTE    uuid[16];             ///< Unique GUID identifying this entry (not only in this database).
DWORD   uGroupId;             ///< ID of the group that contains this entry.
DWORD   uImageId;             ///< Index of the icon in the image list to use for this entry.

TCHAR*  pszTitle;             ///< Title.
TCHAR*  pszURL;               ///< URL.
TCHAR*  pszUserName;          ///< User name.

DWORD   uPasswordLen;         ///< Length of the password (required for memory protection).
TCHAR*  pszPassword;          ///< Password (may be encrypted, use
                                   IKpDatabase::UnlockEntryPassword to decrypt).
TCHAR*  pszAdditional;        ///< Notes.

PW_TIME tCreation;            ///< Time when the entry was created.
PW_TIME tLastMod;             ///< Time when the entry was last modified.
PW_TIME tLastAccess;          ///< Time when the entry was last accessed.
PW_TIME tExpire;              ///< Time when the entry will expire.

TCHAR*  pszBinaryDesc;        ///< A string describing the contents of pBinaryData.
BYTE*   pBinaryData;          ///< Attachment data (of length uBinaryDataLen), may be NULL.
DWORD   uBinaryDataLen;       ///< Length of the attachment data in bytes.
} PW_ENTRY, *PPW_ENTRY;
*/


#pragma once
#include "Date.h"
#include "KpID.h"
#include "KpDataDef.h"
#include "KpRecord.h"
#include "KpStringFld.h"


extern TCchar* TitleLbl;
extern TCchar* URLLbl;
extern TCchar* UserNameLbl;


class Record {

KpRecord&    kpRcd;

public:

  Record(KpRecord& kpRecord);
 ~Record();

  void    clear() {kpRcd.clear();}

  Record& operator= (KpEntry* kpEntry) {kpRcd = kpEntry;   return *this;}

  TCchar* group() {return kpRcd.group;}
  KpID&   kpId()  {return kpRcd.kpId;}

  void    setTitleLbl(CEdit& ctl);                    // Set controls with labels iff empty
  void    setURLLbl(CEdit& ctl);
  void    setUserNameLbl(CEdit& ctl);
  void    setPasswordLbl(CEdit& ctl);
  void    setExtraLbl(CEdit& ctl);
  void    setCreationLbl(CStatic& ctl);
  void    setLastModLbl(CStatic& ctl);
  void    setLastAccessLbl(CStatic& ctl);
  void    setBinDescLbl(CEdit& ctl);
  void    setGroupLbl(CComboBox& ctl);

  void    clrTitleLbl(CEdit& ctl);                    // Clear control iff control contains label
  void    clrURLLbl(CEdit& ctl);
  void    clrUserNameLbl(CEdit& ctl);
  void    clrPasswordLbl(CEdit& ctl);
  void    clrExtraLbl(CEdit& ctl);
  void    clrCreationLbl(CStatic& ctl);
  void    clrLastModLbl(CStatic& ctl);
  void    clrLastAccessLbl(CStatic& ctl);
  void    clrBinDescLbl(CEdit& ctl);
  void    clrGroupLbl(CComboBox& ctl);

  void    setTitle(CEdit& ctl);                       // Set control with record field iff not
  void    setURL(CEdit& ctl);                         // empty else set with label
  void    setUserName(CEdit& ctl);
  void    setPassword(CEdit& ctl);
  void    setExtra(CEdit& ctl);
  void    setCreation(CStatic& ctl);
  void    setLastMod(CStatic& ctl);
  void    setLastAccess(CStatic& ctl);
  void    setBinDesc(CEdit& ctl);
  void    setGroup(CComboBox& ctl);

  bool    isProhibited(CEdit& ctl)                    // Master key prohibited
                                {Cstring cs;   ctl.GetWindowText(cs);   return ::isProhibited(cs);}
  bool    isProhibited(CComboBox& ctl)
                                {Cstring cs;   ctl.GetWindowText(cs);   return ::isProhibited(cs);}

// New Record

  bool    getTitle(CEdit& ctl);                       // Get field from control and store in
  bool    getURL(CEdit& ctl);                         // KpRcd field iff control content not a
  bool    getUserName(CEdit& ctl);                    // label
  bool    getPassword(CEdit& ctl);
  bool    getExtra(CEdit& ctl);
  bool    getBinaryDesc(CEdit& ctl);
  bool    getGroup(CComboBox& ctl);

  // kpRcd
  bool    update() {clearLabels();   return kpRcd.update();}               // Update Current Record
  bool    update(KpEntry* kpEntry)                                         // Update Found Record
                   {clearLabels();   return kpRcd.update(kpEntry);}
  bool    add()    {clearLabels();   return kpRcd.add();}                  // Add New Record

  void    updateCreation(CStatic& ctl);              // Update the date/time to today
  void    updateLastMod(CStatic& ctl);
  void    updateLastAccess(CStatic& ctl);

  String& getEntryDsc()     {return kpRcd.getEntryDsc();}
  String& getLongEntryDsc() {return kpRcd.getLongEntryDsc();}

private:

  void    set(TCchar* lbl, String& fld, CEdit& ctl);
  bool    get(CEdit& ctl, TCchar* lbl, String& fld);
  bool    isEmpty(CEdit& ctl, TCchar* lbl);
  void    clearLabels();

  uint    getGroupId(TCchar* name);
  };



////////----------------

#if 0
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
#endif

//  void    setAnote() {aNote = url == NotesURL;}
//  bool    isNote()   {return aNote;}
#if 0
KpEntry*     kpEntry;               // KeePass record pointer

public:

KpID         kpId;                  // Unique ID of KeePass record
KpCBxFld     group;                 // Group Name that corresponds to groupId
uint         imageId;               // Icon id -- Ignored in this implementation
KpStringFld  title;                 // Entry title
KpStringFld  url;
KpStringFld  userName;              // User Name
KpStringFld  password;              // Password
KpStringFld  extra;                 // used to hold notes (KP field: pszAdditional)
KpDate       creation;              // date created
KpDate       lastMod;               // Last Modification
KpDate       lastAccess;            // Last Access
KpStringFld  binDesc;               // Description of binary data
Byte*        binData;               // ptr to binary data or zero
uint         binDataLng;            // Number of bytes in binaryData
#endif
//#include "KpCBxFld.h"
//KpCBxFld     groupCbx;              // Group Name that corresponds to groupId
//bool    find(KpEntry*& kpEntry);   // Find if title, name, url is in the current KeePass database
#if 0
// Update an Existing Record
  bool    updateTitle(CEdit& ctl);
  bool    updateURL(CEdit& ctl);
  bool    updateUserName(CEdit& ctl);
  bool    updatePassword(CEdit& ctl);
  bool    updateExtra(CEdit& ctl);
  bool    updateBinaryDesc(CEdit& ctl);
  bool    updateGroup(CComboBox& ctl);
#endif
//  int     findTitle(TCchar* title, int index) {return Find(kpMgr, title, true, PWMF_TITLE, index);}

