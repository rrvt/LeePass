// Thread Base -- Useful for starting independent worker threads


#pragma once
#include <atomic>
#include <thread>


typedef int WINAPI AThreadFunction(void* param);

extern atomic<bool> aThreadFlag;


class ThreadBase {

thread myThread;
HWND   hndl;
bool   running;
int    result;
int    msgID;

public:

  ThreadBase() : hndl(0), running(false), result(0) { }
 ~ThreadBase() { }

  void set(HWND hwnd, int userMsg) {hndl = hwnd;   msgID = userMsg;}

  bool isRunning()  {return running;}       // Call from app to determine if thread running

  void start(AThreadFunction& fn, void* param);

  void stop();

private:

  void join();
  };


