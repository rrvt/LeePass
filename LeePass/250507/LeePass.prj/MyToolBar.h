// My ToolBar


#pragma once
#include "TBButton.h"
#include "ToolBarBase.h"
#include "TBCboBx.h"
#include "TBCbxMenu.h"




struct CbxItem;


class MyToolBar : public ToolBarBase {

TBCboBx   cboBx;
TBCboBx   cboBx1;
TBCbxMenu cbxMenu;
TBCbxMenu cbxMenu1;

public:

  MyToolBar();
 ~MyToolBar() { }

  bool    addButton( uint id, TCchar* caption);


  bool    addMenu(   uint id, int idr, TCchar* caption);
  void    dispatch(  uint id);

// Tool Bar ComboBox Functions

  void    clearCBx(  uint id);
  bool    addCBx(    uint id);
  bool    addCBx(    uint id, TCchar* caption);
  bool    addCBx(    uint id, int     idr, TCchar* caption);
  bool    addCbxItem(uint id, TCchar* txt, int data = 0);
  bool    addCBx(    uint id, CbxItem cbxItem[], int n, TCchar* caption);
  bool    addCbxItemSorted(uint id, TCchar* txt, int data = 0);

  bool    find(      uint cbxId, TCchar* tc);
  void*   getCbxData(uint cbxId, int index);
  int     getCurSel( uint cbxId);
  bool    getCurSel( uint cbxId, String& s, void*& data);

  bool    setCurSel( uint cbxId, int index);
  bool    setCurSel( uint cbxId, TCchar* tc);

  uint    getCbxCount(uint id);
  void    setCaption(uint id, TCchar* caption);

  void    setWthPercent(uint id, int prcnt);            // good for Menus and ComboBoxes
  void    setWidth(  uint id);
  void    setHeight( uint id);
  };




////---------------

#ifdef Examples

  bool    addEditBox(uint id, int     noChars);
  CString getText(uint id);

  bool    addMenu(   uint id, int     idr, TCchar* caption);
  bool    addMenu(   uint id, CbxItem cbxItem[], int n, TCchar* caption);

  bool    addCBx(    uint id);
  bool    addCBx(    uint id, int     idr, TCchar* caption);
  bool    addCbxItem(uint id, TCchar* txt, int data = 0);
  bool    addCBx(    uint id, CbxItem cbxItem[], int n, TCchar* caption);
  bool    addCbxItemSorted(uint id, TCchar* txt, int data = 0);
#endif
#ifdef Examples

TBEditBox editBox;
TBCbxMenu cbxMenu1;

#endif
#ifdef Examples
#include "TBEditBox.h"
#endif

