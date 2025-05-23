// Utility Functions


#pragma once
#include "Date.h"
#include "KpDataDef.h"


extern TCchar* MasterKey;


inline bool    isEmpty(TCchar* tc)        {return !tc || !*tc;}
inline bool    isProhibited(TCchar* tgt)  {return String(tgt) == MasterKey;}

       bool    setField(String& fld, TCchar* tc);
       bool    setField(int&    fld, int      v);


       TCchar* setLbl(CEdit&     ctl, TCchar* txt);
       TCchar* setLbl(CComboBox& ctl, TCchar* txt);
       TCchar* setLbl(CStatic&   ctl, TCchar* txt);

       void    clrLbl(CEdit&     ctl, TCchar* txt);
       void    clrLbl(CComboBox& ctl, TCchar* txt);
       void    clrLbl(CStatic&   ctl, TCchar* txt);

       void    clrLbl(String& fld, TCchar* lbl);

       bool    isEmpty(CEdit& ctl, TCchar* lbl);

inline void    clear(CEdit&     ctl) {ctl.SetWindowText(_T(""));}
inline void    clear(CStatic&   ctl) {ctl.SetWindowText(_T(""));}
inline void    clear(CComboBox& ctl) {ctl.SetCurSel(-1);}
inline void    reset(CComboBox& ctl) {ctl.ResetContent();}

       void    expunge(void* p, int nBytes);
       void    expunge(TCchar* path);

       Date&   getDate(PwTime& pwTime);
       PwTime& getPwTime(Date& dt);

       void    setVisible(CEdit* ctl, bool visible);
       void    setVisible(CEdit& ctl, bool visible);

       void    setEdit(  CEdit& ctl, TCchar* txt);
       void    setStatic(CStatic& ctl, TCchar* txt);

inline double  lg2(double v) {return v != 0.0 ? log(v) / log(2) : 0.0;}

       bool    chk(int err);

