// KeePass Search


#pragma once
#include "KpDataDef.h"
#include "SearchDlg.h"


class KpSearch {

uint         grpId;
uint         fldFlags;
bool         caseSens;
SrchMode     srchMode;
int          index;
String       tgt;

public:

  KpSearch();
 ~KpSearch() { }

  void     clear() {grpId = fldFlags = 0;   index = -1;   caseSens = false;}

  bool     isInitialized() {return fldFlags && index >= 0;}

  KpEntry* find(uint curGrpId);
  KpEntry* next();

  KpEntry* findMasterKey();

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

//  void     setGroup(uint x)           {grpId      = x;}
//#include "KpSDK.h"

