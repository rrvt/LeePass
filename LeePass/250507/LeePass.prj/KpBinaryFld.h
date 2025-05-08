// KeePass Binary Field


#pragma once
#include "NewAllocator.h"


class KpBinaryFld {

int  n;

public:
Byte* p;

  KpBinaryFld() : n(0), p(0) { }

  void clear() {NewArray(Byte);  if (!p) return;  FreeArray(p);  p = 0;  n = 0;}
  void expunge();

  bool isEmpty() {return !n;}
  int  length()  {return n;}

  bool get(Byte* q, uint m);
  };



