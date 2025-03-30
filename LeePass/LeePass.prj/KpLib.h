// Interface to KpLib Dll


#pragma once
#include "KpSDK.h"
#include "Record.h"

class CPwManager;


class KpLib {

CPwManager* pwMgr;
Record      rcd;

public:

              KpLib() {InitManager((void**) &pwMgr, true);}
             ~KpLib() {DeleteManager(pwMgr);}

  CPwManager* openDatabase(TCchar* path, Cstring& password);
  void        saveDatabase(TCchar* path);

  Record&     record() {return rcd;}

  void        addEntry();

private:

  bool chk(int err);
  };

