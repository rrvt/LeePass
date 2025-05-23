// KeePass Search

/*
  PWMF_TITLE                // Find fields to search
  PWMF_USER
  PWMF_URL
  PWMF_ADDITIONAL
  PWMF_BinaryDesc

  PWMS_REGEX                // Regular Expression?

  // Find an item

  KP_SHARE DWORD Find(void* pMgr, const TCHAR* pszFindString,
                                              BOOL bCaseSensitive, DWORD fieldFlags, DWORD nStart);
  */

#include "pch.h"
#include "KpSearch.h"
#include "KpSDK.h"
#include "Utility.h"


static TCchar* NotWordChs = _T("[^a-zA-Z0-9_-]");


KpSearch::KpSearch() : grpId(-1), fldFlags(PWMF_TITLE), caseSens(false),
                                                                  srchMode(AnyWhere), index(-1) { }


#if 0
KpEntry* KpSearch::KpSearch::find(uint curGrpId) {
SearchDlg dlg;

  clear();

  if (dlg.DoModal() != IDOK) return 0;

//curGrpId = toolBar.getCurSel(ID_GroupCbx, s, x) ? (uint) x : 0;

  grpId = dlg.groupMode == AllGrpsMode ? 0 : curGrpId;

//  kpLib.setSrchGrp(grpId);

  kpEntry = kpLib.find(dlg);   return kpEntry;
  }
#endif

/*
enum KpSrcTgts {TitleTgt    = PWMF_TITLE,
                UserTgt     = PWMF_USER,
                URLTgt      = PWMF_URL,
                AddtnlTgt   = PWMF_ADDITIONAL,
                BinDescTgt  = PWMF_BinaryDesc,
                RegExprSrch = PWMS_REGEX
                };
*/


KpEntry* KpSearch::find(uint curGrpId) {
SearchDlg dlg;

  if (!kpMgr) return 0;

  clear();

  if (dlg.DoModal() != IDOK) return 0;

  grpId = dlg.groupMode == AllGrpsMode ? 0 : curGrpId;

  if (dlg.titleFld)    fldFlags |= PWMF_TITLE;
  if (dlg.userNameFld) fldFlags |= PWMF_USER;
  if (dlg.urlFld)      fldFlags |= PWMF_URL;
  if (dlg.miscFld)     fldFlags |= PWMF_ADDITIONAL;
  if (dlg.binDescFld)  fldFlags |= PWMF_BinaryDesc;
  caseSens = dlg.caseSens;
  srchMode = (SrchMode) dlg.srchMode;

  switch (srchMode) {
    case RegExpr    : tgt = dlg.target;                             fldFlags |= PWMS_REGEX;  break;
    case WholeWord  : tgt = NotWordChs + dlg.target + NotWordChs;   fldFlags |= PWMS_REGEX;  break;
    case WholeField : tgt = _T('^') + dlg.target + _T('$');         fldFlags |= PWMS_REGEX;  break;
    case Prefix     : tgt = _T('^') + dlg.target + _T('$');         fldFlags |= PWMS_REGEX;  break;
    case AnyWhere   :
    default         : tgt = dlg.target;   break;
    }

  index = -1;   return next();
  }


KpEntry* KpSearch::next() {
KpEntry* kpEntry;                         // index = Find(kpMgr, tgt, caseSens, fldFlags, ++index)
String   s;

  while ((index = Find(kpMgr, tgt, caseSens, fldFlags, ++index)) >= 0) {

    kpEntry = GetEntry(kpMgr, index);   if (!kpEntry) return 0;

    s = kpEntry->pszTitle;   if (s == MasterKey) continue;

    if (!grpId || kpEntry->uGroupId == grpId) return kpEntry;
    }

  return 0;
  }



KpEntry* KpSearch::findMasterKey() {
int      i;

  if (!kpMgr) return 0;

  i = Find(kpMgr, MasterKey, true, PWMF_TITLE, 0);   if (i < 0) return 0;

  return GetEntry(kpMgr, i);
  }




#if 0
bool KpSearch::search() {
String s;
uint   flags = fldFlags;

#if 1

  switch (srchMode) {
    case RegExpr    : s = tgt;                             flags = fldFlags |= PWMS_REGEX; break;
    case WholeWord  : s = NotWordChs + tgt + NotWordChs;   flags |= PWMS_REGEX;            break;
    case WholeField : s = _T('^') + tgt + _T('$');         flags |= PWMS_REGEX;            break;
    case Prefix     : s = _T('^') + tgt + _T('$');         flags |= PWMS_REGEX;            break;
    case AnyWhere   :
    default         : s = tgt;   break;
    }

  index = Find(kpMgr, s, caseSens, flags, ++index);   return index >= 0;

#else
  if (wholeWord)  return wordSrch();
  if (wholeField) return fieldSrch();
  if (beginning)  return prefixSrch();
  if (anywhere)   return anyWhrSrch();
  return false;
#endif
  }

bool KpSearch::wordSrch() {
String s     = NotWordChs + tgt + NotWordChs;
uint   flags = fldFlags | PWMS_REGEX;

  index = Find(kpMgr, s, caseSens, flags, ++index);   return index >= 0;
  }


bool KpSearch::fieldSrch() {
String s = _T('^') + tgt + _T('$');

  fldFlags |= PWMS_REGEX;

  index = Find(kpMgr, s, caseSens, fldFlags, ++index);   return index >= 0;
  }


bool KpSearch::prefixSrch() {
String s = _T('^') + tgt + _T('$');

  fldFlags |= PWMS_REGEX;

  index = Find(kpMgr, s, caseSens, fldFlags, ++index);   return index >= 0;
  }


bool KpSearch::anyWhrSrch()
                     {index = Find(kpMgr, tgt, caseSens, fldFlags, ++index);   return index >= 0;}
#endif
#if 0
void KpSearch::setAttr(SearchDlg& dlg) {
  if (dlg.titleFld)    fldFlags |= PWMF_TITLE;
  if (dlg.userNameFld) fldFlags |= PWMF_USER;
  if (dlg.urlFld)      fldFlags |= PWMF_URL;
  if (dlg.miscFld)     fldFlags |= PWMF_ADDITIONAL;
  if (dlg.binDescFld)  fldFlags |= PWMF_BinaryDesc;
  caseSens   = dlg.caseSens;


#if 1

  srchMode = (SrchMode) dlg.srchMode;

#else
  if (dlg.regExpr)     fldFlags |= PWMS_REGEX;
  wholeWord  = dlg.wholeWord;
  wholeField = dlg.wholeField;
  beginning  = dlg.beginningOf;
  anywhere   = dlg.anyWhere;
#endif
  }
#endif

