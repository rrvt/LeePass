// Password LPentry data


#include "pch.h"
#include "LastPassRcd.h"
#include "GetPathDlg.h"
#include "Groups.h"
#include <random>



TCchar* NotesURL = _T("http://sn");             // flag in url for a note


LastPassRcd::~LastPassRcd() {clear();}


void LastPassRcd::clear() {
  clr(url);     clr(userName);  clr(password);   clr(totp);
  clr(extra);   clr(name);      clr(group);      clr(fav);
  }


void LastPassRcd::clr(String& s) {s.expunge();   s.clear();}


bool LastPassRcd::open(String& path) {
PathDlgDsc dsc;

  dsc(_T("LastPass Export File"), _T(""), _T("*"), _T("*.*"));

  return lex.open(dsc, path);
  }


// url,username,password,totp,extra,name,grouping,fav

bool LastPassRcd::readRecord() {
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
      case 5: name     = tok.name; break;
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
  }





       TCchar*   NoteType = _T("NoteType:");
       TCchar*   AddrType = _T("NoteType:Address");
       TCchar*   BankType = _T("NoteType:Bank Account");
       TCchar*   CCType   = _T("NoteType:Credit Card");
       TCchar*   WiFiType = _T("NoteType:Wi-Fi Password");
static TCchar*   EOL      = _T("\n");


bool LastPassRcd::parse() {
String noteType;

  if (url != _T("http://sn")) return false;

  noteType = extract(NoteType, EOL);   if (noteType.isEmpty()) noteType = _T("Note");
  noteType = NoteType + noteType;

  loop {
    if (noteType == AddrType)  {parseAddr(); break;}
    if (noteType == BankType)  {parseBank(); break;}
    if (noteType == CCType)    {parseCC();   break;}
    if (noteType == WiFiType)  {parseWiFi(); break;}
    break;
    }

  addCR(extra);   return true;
  }


static TCchar LineFeed    = _T('\n');
static TCchar CarriageRet = _T('\r');


// Add Carriage Returns to string

void LastPassRcd::addCR(TCchar* tc) {
String s = tc;
int    i;
int    n = s.length();   if (!n) return;

  extra.expunge();

  for (i = 0; i < n; i++) {
    Tchar ch = s[i];

    if (ch == CarriageRet && i+1 < n && s[i+1] == LineFeed) {i++; continue;}

    if (ch == LineFeed) extra += CarriageRet;

    extra += ch;
    }

  s.expunge();
  }


/*
http://sn,,,,"NoteType:Address
Language:en-US
Title:mr
First Name:Robert
Middle Name:Reed
Last Name:Van Tuyl
Username:rrvt
Gender:m
Birthday:September,7,1940
Company:
Address 1:2572 Ohlone Drive
Address 2:
Address 3:
City / Town:San Jose
County:Santa Clara
State:CA
Zip / Postal Code:95132-2630
Country:US
Timezone:-08:00,1
Email Address:rrvt@swde.com
Phone:{""num"":""14087996463"",""ext"":"""",""cc3l"":""USA""}
Evening Phone:
Mobile Phone:{""num"":""14087996463"",""ext"":"""",""cc3l"":""USA""}
Fax:{""num"":""1"",""ext"":"""",""cc3l"":""USA""}
Notes:This a test
of an Address Note",Robert R. Van Tuyl,Notes,1
*/


static TCchar* UserNamePrfx = _T("Username:");
static TCchar* BirthDayPrfx = _T("Birthday:");


void LastPassRcd::parseAddr() {

  userName = extract(UserNamePrfx, EOL);
  password = extract(BirthDayPrfx, EOL);
  }



/*
http://sn,,,,"NoteType:Bank Account
Language:en-US
Bank Name:My Bank
Account Type:Checking
Routing Number:1234567890
Account Number:0987654321
SWIFT Code:13579
IBAN Number:08642d
Pin:1234
Branch Address:1110 Basic Street
Branch Phone:408.999.7777
Notes:A Note",Bank Account,Finance,0 */

//static TCchar* BankNamePrfx  = _T("Bank Name:");
static TCchar* AccountNoPrfx = _T("Account Number:");
static TCchar* PinCodePrfx   = _T("Pin:");

void LastPassRcd::parseBank() {
  userName = extract(AccountNoPrfx, EOL);
  password = extract(PinCodePrfx,   EOL);
  }


/*
http://sn,,,,"NoteType:Credit Card
Language:en-US
Name on Card:R R Van Tuyl
Type:Credit Card
Number:123456706
Security Code:4321
Start Date:January,
Expiration Date:July,2095
Notes:",American Express Card,Finance,1
*/

static TCchar*   CCNumberPrfx = _T("Number:");
static TCchar*   SecCodePrfx  = _T("Security Code:");

void LastPassRcd::parseCC() {

  userName = extract(CCNumberPrfx, EOL);
  password = extract(SecCodePrfx,  EOL);
  }


/*
http://sn,,,,"NoteType:Wi-Fi Password
SSID:DIRECT-6E-HP OfficeJet 250
Password:12345678
Connection Type:ESS
Connection Mode:auto
Authentication:WPA2PSK
Encryption:AES
Use 802.1X:false
FIPS Mode:
Key Type:passPhrase
Protected:false
Key Index:
Notes:<?xml version=""1.0""?>
<WLANProfile xmlns=""http://www.microsoft.com/networking/WLAN/profile/v1"">
 <name>DIRECT-6E-HP OfficeJet 250</name>
 <SSIDConfig>
         <SSID>
                 <hex>4449524543542D36452D4850204F66666963654A657420323530</hex>
                 <name>DIRECT-6E-HP OfficeJet 250</name>
         </SSID>
 </SSIDConfig>
 <connectionType>ESS</connectionType>
 <connectionMode>auto</connectionMode>
 <MSM>
         <security>
                 <authEncryption>
                         <authentication>WPA2PSK</authentication>
                         <encryption>AES</encryption>
                         <useOneX>false</useOneX>
                 </authEncryption>
                 <sharedKey>
                         <keyType>passPhrase</keyType>
                         <protected>false</protected>
                         <keyMaterial>12345678</keyMaterial>
                 </sharedKey>
         </security>
 </MSM>
</WLANProfile>
",DIRECT-6E-HP OfficeJet 250,Notes,0
*/

static TCchar*   SSIDPrfx     = _T("SSID:");
static TCchar*   PasswordPrfx = _T("Password:");


void LastPassRcd::parseWiFi() {

  userName = extract(SSIDPrfx,     EOL);
  password = extract(PasswordPrfx, EOL);
  }


// Cxtract item between beg & end from extra.

String LastPassRcd::extract(TCchar* beg, TCchar* end) {
int    bPos = extra.find(beg) + _tcslen(beg);
int    ePos = extra.find(end, bPos);          if (bPos < 0 || ePos < 0) return _T("");
String s;

  return extra.substr(bPos, ePos-bPos);
  }



// Export KeePass Entries to LastPass

void LastPassRcd::header(CSVOutF& csv) {

  csv << _T("url") << Comma;
  csv << _T("username") << Comma;
  csv << _T("password") << Comma;
  csv << _T("totp") << Comma;
  csv << _T("extra") << Comma;
  csv << _T("name") << Comma;
  csv << _T("grouping") << Comma;
  csv << _T("fav") << vCrlf;
  }


/*
BYTE    uuid[16];       ///< Unique GUID identifying this entry (not only in this database).
DWORD   uGroupId;       ///< ID of the group that contains this entry.
DWORD   uImageId;       ///< Index of the icon in the image list to use for this entry.

TCHAR*  pszTitle;       ///< Title.
TCHAR*  pszURL;         ///< URL.
TCHAR*  pszUserName;    ///< User name.

DWORD   uPasswordLen;   ///< Length of the password (required for memory protection).
TCHAR*  pszPassword;    ///< Password (may be encrypted, use IKpDatabase::UnlockEntryPassword to decrypt).

TCHAR*  pszAdditional;  ///< Notes.

PW_TIME tCreation;      ///< Time when the entry was created.
PW_TIME tLastMod;       ///< Time when the entry was last modified.
PW_TIME tLastAccess;    ///< Time when the entry was last accessed.
PW_TIME tExpire;        ///< Time when the entry will expire.

TCHAR*  pszBinaryDesc;  ///< A string describing the contents of pBinaryData.
BYTE*   pBinaryData;    ///< Attachment data (of length uBinaryDataLen), may be NULL.
DWORD   uBinaryDataLen; ///< Length of the attachment data in bytes.
*/

// url,username,password,totp,extra,name,grouping,fav


void LastPassRcd::writeRecord(KpEntry* kpEntry, CSVOutF& csv) {
String s = kpEntry->pszBinaryDesc;
int    pos = s.find(_T(','));
String totp;
String fav;

  if (pos >= 0) {totp = s.substr(0, pos);   fav = s.substr(pos+1);}

  csv << kpEntry->pszURL                   << Comma;
  csv << kpEntry->pszUserName              << Comma;
  csv << kpEntry->pszPassword              << Comma;
  csv << totp                              << Comma;
  csv << kpEntry->pszAdditional            << Comma;
  csv << kpEntry->pszTitle                 << Comma;
  csv << groups.getName(kpEntry->uGroupId) << Comma;
  csv << fav                               << vCrlf;
  }





//////------------------

#if 0
TCchar* FavOpen     = _T("<Fav~>");
TCchar* FavClose    = _T("</Fav~>\n");
TCchar* ExtraOpen   = _T("<Extra~>");
TCchar* ExtraClose  = _T("</Extra~>\n");
TCchar* ToTpOpen    = _T("<ToTP~>");
TCchar* ToTpClose   = _T("</ToTP~>\n");
TCchar* TimeOpen    = _T("<Time~>");
TCchar* TimeClose   = _T("</Time~>\n");
TCchar* FFLineOpen  = _T("<FFLine~>");
TCchar* FFLineClose = _T("</FFLine~>\n");
#endif
#if 1
#else

  if (!isNote) {
    csv << kpEntry->pszUserName << Comma;
    csv << kpEntry->pszPassword << Comma;
    }
  else csv << Comma << Comma;
#endif
#if 0
  csv << _T("Test Entry")       << Comma;
#else
#endif

