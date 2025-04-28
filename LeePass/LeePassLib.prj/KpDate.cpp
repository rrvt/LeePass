// KeePass Date/Time


#include "pch.h"
#include "KpDate.h"
#include "Utility.h"

#include "MessageBox.h"


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


KpDate&  KpDate::operator= (PwTime& pwTime) {dt = getDate(pwTime);   return *this;}



////----------

//KpDate::operator PwTime&() {return getPwTime(dt);}

#if 0
Date& KpDate::toDate(PW_TIME& pwTime) {
static Date dt;
Date        x(pwTime.shYear, pwTime.btMonth,  pwTime.btDay,
              pwTime.btHour, pwTime.btMinute, pwTime.btSecond);

  return dt = x;
  }


PW_TIME& KpDate::toPWTime(Date& dt) {
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
#if 1
#else
  if (pwTime.shYear < 1970) {Date x(1970, 1, 1, 0, 0, 0);   ((Date)*this) = x;   return *this;}

  Date x(pwTime.shYear, pwTime.btMonth,  pwTime.btDay,
       pwTime.btHour, pwTime.btMinute, pwTime.btSecond);

  ((Date)*this) = x;
#endif
#if 1
#else
static PwTime pwTime;

  pwTime.shYear   = getYear();
  pwTime.btMonth  = getMonth();
  pwTime.btDay    = getDay();
  pwTime.btHour   = getHour();
  pwTime.btMinute = getMinute();
  pwTime.btSecond = getSecond();

  return pwTime;
#endif

