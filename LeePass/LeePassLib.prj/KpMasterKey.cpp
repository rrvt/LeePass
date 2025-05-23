// Manage Master Key


#include "pch.h"
#include "KpMasterKey.h"
#include "ChngMstrKeyDlg.h"
#include "Groups.h"
#include "KpSDK.h"
#include "VerifyDlg.h"



bool KpMasterKey::change() {
ChngMstrKeyDlg dlg;
bool           rslt = true;

  rslt &= dlg.DoModal() == IDOK;

  rslt &= isKey(dlg.curPswd);

  rslt &= dlg.newPswd == dlg.confirmPswd;

  if (rslt) {

    rslt = chk(SetMasterKey(kpMgr, dlg.newPswd, false, 0, 0, true));

    if (rslt) save(dlg.newPswd);
    else      dlg.newPswd.expunge();
    }

  dlg.confirmPswd.expunge();   return rslt;
  }



void KpMasterKey::save(Cstring& masterKey) {
KpEntry* kpEntry = kpSrch.findMasterKey();
uint     grpId   = groups.getID(MasterKey);   if (!grpId) grpId = groups.add(MasterKey);
bool     rslt;

  if (kpEntry) {
    UnlockEntryPassword(kpMgr, kpEntry);
      rslt = masterKey != kpEntry->pszPassword;
    LockEntryPassword(kpMgr, kpEntry);

    if (rslt) {PE_SetPasswordAndLock(kpMgr, kpEntry, masterKey);}
    }

  else CreateEntry(kpMgr, grpId, MasterKey, MasterKey, 0, masterKey, 0);

  masterKey.expunge();
  }


bool KpMasterKey::verify() {
VerifyDlg dlg;
bool      rslt;

  rslt = dlg.DoModal() == IDOK;

  if (rslt) rslt &= isKey(dlg.curPswd);
  else      dlg.curPswd.expunge();

  return rslt;
  }


bool KpMasterKey::isKey(Cstring& tgt) {
KpEntry* kpEntry = kpSrch.findMasterKey();   if (!kpEntry) return false;
bool     rslt;

  UnlockEntryPassword(kpMgr, kpEntry);
    rslt = tgt == kpEntry->pszPassword;
  LockEntryPassword(kpMgr, kpEntry);

  tgt.expunge();   return rslt;
  }

