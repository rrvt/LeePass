// My ToolBar


#include "pch.h"
#include "MyToolBar.h"
#include "Resource.h"

static int NoOfButtonControls =  3;
static int NoOfButtons        =  10;                                    //button(ID_Login),


MyToolBar::MyToolBar() : cboBx(ID_GroupCbx),     cboBx1(ID_EntryCbx),
                         cbxMenu(ID_DeleteMenu), cbxMenu1(ID_LastPassMenu)
                        {toolBarDim.initialize(NoOfButtonControls, NoOfButtons);}



bool MyToolBar::addMenu(uint id, int idr, TCchar* caption) {
  if (id == cbxMenu.getId())  return add(cbxMenu,  id, idr, caption);
  if (id == cbxMenu1.getId()) return add(cbxMenu1, id, idr, caption);

  return false;
  }


void MyToolBar::dispatch(uint id) {
  if (id == cbxMenu.getId())  ToolBarBase::dispatch(cbxMenu);
  if (id == cbxMenu1.getId()) ToolBarBase::dispatch(cbxMenu1);
  }


void MyToolBar::clearCBx(  uint id) {
  if (id == cboBx.getId())  {clear(cboBx);    return;}
  if (id == cboBx1.getId()) {clear(cboBx1);   return;}
  }


bool MyToolBar::addCBx(uint id) {
  if (id == cboBx.getId())  return add(cboBx,  id, 10);
  if (id == cboBx1.getId()) return add(cboBx1, id, 10);

  return false;
  }


bool MyToolBar::addCBx(uint id, TCchar* caption) {
String cp = caption;
int    lng = cp.length();

  if (id == cboBx.getId() && add(cboBx,  id, lng))
                                        {ToolBarBase::setCaption(cboBx,  caption);   return true;}
  if (id == cboBx1.getId() && add(cboBx1, id, lng))
                                        {ToolBarBase::setCaption(cboBx1, caption);   return true;}
  return false;
  }



bool MyToolBar::addCBx(uint id, int idr, TCchar* caption) {
  if (id == cboBx.getId())   return add(cboBx,  id, idr, caption);
  if (id == cboBx1.getId())  return add(cboBx1, id, idr, caption);

  return false;
  }


bool MyToolBar::addCbxItem(uint id, TCchar* txt, int data) {
  if (id == cboBx.getId())  return addItem(cboBx,  txt, data);
  if (id == cboBx1.getId()) return addItem(cboBx1, txt, data);

  return false;
  }


bool MyToolBar::addCBx(uint id, CbxItem cbxItem[], int n, TCchar* caption) {
  if (id == cboBx.getId())  return add(cboBx,  id, cbxItem, n, caption);
  if (id == cboBx1.getId()) return add(cboBx1, id, cbxItem, n, caption);

  return false;
  }


bool MyToolBar::addCbxItemSorted(uint id, TCchar* txt, int data) {
  if (id == cboBx.getId())  return addItemSorted(cboBx,  txt, data);
  if (id == cboBx1.getId()) return addItemSorted(cboBx1, txt, data);

  return false;
  }


bool MyToolBar::find(uint cbxId, TCchar* tc) {
  if (cbxId == cboBx.getId())  return ToolBarBase::find(cboBx, tc);
  if (cbxId == cboBx1.getId()) return ToolBarBase::find(cboBx1, tc);

  return false;
  }


void* MyToolBar::getCbxData(uint cbxId, int index) {
  if (cbxId == cboBx.getId())  return ToolBarBase::getCbxData(cboBx,  index);
  if (cbxId == cboBx1.getId()) return ToolBarBase::getCbxData(cboBx1, index);

  return 0;
  }


// Returns Index >= 0 or -1

int MyToolBar::getCurSel( uint cbxId) {
  if (cbxId == cboBx.getId())  return ToolBarBase::getCurSel(cboBx);
  if (cbxId == cboBx1.getId()) return ToolBarBase::getCurSel(cboBx1);

  return -1;
  }


bool MyToolBar::getCurSel( uint id, String& s, void*& data) {
  if (id == cboBx.getId())  return ToolBarBase::getCurSel(cboBx,  s, data);
  if (id == cboBx1.getId()) return ToolBarBase::getCurSel(cboBx1, s, data);

  return false;
  }


bool MyToolBar::setCurSel( uint cbxId, int index) {
  if (cbxId == cboBx.getId())  return ToolBarBase::setCurSel(cboBx, index);
  if (cbxId == cboBx1.getId()) return ToolBarBase::setCurSel(cboBx1, index);

  return false;
  }


bool MyToolBar::setCurSel( uint cbxId, TCchar* tc) {
  if (cbxId == cboBx.getId())  return ToolBarBase::setCurSel(cboBx,  tc);
  if (cbxId == cboBx1.getId()) return ToolBarBase::setCurSel(cboBx1, tc);

  return false;
  }


uint MyToolBar::getCbxCount(uint id) {
  if (id == cboBx.getId())  return cboBx.getCount();
  if (id == cboBx1.getId()) return cboBx1.getCount();

  return 0;
  }


void MyToolBar::setCaption(uint id, TCchar* caption) {
  if (id == cboBx.getId())  ToolBarBase::setCaption(cboBx,  caption);
  if (id == cboBx1.getId()) ToolBarBase::setCaption(cboBx1, caption);
  }


void MyToolBar::setWthPercent(uint id, int prcnt) {
  if (id == cboBx.getId())    {ToolBarBase::setWthPercent(cboBx,    prcnt);   return;}
  if (id == cboBx1.getId())   {ToolBarBase::setWthPercent(cboBx1,   prcnt);   return;}
  if (id == cbxMenu.getId())  {ToolBarBase::setWthPercent(cbxMenu,  prcnt);   return;}
  if (id == cbxMenu1.getId()) {ToolBarBase::setWthPercent(cbxMenu1, prcnt);   return;}
  }



void MyToolBar::setWidth(uint id) {
  if (id == cboBx.getId())    {ToolBarBase::setWidth(cboBx);      return;}
  if (id == cboBx1.getId())   {ToolBarBase::setWidth(cboBx1);     return;}
  if (id == cbxMenu.getId())  {ToolBarBase::setWidth(cbxMenu);    return;}
  if (id == cbxMenu1.getId()) {ToolBarBase::setWidth(cbxMenu1);   return;}
  }


void MyToolBar::setHeight( uint id) {
  if (id == cboBx.getId())  ToolBarBase::setHeight(cboBx);
  if (id == cboBx1.getId()) ToolBarBase::setHeight(cboBx1);
  }




//////--------------

#if 0

bool MyToolBar::addEditBox(uint id, int noChars)              {
  if (id == editBox.getId()) return add(editBox, id, noChars);

  return false;
  }


CString MyToolBar::getText(uint id) {
  if (id == editBox.getId()) {return ToolBarBase::getText(editBox);}

  return _T("");
  }


#if 0
void MyToolBar::setWthPercent(uint id, int prcnt) {
  if (id == cbxMenu.getId())   {setWthPercent(cbxMenu,  prcnt);   return;}
  if (id == cbxMenu1.getId())  {setWthPercent(cbxMenu1, prcnt);   return;}
  }
#endif


bool MyToolBar::addMenu(uint id, CbxItem cbxItem[], int n, TCchar* caption) {
  if (id == cbxMenu.getId())   return add(cbxMenu,  id, cbxItem, n, caption);
  if (id == cbxMenu1.getId())  return add(cbxMenu1, id, cbxItem, n, caption);

  return false;
  }


bool MyToolBar::addCBx(uint id, int idr, TCchar* caption) {
  if (id == cboBx.getId())   return add(cboBx,  id, idr, caption);
  if (id == cboBx1.getId())  return add(cboBx1, id, idr, caption);

  return false;
  }


bool MyToolBar::addCbxItem(uint id, TCchar* txt, int data) {
  if (id == cboBx.getId())  return addItem(cboBx,  txt, data);
  if (id == cboBx1.getId()) return addItem(cboBx1, txt, data);

  return false;
  }


bool MyToolBar::addCBx(uint id, CbxItem cbxItem[], int n, TCchar* caption) {
  if (id == cboBx.getId())  return add(cboBx,  id, cbxItem, n, caption);
  if (id == cboBx1.getId()) return add(cboBx1, id, cbxItem, n, caption);

  return false;
  }


bool MyToolBar::addCbxItemSorted(uint id, TCchar* txt, int data) {
  if (id == cboBx.getId())  return addItemSorted(cboBx,  txt, data);
  if (id == cboBx1.getId()) return addItemSorted(cboBx1, txt, data);

  return false;
  }

#endif
#ifdef Examples
                       ,        editBox(ID_EditBox), cbxMenu(ID_PopupMenu),
                         cbxMenu1(ID_PopupMenu1)
#endif
#ifdef Examples
static int NoOfButtonControls =  6;
static int NoOfButtons        = 13;
#else
#endif
#if 0
bool MyToolBar::addButton( uint id, TCchar* caption) {
  if (id == button.getId()) return add(button,  id, caption);

  return false;
  }
#endif

