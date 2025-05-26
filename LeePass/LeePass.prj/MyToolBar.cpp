// My ToolBar


#include "pch.h"
#include "MyToolBar.h"
#include "Resource.h"

static int NoOfButtonControls =  3;
static int NoOfButtons        =  10;                                    //button(ID_Login),


MyToolBar::MyToolBar() : groupCbx(ID_GroupCbx),     entryCbx(ID_EntryCbx),
                         deleteMenu(ID_DeleteMenu), lastPassMenu(ID_LastPassMenu)
                        {toolBarDim.initialize(NoOfButtonControls, NoOfButtons);}



bool MyToolBar::addMenu(uint id, int idr, TCchar* caption) {
  if (id == deleteMenu.getId())   return add(deleteMenu,  id,  idr, caption);
  if (id == lastPassMenu.getId()) return add(lastPassMenu, id, idr, caption);

  return false;
  }


void MyToolBar::dispatch(uint id) {
  if (id == deleteMenu.getId())   ToolBarBase::dispatch(deleteMenu);
  if (id == lastPassMenu.getId()) ToolBarBase::dispatch(lastPassMenu);
  }


void MyToolBar::clearCbx(  uint id) {
  if (id == groupCbx.getId()) {clear(groupCbx);   return;}
  if (id == entryCbx.getId()) {clear(entryCbx);   return;}
  }


bool MyToolBar::addCBx(uint id) {
  if (id == groupCbx.getId()) return add(groupCbx, id, 10);
  if (id == entryCbx.getId()) return add(entryCbx, id, 10);

  return false;
  }


bool MyToolBar::addCBx(uint id, TCchar* caption) {
String cp  = caption;
int    lng = cp.length();

  if (id == groupCbx.getId() && add(groupCbx,  id, lng))
                                      {ToolBarBase::setCaption(groupCbx,  caption);   return true;}
  if (id == entryCbx.getId() && add(entryCbx, id, lng))
                                       {ToolBarBase::setCaption(entryCbx, caption);   return true;}
  return false;
  }



bool MyToolBar::addCBx(uint id, int idr, TCchar* caption) {
  if (id == groupCbx.getId()) return add(groupCbx,  id, idr, caption);
  if (id == entryCbx.getId()) return add(entryCbx,  id, idr, caption);

  return false;
  }


bool MyToolBar::addCbxItem(uint id, TCchar* txt, int data) {
  if (id == groupCbx.getId()) return addItem(groupCbx, txt, data);
  if (id == entryCbx.getId()) return addItem(entryCbx, txt, data);

  return false;
  }


bool MyToolBar::addCBx(uint id, CbxItem cbxItem[], int n, TCchar* caption) {
  if (id == groupCbx.getId()) return add(groupCbx, id, cbxItem, n, caption);
  if (id == entryCbx.getId()) return add(entryCbx, id, cbxItem, n, caption);

  return false;
  }


bool MyToolBar::addCbxItemSorted(uint id, TCchar* txt, int data) {
  if (id == groupCbx.getId()) return addItemSorted(groupCbx, txt, data);
  if (id == entryCbx.getId()) return addItemSorted(entryCbx, txt, data);

  return false;
  }


bool MyToolBar::find(uint cbxId, TCchar* tc) {
  if (cbxId == groupCbx.getId()) return ToolBarBase::find(groupCbx, tc);
  if (cbxId == entryCbx.getId()) return ToolBarBase::find(entryCbx, tc);

  return false;
  }


void* MyToolBar::getCbxData(uint cbxId, int index) {
  if (cbxId == groupCbx.getId()) return ToolBarBase::getCbxData(groupCbx, index);
  if (cbxId == entryCbx.getId()) return ToolBarBase::getCbxData(entryCbx, index);

  return 0;
  }


// Returns Index >= 0 or -1

int MyToolBar::getCurSel( uint cbxId) {
  if (cbxId == groupCbx.getId()) return ToolBarBase::getCurSel(groupCbx);
  if (cbxId == entryCbx.getId()) return ToolBarBase::getCurSel(entryCbx);

  return -1;
  }


bool MyToolBar::getCurSel( uint id, String& s, void*& data) {
  if (id == groupCbx.getId()) return ToolBarBase::getCurSel(groupCbx, s, data);
  if (id == entryCbx.getId()) return ToolBarBase::getCurSel(entryCbx, s, data);

  return false;
  }


bool MyToolBar::setCurSel( uint cbxId, int index) {
  if (cbxId == groupCbx.getId()) return ToolBarBase::setCurSel(groupCbx, index);
  if (cbxId == entryCbx.getId()) return ToolBarBase::setCurSel(entryCbx, index);

  return false;
  }


bool MyToolBar::setCurSel( uint cbxId, TCchar* tc) {
  if (cbxId == groupCbx.getId()) return ToolBarBase::setCurSel(groupCbx, tc);
  if (cbxId == entryCbx.getId()) return ToolBarBase::setCurSel(entryCbx, tc);

  return false;
  }


uint MyToolBar::getCbxCount(uint id) {
  if (id == groupCbx.getId()) return groupCbx.getCount();
  if (id == entryCbx.getId()) return entryCbx.getCount();

  return 0;
  }


void MyToolBar::setCaption(uint id, TCchar* caption) {
  if (id == groupCbx.getId()) ToolBarBase::setCaption(groupCbx, caption);
  if (id == entryCbx.getId()) ToolBarBase::setCaption(entryCbx, caption);
  }


void MyToolBar::setWthPercent(uint id, int prcnt) {
  if (id == groupCbx.getId())     {ToolBarBase::setWthPercent(groupCbx,     prcnt);   return;}
  if (id == entryCbx.getId())     {ToolBarBase::setWthPercent(entryCbx,     prcnt);   return;}
  if (id == deleteMenu.getId())   {ToolBarBase::setWthPercent(deleteMenu,   prcnt);   return;}
  if (id == lastPassMenu.getId()) {ToolBarBase::setWthPercent(lastPassMenu, prcnt);   return;}
  }



void MyToolBar::setWidth(uint id) {
  if (id == groupCbx.getId())     {ToolBarBase::setWidth(groupCbx);      return;}
  if (id == entryCbx.getId())     {ToolBarBase::setWidth(entryCbx);      return;}
  if (id == deleteMenu.getId())   {ToolBarBase::setWidth(deleteMenu);    return;}
  if (id == lastPassMenu.getId()) {ToolBarBase::setWidth(lastPassMenu);  return;}
  Invalidate();
  }


void MyToolBar::setHeight( uint id) {
  if (id == groupCbx.getId()) ToolBarBase::setHeight(groupCbx);
  if (id == entryCbx.getId()) ToolBarBase::setHeight(entryCbx);
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
  if (id == deleteMenu.getId())   {setWthPercent(deleteMenu,  prcnt);   return;}
  if (id == lastPassMenu.getId())  {setWthPercent(lastPassMenu, prcnt);   return;}
  }
#endif


bool MyToolBar::addMenu(uint id, CbxItem cbxItem[], int n, TCchar* caption) {
  if (id == deleteMenu.getId())   return add(deleteMenu,  id, cbxItem, n, caption);
  if (id == lastPassMenu.getId())  return add(lastPassMenu, id, cbxItem, n, caption);

  return false;
  }


bool MyToolBar::addCBx(uint id, int idr, TCchar* caption) {
  if (id == groupCbx.getId())   return add(groupCbx,  id, idr, caption);
  if (id == entryCbx.getId())  return add(entryCbx, id, idr, caption);

  return false;
  }


bool MyToolBar::addCbxItem(uint id, TCchar* txt, int data) {
  if (id == groupCbx.getId())  return addItem(groupCbx,  txt, data);
  if (id == entryCbx.getId()) return addItem(entryCbx, txt, data);

  return false;
  }


bool MyToolBar::addCBx(uint id, CbxItem cbxItem[], int n, TCchar* caption) {
  if (id == groupCbx.getId())  return add(groupCbx,  id, cbxItem, n, caption);
  if (id == entryCbx.getId()) return add(entryCbx, id, cbxItem, n, caption);

  return false;
  }


bool MyToolBar::addCbxItemSorted(uint id, TCchar* txt, int data) {
  if (id == groupCbx.getId())  return addItemSorted(groupCbx,  txt, data);
  if (id == entryCbx.getId()) return addItemSorted(entryCbx, txt, data);

  return false;
  }

#endif
#ifdef Examples
                       ,editBox(ID_EditBox), deleteMenu(ID_PopupMenu),
                         lastPassMenu(ID_PopupMenu1)
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

