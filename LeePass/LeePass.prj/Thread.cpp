// Timer Thread


#include "pch.h"
#include "Thread.h"


atomic<bool> threadFlag;


void Thread::start(MyThreadFunction& fn, void* param) {

  myThread = thread(
    [this, fn, param]() {
      running = true; threadFlag = false;
        result = fn(winHndl, param);
      running = false;
      }
    );

  myThread.detach();
  }


void Thread::join() {
  try {
    /*if (myThread.joinable())*/ myThread.join();
    } catch(...){};
  }


void Thread::stop() {if (!running) return;   threadFlag = true;   join();}




static uint second = 1000;        // miliseconds

int WINAPI timerFunction(HWND winHndl, void* param) {
int x = 0;
int n = (int) param;
int i;

  for (i = 0; i < n; i++) {
    Sleep(second);   x++;

    if (threadFlag) break;
    }

  PostMessage(winHndl, WM_MY_MESSAGE, (WPARAM)x, 0);
  return x;
  }

