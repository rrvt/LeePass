// Utility Functions


#pragma once
#include "Date.h"

typedef struct _PW_TIME PW_TIME;


Date&    toDate(PW_TIME& pwTime);
PW_TIME& toPWTime(Date& dt);


bool setField(String& fld, TCchar* tc);
bool setField(int&    fld, int      v);


       TCchar* set(CEdit&     ctl, TCchar* txt);
       TCchar* set(CComboBox& ctl, TCchar* txt);

       void    clrLbl(CEdit&     ctl, TCchar* txt);
       void    clrLbl(CComboBox& ctl, TCchar* txt);

inline void    clear(CEdit&     ctl) {ctl.SetWindowText(_T(""));}
inline void    clear(CComboBox& ctl) {ctl.SetCurSel(-1);}
inline void    reset(CComboBox& ctl) {ctl.ResetContent();}

