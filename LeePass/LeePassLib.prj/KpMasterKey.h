// Manage Master Key


#pragma once
#include "KpSearch.h"


class KpMasterKey {

KpSearch    kpSrch;

public:

  KpMasterKey() { }
 ~KpMasterKey() { }


  bool change();
  void save(Cstring& masterKey);
  bool verify();
  bool isKey(Cstring& tgt);
  };


///////----------
//#include "KpSDK.h"

