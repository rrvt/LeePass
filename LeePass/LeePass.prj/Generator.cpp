// Password Generator


#include "pch.h"
#include "Generator.h"
#include "GeneratorDlg.h"
#include "MessageBox.h"
#include "Utility.h"



static TCchar* UpperCase     = _T("A-Z");
static TCchar* LowerCase     = _T("a-z");
static TCchar* Numbers       = _T("0-9");
static TCchar* SpecialChars  = _T("!-/");
static TCchar* SpecialChars1 = _T(":-@");
static TCchar* SpecialChars2 = _T("[-`");
static TCchar* SpecialChars3 = _T("{-~");


bool Generator::get(String& password) {
GeneratorDlg dlg;

  if (dlg.DoModal() == IDOK) {

    if (!dlg.password.isEmpty()) messageBox(dlg.password);

    return true;
    }

  return false;
  }



double Generator::quality(TCchar* pswd) {
int    nFreq = noElements(frequencies);
String s;
int    nPswd;
int    i;
int    nChars;
double freq;
double info = 0.0;
String t;

  if (!pswd) return 0;

  s = pswd;   nPswd = s.length();

  ZeroMemory(frequencies, noElements(frequencies) * sizeof(int));
  addChars(UpperCase     );
  addChars(LowerCase     );
  addChars(Numbers       );
  addChars(SpecialChars  );
  addChars(SpecialChars1 );
  addChars(SpecialChars2 );
  addChars(SpecialChars3 );

  for (i = 0 ; i < nPswd; i++) frequencies[s[i]]++;

  for (i = 0, nChars = 0; i < nFreq; i++) if (frequencies[i]) nChars++;

  double k = ((double) nPswd) * lg2(nChars);

  t.format(_T("l*log2(nChar) %f, %i, %i"), k, nPswd, nChars);  messageBox(t);

  for (i = 0; i < nFreq; i++)
                           {freq = ((double) frequencies[i]) / nChars;   info += freq * lg2(freq);}
  info *= -1;

  t.format(_T("%s --> %f"), s.str(), info);

  messageBox(t);

  return info;
  }


void Generator::addChars(TCchar* set) {
Tchar firstCh = set[0];
Tchar lastCh  = set[2];;
Tchar ch;

  if (!set) return;

  for (ch = firstCh; ch <= lastCh; ch++) {
    frequencies[ch]++;
    }
  }

