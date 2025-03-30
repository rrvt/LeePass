// KeePass Binary Field


#include "pch.h"
#include "KpBinaryFld.h"
#include <random>


void KpBinaryFld::expunge() {
static random_device       rd;
mt19937                    gen(rd());
uniform_int_distribution<> distribute(0, 255);
int                        i;
Byte*                      q;

  if (!p) return;

  for (i = 0, q = p; i < n; i++) *q++ = (Byte) distribute(gen);   clear();
  }


bool KpBinaryFld::get(Byte* q, uint m)
                         {NewArray(Byte);    if (m) {p = AllocArray(m);   n = m;}   return true;}

