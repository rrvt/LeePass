// Timer Thread


#include <atomic>
#include <thread>


typedef int WINAPI MyThreadFunction(HWND hndl, void* param);
#define WM_MY_MESSAGE (WM_USER + 100)

extern atomic<bool> threadFlag;


class Thread {

thread       myThread;
HWND         winHndl;
bool         running;
atomic<int>  result;

public:

  Thread() : running(false), result(0) { }
 ~Thread() { }

  void set(HWND hwnd) {winHndl = hwnd;}

  bool isRunning()  {return running;}       // Call from app to determine if thread running

  void start(MyThreadFunction& fn, void* param);
  void join();
  void stop();
  int  getResult() {return result;}
  };


int WINAPI timerFunction(HWND winHndl, void* param);

