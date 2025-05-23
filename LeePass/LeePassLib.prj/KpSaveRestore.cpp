// KeePass Save/Restore -- Save from a Backup database and restore to current database


#include "pch.h"
#include "KpSaveRestore.h"
#include "CSVLexF.h"
#include "CSVOutF.h"
#include "FileName.h"
#include "GetPathDlg.h"
#include "Groups.h"
#include "KpBinData.h"
#include "KpIndicator.h"
#include "KpIter.h"
#include "KpMasterKey.h"
#include "VerifyDlg.h"


void KpSaveRestore::saveEntry(KpEntry* kpEntry) {
CSVOutF     csv;

  if (!start(_T("Save One Entry"), csv)) return;;

  saveRcd(kpEntry, csv);
  }


void KpSaveRestore::saveGroup(uint grpId) {
CSVOutF  csv;
KpIter   iter;
KpEntry* kpEntry;

  if (!grpId) return;

  if (!start(_T("Save One Group"), csv)) return;

  for (kpEntry = iter(); kpEntry; kpEntry = iter++)
                                             if (kpEntry->uGroupId == grpId) saveRcd(kpEntry, csv);
  }


bool KpSaveRestore::start(TCchar* title, CSVOutF& csv) {
PathDlgDsc  dsc;

  dsc(title, _T("Recovery"), _T("dbrcv"), _T("*.dbrcv"));

  if (!open(dsc, csv)) return false;

  header(csv);   return true;
  }


bool KpSaveRestore::open(PathDlgDsc& dsc, CSVOutF& csv) {
KpMasterKey mk;

  return mk.verify() && csv.open(dsc);
  }


void KpSaveRestore::header(CSVOutF& csv) {

  csv << _T("Group")       << Comma;
  csv << _T("ImageID")     << Comma;
  csv << _T("Title")       << Comma;
  csv << _T("URL")         << Comma;
  csv << _T("UserName")    << Comma;
  csv << _T("Password")    << Comma;
  csv << _T("Extra")       << Comma;
  csv << _T("BinDesc")     << Comma;
  csv << _T("BinDataPath") << vCrlf;
  }



void KpSaveRestore::saveRcd(KpEntry* kpEntry, CSVOutF& csv) {
KpBinData binFld;

  csv << groups.getName(kpEntry->uGroupId) << Comma;
  csv << (uint) kpEntry->uImageId          << Comma;
  csv << kpEntry->pszTitle                 << Comma;
  csv << kpEntry->pszURL                   << Comma;
  csv << kpEntry->pszUserName              << Comma;
  csv << kpEntry->pszPassword              << Comma;
  csv << kpEntry->pszAdditional            << Comma;
  csv << kpEntry->pszBinaryDesc            << Comma;

#if 1
Byte  testBlk[] = {100, 120, 140, 160, 180, 200, 220, 240};
uint  nTest     = noElements(testBlk);
  binFld.get(testBlk, nTest);
#else
  binFld.get(kpEntry->pBinaryData, kpEntry->uBinaryDataLen);
#endif
  if (!binFld.isEmpty()) {csv << binFld.save(binPath(csv));}

  csv << vCrlf;;
  }


TCchar* KpSaveRestore::binPath(CSVOutF& csv) {
static String path = csv.getPath();

  path = getPath(path);   path.format(_T("%sBinData.~%i~.dbrcv"), path.str(), binIndex);

  binIndex++;   return path;
  }


void KpSaveRestore::recover() {
#if 1
#else
PathDlgDsc  dsc;
String      path;
CSVLexF     lex;
KpIndicator idctr;

  dsc(_T("Recovery File"), _T(""), _T("dbrcv"), _T("*.dbrcv"));

  if (!lex.open(dsc, path)) return;

  readRecord();

  for (n = 0; readRecord(); n++) {
    store();
    }
#endif
  }


bool KpSaveRestore::readRecord() {
#if 1
#else
int        field;
CSVtokCode code;
String     stg;

  clear();

  for (field = 0, code = lex.get_token(); code != EOFToken; field++, code = lex.get_token()) {

    CSVtok& tok  = lex.token;   if (tok.code == CommaToken) {lex.accept_token(); continue;}
    CSVtok& tok1 = lex.token1;

    switch (field) {
      case 0: if (tok.code == EolToken) {field--;  lex.accept_token(); continue;}
              url      = tok.name; break;
      case 1: userName = tok.name; break;
      case 2: password = tok.name; break;
      case 3: totp     = tok.name; break;
      case 4: extra    = tok.name; parse(); break;
      case 5: title    = tok.name; break;
      case 6: group    = tok.name; break;
      case 7: fav      = tok.name; break;
      }

    if (tok1.code >= EolToken) {

      if (field  < 7) {errorField.format(_T("No Fields = %i"), field); return false;}

      lex.accept_two_tokens();   return true;
      }

    lex.accept_two_tokens();
    }

  return false;
#endif
  }


bool KpSaveRestore::restoreRcd() {
  }

