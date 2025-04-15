// Interface to KpLib Dll


#pragma once
#include "KpSDK.h"
#include "Record.h"

class CPwManager;
class LastPassRcd;
class LPExtras;


class KpLib {

CPwManager* pwMgr;

public:
Record      rcd;

              KpLib() {InitManager((void**) &pwMgr, true);}
             ~KpLib() {DeleteManager(pwMgr);}

  CPwManager* newDatabase( String& path, Cstring& password);
  CPwManager* openDatabase(TCchar* path, Cstring& password);

  void        dspEncryption();
  String      getVersion();

  void        saveDatabase(TCchar* path);
  bool        setPassword(Cstring& password);

  bool        importFile(String& path);
  bool        store(LastPassRcd& lpRcd);

private:

  bool        chk(int err);
  };



//////-------------------

//  void        addEntry();


  //  void        removeDups();
//  void        findAllDups(int tgtIndex);
//  int         findTitle(TCchar* title, int index)
//                                              {return Find(pwMgr, title, true, PWMF_TITLE, index);}
//  int         compare(int index, int dupIndex);

