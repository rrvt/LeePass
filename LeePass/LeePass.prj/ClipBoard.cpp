


#include "pch.h"
#include "ClipBoard.h"
#include "MessageBox.h"


bool ClipBoard::clear() {

  if (!OpenClipboard(0)) {messageBox(_T("Cannot open the Clipboard")); return false;}

  if (!EmptyClipboard())
                 {messageBox(_T("Cannot empty the Clipboard"));    CloseClipboard(); return false;}

  CloseClipboard();   return true;
  }




bool ClipBoard::load(TCchar* tc) {
ToAnsi  ansi(tc);
size_t  cbStr = ansi.length() + 1;
char*   p;
HGLOBAL hData;

  NewArray(char);   p = AllocArray(cbStr);   if (!p) return false;

  hData = GlobalAlloc(GMEM_MOVEABLE, cbStr);

  memcpy_s(GlobalLock(hData), cbStr, ansi(), cbStr);

  GlobalUnlock(hData);   FreeArray(p);

  if (!OpenClipboard(0)) {messageBox(_T("Cannot open the Clipboard")); return false;}

  if (!EmptyClipboard())
                 {messageBox(_T("Cannot empty the Clipboard"));    CloseClipboard(); return false;}

  if (::SetClipboardData(CF_TEXT, hData) == NULL)
                 {messageBox(_T("Unable to set Clipboard data"));  CloseClipboard(); return false;}

  CloseClipboard();   return true;
  }




static uint second = 1000;        // miliseconds


int WINAPI clipBoardTimer(void* param) {
int x = 0;
int n = (int) param;
int i;

  for (i = 0; i < n; i++) {
    Sleep(second);   x++;

    if (aThreadFlag) break;
    }

  return x;
  }

