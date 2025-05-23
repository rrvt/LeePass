// Utility Functions


#include "pch.h"
#include "Utility.h"
#include "FileIO.h"
#include "KpSDK.h"
#include "MessageBox.h"
#include <random>


TCchar* MasterKey  = _T("~!~!~Master~Key~!~!~");


// set field only if not equal

bool setField(String& fld, TCchar* tc) {if (fld ==  tc) return false;   fld = tc;    return true;}
bool setField(int&    fld, int      v) {if (fld ==   v) return false;   fld =  v;    return true;}


TCchar* setLbl(CEdit& ctl, TCchar* txt)
     {Cstring cs;  ctl.GetWindowText(cs);  if (cs.isEmpty()) ctl.SetWindowText(txt);   return txt;}


TCchar* setLbl(CComboBox& ctl, TCchar* txt)
     {Cstring cs;  ctl.GetWindowText(cs);  if (cs.isEmpty()) ctl.SetWindowText(txt);   return txt;}


TCchar* setLbl(CStatic& ctl, TCchar* txt)
     {Cstring cs;  ctl.GetWindowText(cs);  if (cs.isEmpty()) ctl.SetWindowText(txt);   return txt;}


void    clrLbl(CEdit& ctl, TCchar* txt)
                             {Cstring lbl;   ctl.GetWindowText(lbl);   if (lbl == txt) clear(ctl);}

void    clrLbl(CComboBox& ctl, TCchar* txt)
                             {Cstring lbl;   ctl.GetWindowText(lbl);   if (lbl == txt) clear(ctl);}
void    clrLbl(CStatic&   ctl, TCchar* txt)
                             {Cstring lbl;   ctl.GetWindowText(lbl);   if (lbl == txt) clear(ctl);}

void    clrLbl(String& fld, TCchar* lbl) {if (fld == lbl) fld.clear();}


bool isEmpty(CEdit& ctl, TCchar* lbl)
                         {Cstring cs;   ctl.GetWindowText(cs);   return cs.isEmpty() || cs == lbl;}



void expunge(void* p, int nBytes) {
static random_device       rd;
mt19937                    gen(rd());
uniform_int_distribution<> distribute(0, 255);
int                        i;
Byte*                      q;

  if (!p || !nBytes) return;

  for (i = 0, q = (Byte*) p; i < nBytes; i++) *q++ = (Byte) distribute(gen);
  }


void expunge(TCchar* path) {
String                     pth = path;
FileIO                     fi;
int                        n;
int                        i;
static random_device       rd;
mt19937                    gen(rd());
uniform_int_distribution<> distribute(0, 255);

  if (!fi.open(pth, FileIO::Read | FileIO::Write))
                {String s;   s = _T("Unable to Expunge ");   s += path;   messageBox(s);   return;}

  n = fi.getLength();

  for (i = 0; i < n; i++) fi.write((Byte) distribute(gen));

  fi.close();

  if (_tremove(pth)) {String err;   getError(GetLastError(), err);   messageBox(err);}
  }


Date& getDate(PwTime& pwTime) {
static Date dtZero(1970, 1, 1, 0, 0, 0);
static Date dt;

  if (pwTime.shYear < 1970) {dt = dtZero;}
  else {
    Date x(pwTime.shYear, pwTime.btMonth,  pwTime.btDay,
           pwTime.btHour, pwTime.btMinute, pwTime.btSecond);   dt = x;
    }

  return dt;
  }


PwTime& getPwTime(Date& dt) {
static PwTime pwTime;

  pwTime.shYear   = dt.getYear();
  pwTime.btMonth  = dt.getMonth();
  pwTime.btDay    = dt.getDay();
  pwTime.btHour   = dt.getHour();
  pwTime.btMinute = dt.getMinute();
  pwTime.btSecond = dt.getSecond();

  return pwTime;
  }


static TCchar BulletCh = 0x25CF;


void setVisible(CEdit* ctl, bool visible)
   {Tchar ch = visible ? 0 : BulletCh;   if (ctl) {ctl->SetPasswordChar(ch);   ctl->Invalidate();}}


void setVisible(CEdit& ctl, bool visible)
                {Tchar ch = visible ? 0 : BulletCh;   ctl.SetPasswordChar(ch);   ctl.Invalidate();}


void setEdit(  CEdit&   ctl, TCchar* txt) {ctl.SetWindowText(txt);   ctl.Invalidate();}
void setStatic(CStatic& ctl, TCchar* txt) {ctl.SetWindowText(txt);   ctl.Invalidate();}





static TCchar* ErrorCodes[] = {_T("UNKNOWN"),                _T("SUCCESS"),
                               _T("INVALID_PARAM"),          _T("NO_MEM"),
                               _T("INVALID_KEY"),            _T("NOFILEACCESS_READ"),
                               _T("NOFILEACCESS_WRITE"),     _T("FILEERROR_READ"),
                               _T("FILEERROR_WRITE"),        _T("INVALID_RANDOMSOURCE"),
                               _T("INVALID_FILESTRUCTURE"),  _T("CRYPT_ERROR"),
                               _T("INVALID_FILESIZE"),       _T("INVALID_FILESIGNATURE"),
                               _T("INVALID_FILEHEADER"),     _T("NOFILEACCESS_READ_KEY"),
                               _T("KEYPROV_INVALID_KEY"),    _T("FILEERROR_VERIFY"),
                               _T("UNSUPPORTED_KDBX"),       _T("GETLASTERROR"),
                               _T("DB_EMPTY"),               _T("ATTACH_TOOLARGE")
                               };


bool chk(int err) {
String s;

  if (err == PWE_SUCCESS) return true;

#ifdef _DEBUG

  s = 0 <= err && err < noElements(ErrorCodes) ? ErrorCodes[err] :
                                                  s.format(_T("Err: %i"), err).str();
  messageBox(s);
#endif

  return false;
  }


///////--------------------

#if 0


/*
/// Time structure.
typedef struct _PW_TIME {
USHORT shYear;           ///< Year. 2004 means 2004.
BYTE   btMonth;            ///< Month. Ranges from 1 = Jan to 12 = Dec.
BYTE   btDay;              ///< Day. The first day is 1.
BYTE   btHour;             ///< Hour. Begins with hour 0, maximum value is 23.
BYTE   btMinute;           ///< Minutes. Begins at 0, maximum value is 59.
BYTE   btSecond;           ///< Seconds. Begins at 0, maximum value is 59.

#ifdef VPF_ALIGN
BYTE   btDummy;
#endif
  } PW_TIME, *PPW_TIME;
*/

Date& toDate(PW_TIME& pwTime) {
static Date dt;
Date        x(pwTime.shYear, pwTime.btMonth,  pwTime.btDay,
              pwTime.btHour, pwTime.btMinute, pwTime.btSecond);

  return dt = x;
  }


PW_TIME& toPWTime(Date& dt) {
static PW_TIME pwTime;

  pwTime.shYear   = dt.getYear();
  pwTime.btMonth  = dt.getMonth();
  pwTime.btDay    = dt.getDay();
  pwTime.btHour   = dt.getHour();
  pwTime.btMinute = dt.getMinute();
  pwTime.btSecond = dt.getSecond();
  return pwTime;
  }
#endif

