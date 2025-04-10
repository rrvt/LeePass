// KeePass Date/Time


#pragma once
#include "Date.h"
#include "KpSDK.h"

typedef struct _PW_TIME PwTime;


class KpDate : public Date {

public:

  PwTime& today() {getToday();   return *this;}

  KpDate&  operator= (PwTime& pwTime);

//  Date&    toDate(PW_TIME& pwTime);
//  PW_TIME& toPTime(Date& dt);

  operator PwTime&();
  };

