// KeePass Search


#pragma once
#include "KpSDK.h"
#include "SearchDlg.h"


class KpSearch {

CPwManager* pwMgr;
uint        grpId;
uint        fldFlags;
bool        caseSens;
SrchMode    srchMode;
int         index;
String      tgt;

public:

  KpSearch() : pwMgr(0), grpId(-1), fldFlags(PWMF_TITLE),
                                  caseSens(false), srchMode(AnyWhere), index(-1) { }
 ~KpSearch() { }

  void     setPwMgr(CPwManager* pMgr) {pwMgr     = pMgr;}

  void     clear() {grpId = fldFlags = 0;   index = -1;   caseSens = false;}

  void     setGroup(uint x)           {grpId      = x;}

  bool     isInitialized() {return fldFlags && index >= 0;}

  KpEntry* find(SearchDlg& dlg);
  KpEntry* next();

private:

  bool search();
  bool wordSrch();
  bool fieldSrch();
  bool prefixSrch();
  bool anyWhrSrch();
  };



////----------------

#if 1
#else
  void     setAttr(SearchDlg& dlg);
bool        wholeWord;
bool        wholeField;
bool        beginning;
bool        anywhere;
bool        regExpr;
#endif

