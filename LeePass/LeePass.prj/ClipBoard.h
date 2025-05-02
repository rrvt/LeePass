// Manage ClipBoard


#pragma once
#include "ThreadBase.h"


int WINAPI clipBoardTimer(void* param);


struct ClipBoard : public ThreadBase {

  ClipBoard() : ThreadBase() { }
 ~ClipBoard() { }

  bool clear();

  bool load(TCchar* tc);

  void start(int nSeconds) {ThreadBase::start(clipBoardTimer, (void*) nSeconds);}
  void stop()              {ThreadBase::stop();}
  };



