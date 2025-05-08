// Last Pass Integration


#include "pch.h"
#include "LastPass.h"
#include "FileIO.h"
#include "LastpassRcd.h"
#include "MessageBox.h"


void LastPass::expungeFile(TCchar* path) {
String pth = path;
FileIO fi;
int    n;
int    i;
Tchar  ch;

  if (!fi.open(pth, FileIO::Read | FileIO::Write)) return;

  n = fi.getLength();

  for (i = 0, ch = ' '; i < n; i++)
                                {fi.write(ch);   if (ch++ > 126) {fi.write(_T("\r\n")); ch = ' ';}}
  fi.close();

  if (_tremove(pth)) {String err;   getError(GetLastError(), err);   messageBox(err);}
  }

