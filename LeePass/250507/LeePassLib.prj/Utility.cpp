// Utility Functions


#include "pch.h"
#include "Utility.h"
#include "KpSDK.h"
#include <random>


TCchar* MasterKey  = _T("~!~!~Master~Key~!~!~");


// set field only if not equal

bool setField(String& fld, TCchar* tc) {if (fld ==  tc) return false;   fld = tc;    return true;}
bool setField(int&    fld, int      v) {if (fld ==   v) return false;   fld =  v;    return true;}


TCchar* setLbl(CEdit& ctl, TCchar* txt)
     {Cstring cs;  ctl.GetWindowText(cs);  if (cs.isEmpty()) ctl.SetWindowText(txt);   return txt;}


TCchar* setLbl(CComboBox& ctl, TCchar* txt)
     {Cstring cs;  ctl.GetWindowText(cs);  if (cs.isEmpty()) ctl.SetWindowText(txt);   return txt;}


void    clrLbl(CEdit& ctl, TCchar* txt)
                             {Cstring lbl;   ctl.GetWindowText(lbl);   if (lbl == txt) clear(ctl);}

void    clrLbl(CComboBox& ctl, TCchar* txt)
                             {Cstring lbl;   ctl.GetWindowText(lbl);   if (lbl == txt) clear(ctl);}
void    clrLbl(CStatic&   ctl, TCchar* txt)
                             {Cstring lbl;   ctl.GetWindowText(lbl);   if (lbl == txt) clear(ctl);}



void expunge(void* p, int nBytes) {
static random_device       rd;
mt19937                    gen(rd());
uniform_int_distribution<> distribute(0, 255);
int                        i;
Byte*                      q;

  if (!p || !nBytes) return;

  for (i = 0, q = (Byte*) p; i < nBytes; i++) *q++ = (Byte) distribute(gen);
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

