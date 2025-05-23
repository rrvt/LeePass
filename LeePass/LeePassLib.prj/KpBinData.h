// KeePass Binary Field


#pragma once
#include "NewAllocator.h"


class KpBinData {
public:

String path;
int    n;
Byte*  p;

  KpBinData() : n(0), p(0) { }

  void    clear() {NewArray(Byte);  if (!p) return;  FreeArray(p);  p = 0;  n = 0;}
  void    expunge();

  bool    isEmpty() {return !n;}
  int     length()  {return n;}

  bool    get(Byte* q, uint m);

  TCchar* save(TCchar* path);
  };



