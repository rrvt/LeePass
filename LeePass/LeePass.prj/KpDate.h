// KeePass Date/Time


#pragma once
#include "Date.h"
#include "KpSDK.h"



class KpDate : public Date {

public:

  PwTime& today() {getToday();   return *this;}

  KpDate&  operator= (PwTime& pwTime);

  operator PwTime&();
  };




//////////////------------------

//  Date&    toDate(PW_TIME& pwTime);
//  PW_TIME& toPTime(Date& dt);

