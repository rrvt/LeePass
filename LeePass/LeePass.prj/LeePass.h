// LeePass.h -- Template for a Dialog Box Application


#pragma once
#include "CDialogApp.h"


class LeePass : public CDialogApp {
public:

UINT  m_nAppLook;

  LeePass() noexcept;

public:

  virtual BOOL InitInstance();
  virtual int  ExitInstance();
  };


extern LeePass theApp;
