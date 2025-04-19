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
#include "KpSDK.h"
#include "KpCBxFld.h"
#include "KpStringFld.h"

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

class Record {

CPwManager* pwMgr;
KpEntry*    kpEntry;              // KeePass record pointer

public:

KpCBxFld    group;                // Group Name that corresponds to groupId
uint        imageId;              // Icon id -- Ignored in this implementation
KpStringFld title;                // Entry title
KpStringFld url;
KpStringFld name;                 // User Name
KpStringFld password;             // Password
KpStringFld notes;                // used to hold notes (KP field: pszAdditional)
KpDate      creation;             // date created
KpDate      lastMod;              // Last Modification
KpDate      lastAccess;           // Last Access
KpStringFld binDesc;              // Description of binary data
Byte*       binData;              // ptr to binary data or zero
uint        binDataLng;           // Number of bytes in binaryData

  Record();
 ~Record();

  void    setPwMgr(CPwManager* pMgr) {pwMgr = pMgr;}

  void    clear();

  Record& operator= (KpEntry* kpRcd);

  bool    find(KpEntry*& kpEntry);   // Find if title, name, url is in the current KeePass database

// New Record
  bool    setTitle(CEdit& ctl);
  bool    setURL(CEdit& ctl);
  bool    setName(CEdit& ctl);
  bool    setPassword(CEdit& ctl);
  bool    setNotes(CEdit& ctl);
  bool    setBinaryDesc(CEdit& ctl);
  bool    setGroup(CComboBox& ctl);

  bool    update(KpEntry* kpEntry);
  bool    add();

// Update an Existing Record
  bool    updateTitle(CEdit& ctl);
  bool    updateURL(CEdit& ctl);
  bool    updateName(CEdit& ctl);
  bool    updatePassword(CEdit& ctl);
  bool    updateNotes(CEdit& ctl);
  bool    updateLastMod();
  bool    updateLastAccess();
  bool    updateBinaryDesc(CEdit& ctl);
  bool    updateGroup(CComboBox& ctl);

  bool    isTitleEmpty(CEdit& ctl) {return title.isEmpty();}
  bool    isURLEmpty(  CEdit& ctl) {return   url.isEmpty();}
  bool    isNameEmpty( CEdit& ctl) {return  name.isEmpty();}

  String& getEntryDsc();

//  void    setAnote() {aNote = url == NotesURL;}
//  bool    isNote()   {return aNote;}

private:

  uint    getGroupId(TCchar* name);
  int     findTitle(TCchar* title, int index) {return Find(pwMgr, title, true, PWMF_TITLE, index);}
  };

