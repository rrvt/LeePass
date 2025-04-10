// Password LPentry data


#include "pch.h"
#include "LastPassRcd.h"
#include "GetPathDlg.h"
#include <random>



TCchar* NotesURL = _T("http://sn");             // flag in url for a note


LastPassRcd::~LastPassRcd() {clear();}


void LastPassRcd::clear() {
  clr(url);     clr(userName);  clr(password);   clr(totp);
  clr(extra);   clr(name);      clr(grouping);   clr(fav);
  clr(desc);
  }


void LastPassRcd::clr(String& s) {s.expunge();   s.clear();}


bool LastPassRcd::open(String& path) {
PathDlgDsc dsc;

  dsc(_T("LastPass Export File"), _T(""), _T("csv"), _T("*.csv"));

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
      case 6: grouping = tok.name; break;
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





static TCchar*   NoteType = _T("NoteType:");
static TCchar*   AddrType = _T("NoteType:Address");
static TCchar*   BankType = _T("NoteType:Bank Account");
static TCchar*   CCType   = _T("NoteType:Credit Card");
static TCchar*   WiFiType = _T("NoteType:Wi-Fi Password");
static TCchar*   EOL      = _T("\n");


bool LastPassRcd::parse() {

  if (url != _T("http://sn")) return false;

  desc = extract(NoteType, EOL);   if (desc.isEmpty()) desc = _T("Note");
  desc = NoteType + desc;

  loop {
    if (desc == AddrType)  {parseAddr(); break;}
    if (desc == BankType)  {parseBank(); break;}
    if (desc == CCType)    {parseCC();   break;}
    if (desc == WiFiType)  {parseWiFi(); break;}
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

