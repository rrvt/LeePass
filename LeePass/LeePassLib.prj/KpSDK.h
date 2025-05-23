// KeePass SDK Interface

#pragma once
#include "KpDataDef.h"
#include <assert.h>


typedef std::basic_string<TCHAR> std_string;    // Required by IKpConfig.h

#include "SDK/KpSDK.h"
#include "ManagerAPI.h"
#include "StructAPI.h"



extern CPwManager* kpMgr;                              // The only  path to Kp Database


/////-------------
//typedef struct _PW_ENTRY KpEntry;

