// KeePass Save/Restore -- Save from a Backup database and restore to current database


#pragma once
#include "KpBinData.h"
#include "KpDataDef.h"

class PathDlgDsc;
class CSVOutF;


class KpSaveRestore {

int         binIndex;

String      group;
uint        ImageId;
String      title;
String      url;
String      userName;
String      password;
String      extra;
KpBinData   binData;

public:

  KpSaveRestore() : binIndex(0) { }
 ~KpSaveRestore() { }

  void    saveEntry(KpEntry* kpEntry);
  void    saveGroup(uint grpId);
  bool    open(PathDlgDsc& dsc, CSVOutF& csv);
  void    recover();

private:

  bool    start(TCchar* title, CSVOutF& csv);
  void    writeRecord(KpEntry* kpEntry, CSVOutF& csv);
  bool    restoreRcd();

  void    header(CSVOutF& csvOut);
  void    saveRcd(KpEntry* kpEntry, CSVOutF& csv);
  TCchar* binPath(CSVOutF& csv);

  bool    readRecord();
  };



////////--------
//#include "KpSDK.h"

