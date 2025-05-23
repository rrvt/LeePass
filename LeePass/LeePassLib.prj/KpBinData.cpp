// KeePass Binary Field


#include "pch.h"
#include "KpBinData.h"
#include "FileIO.h"

#include <random>

#include "MessageBox.h"         /// Debugging


void KpBinData::expunge() {
static random_device       rd;
mt19937                    gen(rd());
uniform_int_distribution<> distribute(0, 255);
int                        i;
Byte*                      q;

  if (!p) return;

  for (i = 0, q = p; i < n; i++) *q++ = (Byte) distribute(gen);   clear();
  }


bool KpBinData::get(Byte* q, uint m)
        {NewArray(Byte);    if (m) {p = AllocArray(m);   n = m;   memcpy(p, q, n);}   return true;}


TCchar* KpBinData::save(TCchar* name) {
FileIO fo;

  messageBox(name);

  if (!fo.open(name, FileIO::Create | FileIO::Write)) return 0;

  if (!fo.write(&n, sizeof(n)) || !fo.write(p, n)) return 0;

  return name;
  }

