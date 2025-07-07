// Generator Dialog


#include "pch.h"
#include "GeneratorDlg.h"
#include "Date.h"
#include "IniFileEx.h"
#include "Random.h"
#include "Utility.h"


static TCchar* Section         = _T("Generator");
static TCchar* LengthKey       = _T("Length");
static TCchar* UpperCaseKey    = _T("UpperCase");
static TCchar* LowerCaseKey    = _T("LowerCase");
static TCchar* DigitsKey       = _T("Digits");
static TCchar* PunctuationKey  = _T("Punctuation");
static TCchar* QuotesKey       = _T("Quotes");
static TCchar* UnderScoreKey   = _T("UnderScore");
static TCchar* OtherSpecialKey = _T("OtherSpecial");
static TCchar* OtherCharsKey   = _T("OtherChars");


// GeneratorDlg dialog

IMPLEMENT_DYNAMIC(GeneratorDlg, CDialogEx)


BEGIN_MESSAGE_MAP(GeneratorDlg, CDialogEx)
  ON_BN_CLICKED(IDC_Generate,  &onGenerate)
END_MESSAGE_MAP()


GeneratorDlg::GeneratorDlg(CWnd* pParent) : CDialogEx(IDD_Generator, pParent),   length(_T("")),
                                            upperCase(FALSE),  lowerCase(FALSE), digits(FALSE),
                                            Punctuation(FALSE), quotes(FALSE),
                                            otherSpecial(FALSE) { }

GeneratorDlg::~GeneratorDlg() { }



BOOL GeneratorDlg::OnInitDialog() {

  iniFile.read(Section, LengthKey,       length,    _T("17"));
  iniFile.read(Section, UpperCaseKey,    upperCase,    true);
  iniFile.read(Section, LowerCaseKey,    lowerCase,    true);
  iniFile.read(Section, DigitsKey,       digits,       true);
  iniFile.read(Section, PunctuationKey,  Punctuation,  true);
  iniFile.read(Section, QuotesKey,       quotes,       true);
  iniFile.read(Section, OtherSpecialKey, otherSpecial, true);
  iniFile.read(Section, OtherCharsKey,   otherChars);

  CDialogEx::OnInitDialog();   return TRUE;
  }


void GeneratorDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_Password,     passwordCtl);
    DDX_Text(   pDX, IDC_Password,     password);
    DDX_Text(   pDX, IDC_Length,       length);
    DDX_Check(  pDX, IDC_UpperCase,    upperCase);
    DDX_Check(  pDX, IDC_LowerCase,    lowerCase);
    DDX_Check(  pDX, IDC_Digits,       digits);
    DDX_Check(  pDX, IDC_Punctuation,  Punctuation);
    DDX_Check(  pDX, IDC_Quotes,       quotes);
    DDX_Check(  pDX, IDC_OtherSpecial, otherSpecial);
    DDX_Control(pDX, IDC_Entropy,      entropyCtl);
    DDX_Text(   pDX, IDC_OtherChars,   otherChars);
}


// GeneratorDlg message handlers

static TCchar* UpperCaseSet    = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
static TCchar* LowerCaseSet    = _T("abcdefghijklmnopqrstuvwxyz");
static TCchar* DigitSet        = _T("0123456789");
static TCchar* PunctuationSet  = _T("[]{}()<>:;?.,");
static TCchar* QuotesSet       = _T("`\"'");
static TCchar* OtherSpecialSet = _T("-_~!@#$%^&*=+\\|/");


void GeneratorDlg::onGenerate() {
Date    today;     today.getToday();
Uint64  sec  = today.getSeconds();
long    iRnd = (sec & 0xffffffff) ^ (sec >> 32);      // Random initialization seconds since 1970
Random  rnd(iRnd);
int     nSet;
uint    x;
int     n;
int     i;
int     k;

  UpdateData(true);   set.clear();

  if (lowerCase)             integrate(LowerCaseSet);
  if (digits)                integrate(DigitSet);
  if (otherSpecial)          integrate(OtherSpecialSet);
  if (Punctuation)           integrate(PunctuationSet);
  if (quotes)                integrate(QuotesSet);
  if (upperCase)             integrate(UpperCaseSet);
  addOtherChars();

  nSet = set.length();   n = length.stoi(x);   password.clear();

  for (i = 0; i < n; i++) {
    k = (int) (rnd.next() * nSet);

    password += set[k];
    }

  setEdit(passwordCtl, password);
  dspEntropy(n, nSet);
  }


void GeneratorDlg::integrate(TCchar* tc) {
String s;
String t = tc ? tc : _T("");
int    tLng   = t.length();
int    setLng = set.length();
int    sLng;
int    n;
int    k;
int    i;

  if (!tLng) return;
  if (!setLng) {set = t;   return;}

  if (setLng >= tLng) {s = set;   sLng = setLng;}
  else       {s = t;   t = set;   sLng = tLng;   tLng = setLng;}

  n = sLng / tLng;  k = (sLng + n*(1-tLng)) / 2;

  set.clear();

  for (i = 0; i < tLng; i++, k = n) {set += s.substr(0, k);   s = s.substr(k);   set += t[i];}

  set += s;
  }


void GeneratorDlg::addOtherChars() {
int    n;
int    i;
Tchar  ch;
String s;

  if (otherChars.isEmpty()) return;

  for (i = 0, n = otherChars.length(); i < n; i++)
                                             {ch = otherChars[i];   if (set.find(ch) < 0) s += ch;}
  integrate(s);
  }


void GeneratorDlg::dspEntropy(int pswdLng, int setLng) {
double entropy;
String s;

  entropy = ((double)pswdLng) * lg2(setLng);

  s.format(_T("%4.1f"), entropy);    setStatic(entropyCtl, s.trim());
  }


void GeneratorDlg::OnOK() {

  CDialogEx::OnOK();

  if (password.isEmpty()) onGenerate();

  iniFile.write(Section, LengthKey,       length);
  iniFile.write(Section, UpperCaseKey,    upperCase);
  iniFile.write(Section, LowerCaseKey,    lowerCase);
  iniFile.write(Section, DigitsKey,       digits);
  iniFile.write(Section, PunctuationKey,  Punctuation);
  iniFile.write(Section, QuotesKey,       quotes);
  iniFile.write(Section, OtherSpecialKey, otherSpecial);
  iniFile.write(Section, OtherCharsKey,   otherChars);
  }



//////////-----------------
//  passwordCtl.SetWindowText(password);   passwordCtl.Invalidate();
#if 1
#else
  double  entropy;
  Cstring cs;
  entropy = ((double)n) * lg2(nSet);    cs = entropy;

  setStatic(entropyCtl, cs);
//  entropyCtl.SetWindowText(cs);   entropyCtl.Invalidate();
#endif
//  entropyCtl.SetWindowText(cs);   entropyCtl.Invalidate();
//    DDX_Control(pDX, IDC_Length,       lengthCtl);
//    DDX_Control(pDX, IDC_UpperCase,    upperCaseCtl);
//    DDX_Control(pDX, IDC_LowerCase,    lowerCaseCtl);
//    DDX_Control(pDX, IDC_Digits,       digitsCtl);
//    DDX_Control(pDX, IDC_Punctuation,  PunctuationCtl);
//    DDX_Control(pDX, IDC_Quotes,       quotesCtl);
//    DDX_Control(pDX, IDC_OtherSpecial, otherSpecialCtl);

