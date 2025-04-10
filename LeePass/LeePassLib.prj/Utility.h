// Utility Functions


#pragma once
#include "Date.h"

#if 0
typedef struct _PW_TIME PW_TIME;


Date&    toDate(PW_TIME& pwTime);
PW_TIME& toPWTime(Date& dt);
#endif


bool setField(String& fld, TCchar* tc);
bool setField(int&    fld, int      v);


       TCchar* setLbl(CEdit&     ctl, TCchar* txt);
       TCchar* setLbl(CComboBox& ctl, TCchar* txt);

       void    clrLbl(CEdit&     ctl, TCchar* txt);
       void    clrLbl(CComboBox& ctl, TCchar* txt);

inline void    clear(CEdit&     ctl) {ctl.SetWindowText(_T(""));}
inline void    clear(CComboBox& ctl) {ctl.SetCurSel(-1);}
inline void    reset(CComboBox& ctl) {ctl.ResetContent();}

       void expunge(void* p, int nBytes);

