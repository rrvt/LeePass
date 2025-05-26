// Interface to KpLib Dll


#pragma once
#include "KpRecord.h"

class CPwManager;
class LastPassRcd;
class LPExtras;
class PathDlgDsc;
class CSVOutF;


class KpLib {

bool        backupDone;

public:

KpRecord    kpRcd;

              KpLib();
             ~KpLib();

  bool        newDatabase( String& path, Cstring& password);
  bool        openDatabase(TCchar* path, Cstring& password);

  void        dspEncryption();
  String      getVersion();

  bool        saveDatabase(TCchar* path);

  bool        importFile(String& path);
  bool        store(LastPassRcd& lpRcd);

  void        exportFile();

private:

  bool        setPassword(Cstring& password);
  void        writeRecord(KpEntry* kpEntry, CSVOutF& csv);
  };


extern KpLib kpLib;





//////-------------------

//  void        addEntry();


  //  void        removeDups();
//  void        findAllDups(int tgtIndex);
//  int         findTitle(TCchar* title, int index)
//                                              {return Find(pwMgr, title, true, PWMF_TITLE, index);}
//  int         compare(int index, int dupIndex);
#if 0
  void        saveEntry(KpEntry* kpEntry);
  void        saveGroup(uint grpId);
  void        recoverEntries();
#endif
//  bool        startSave(TCchar* title, CSVOutF& csv);
//  bool        openSave(PathDlgDsc& dsc, CSVOutF& csv);
//  bool        restoreRcd();

//  void        header(CSVOutF& csvOut);
//  void        saveRcd(KpEntry* kpEntry, CSVOutF& csv);
//  TCchar*     binPath(CSVOutF& csv);

//#include "KpSDK.h"
//#include "KpSearch.h"

#if 0
  void        clrSrch() {                  kpSrch.clear();}
  bool        isSrchInit() {        return kpSrch.isInitialized();}
  void        setSrchGrp(uint x) {         kpSrch.setGroup(x);}

  KpEntry*    find(SearchDlg& dlg) {return kpSrch.find(dlg);}
  KpEntry*    findNext()       {    return kpSrch.next();}
#endif

