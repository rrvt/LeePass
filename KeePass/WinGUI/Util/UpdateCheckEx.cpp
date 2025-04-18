/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2025 Dominik Reichl <dominik.reichl@t-online.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "StdAfx.h"
#include "UpdateCheckEx.h"

#include "../../KeePassLibCpp/Util/StrUtil.h"
#include "../../KeePassLibCpp/Util/TranslateEx.h"
#include "../../KeePassLibCpp/PwManager.h" // For PWM_DEVSNAPSHOT
#include "../../KeePassLibCpp/IO/KpInternetStream.h"
#include "../Plugins/PluginMgr.h"
#include "../Plugins/KpApiImpl.h"
#include "../NewGUI/NewGUICommon.h"
#include "../NewGUI/TaskDialog/VistaTaskDialog.h"

#include "../UpdateInfoDlg.h"

typedef struct
{
	HWND hParent;
	CImageList* pImages;
	BOOL bUIOnlyOnUpdate;
} UC_INIT_STRUCT;

DWORD WINAPI CfuEx_Thread(LPVOID lpParameter);

static UC_INIT_STRUCT g_ucInitStruct;

CUpdateCheckEx::CUpdateCheckEx(HWND hParent, CImageList* pImages,
	BOOL bUIOnlyOnUpdate) :
	m_hParent(hParent), m_pImages(pImages), m_bUIOnlyOnUpdate(bUIOnlyOnUpdate)
{
}

HRESULT CUpdateCheckEx::Check(BOOL bRunInThread, HWND hParent, CImageList* pImages,
	BOOL bUIOnlyOnUpdate)
{
	HRESULT hRes = S_OK;

	if(bRunInThread == TRUE)
	{
		g_ucInitStruct.hParent = hParent;
		g_ucInitStruct.pImages = pImages;
		g_ucInitStruct.bUIOnlyOnUpdate = bUIOnlyOnUpdate;

		DWORD dwDummyID = 0; // Pointer may not be NULL on Windows 9x/Me
		HANDLE h = CreateThread(NULL, 0, CfuEx_Thread, NULL, 0, &dwDummyID);

		if((h != NULL) && (h != INVALID_HANDLE_VALUE)) CloseHandle(h);
		else hRes = E_FAIL;
	}
	else
	{
		// CStatusDialogEx dlg(hParent, false, true);
		// dlg.Show();
		// std_string str = TRL("Check for Updates");
		// str += _T("...");
		// dlg.SetStatus(true, str.c_str());

		CUpdateCheckEx uc(hParent, pImages, bUIOnlyOnUpdate);
		uc._RunCheck(NULL);

		// dlg.Release();
	}

	ASSERT(SUCCEEDED(hRes));
	return hRes;
}

DWORD WINAPI CfuEx_Thread(LPVOID lpParameter)
{
	UNREFERENCED_PARAMETER(lpParameter);

	CUpdateCheckEx uc(g_ucInitStruct.hParent, g_ucInitStruct.pImages,
		g_ucInitStruct.bUIOnlyOnUpdate);
	uc._RunCheck(NULL);

	return 0;
}

void CUpdateCheckEx::_RunCheck(void* pDlg)
{
	UC_COMPONENTS_LIST vInstalled;
	CUpdateCheckEx::GetInstalledComponents(vInstalled, true);

	BYTE* pb = NULL;
	std_string strError;
	HRESULT hRes = CUpdateCheckEx::DownloadInfoFile(&pb, strError);
	if(FAILED(hRes))
	{
		SAFE_DELETE_ARRAY(pb);

		_FinalReport(vInstalled, strError.c_str(), true, 0, pDlg);
		return;
	}

	UC_COMPONENTS_LIST vAvailable;
	hRes = CUpdateCheckEx::ParseInfoFile(pb, vAvailable);
	SAFE_DELETE_ARRAY(pb);
	if(FAILED(hRes))
	{
		_FinalReport(vInstalled, TRL("Loading error"), true, 0, pDlg);
		return;
	}

	CUpdateCheckEx::CopyAvailableVersions(vAvailable, vInstalled);

	std_string strReport;
	DWORD dwUpdates = 0;
	CUpdateCheckEx::CompareVersions(vInstalled, true, strReport, dwUpdates);
	_FinalReport(vInstalled, strReport.c_str(), false, dwUpdates, pDlg);
}

void CUpdateCheckEx::_FinalReport(const UC_COMPONENTS_LIST& vInstalled,
	LPCTSTR lpResult, bool bError, DWORD dwUpdates, void* pDlg)
{
	UNREFERENCED_PARAMETER(pDlg);

	LPCTSTR lpMsg = ((lpResult != NULL) ? lpResult : _T(""));

	bool bShowDialog = true;
	if(m_bUIOnlyOnUpdate == TRUE) bShowDialog = (dwUpdates > 0);
	if(m_pImages == NULL) bShowDialog = false;

	if(bShowDialog)
	{
		// if(pDlg != NULL) pDlg->Release();

		// FromHandle, not FromHandlePermanent, due to other thread
		CUpdateInfoDlg dlg(CWnd::FromHandle(m_hParent));
		dlg.InitEx(m_pImages, &vInstalled, lpResult);
		NewGUI_DoModal(&dlg);
	}
	else
	{
		if(bError || (dwUpdates > 0))
			CKpApiImpl::Instance().SetStatusBarText(lpMsg);
	}
}

HRESULT CUpdateCheckEx::DownloadInfoFile(BYTE** ppbData, std_string& strError)
{
	if(ppbData == NULL) { ASSERT(FALSE); return E_POINTER; }
	*ppbData = NULL;

	/* TCHAR tszFile[MAX_PATH + 34];
	ZeroMemory(tszFile, sizeof(TCHAR) * (MAX_PATH + 34));

	URLDownloadToCacheFile(NULL, PWM_URL_VERSION, tszFile, URLOSTRM_GETNEWESTVERSION,
		0, NULL);

	if(_tcslen(tszFile) != 0)
	{
		FILE* fp = NULL;
		_tfopen_s(&fp, tszFile, _T("rb"));

		if(fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			const DWORD uFileSize = static_cast<DWORD>(ftell(fp));
			fseek(fp, 0, SEEK_SET);

			if(uFileSize != 0)
			{
				BYTE* pDataBuf = new BYTE[uFileSize + 2];

				fread(pDataBuf, 1, uFileSize, fp);
				fclose(fp);

				pDataBuf[uFileSize] = 0;
				pDataBuf[uFileSize + 1] = 0;

				*ppbData = pDataBuf;
			}
			else
			{
				strError = TRL("Cache error (cannot open cached file).");
				return E_ACCESSDENIED;
			}
		}
		else
		{
			strError = TRL("Cache error (cannot open cached file).");
			return E_ACCESSDENIED;
		}
	}
	else
	{
		strError = TRL("Connect failed, cannot check for updates.");
		return E_ACCESSDENIED;
	} */

	CKpInternetStream s(PWM_URL_VERSION, false);
	s.SetConnectOptions(5000, 1);
	s.SetTransferOptions(5000);

	std::vector<BYTE> vData;
	HRESULT r = s.ReadToEnd(vData);
	s.Close();
	if(FAILED(r) || (vData.size() == 0))
	{
		strError = TRL("Connect failed, cannot check for updates.");
		return r;
	}

	*ppbData = new BYTE[vData.size() + 2];
	(*ppbData)[vData.size()] = 0;
	(*ppbData)[vData.size() + 1] = 0;
	memcpy(*ppbData, &vData[0], vData.size());

	strError.clear();
	return S_OK;
}

HRESULT CUpdateCheckEx::ParseInfoFile(BYTE* pbData, UC_COMPONENTS_LIST& vList)
{
	if(pbData == NULL) { ASSERT(FALSE); return E_POINTER; }

	vList.clear();

	TCHAR* lpData = _UTF8ToString(pbData);
	if(lpData == NULL) { ASSERT(FALSE); return E_FAIL; }
	std_string strData = lpData;
	SAFE_DELETE_ARRAY(lpData);

	std::vector<std_string> vItems;
	SU_Split(vItems, strData, _T("\r\n"));

	for(size_t i = 0; i < vItems.size(); ++i)
		CUpdateCheckEx::AddComponent(vItems[i], vList);

	return S_OK;
}

void CUpdateCheckEx::AddComponent(const std_string& strLine, UC_COMPONENTS_LIST& vList)
{
	if(strLine.size() == 0) return;

	if(strLine[0] == _T('#')) { ASSERT(FALSE); return; } // Reserved for future use

	std::vector<std_string> vInfo;
	SU_Split(vInfo, strLine, _T("#"));
	if(vInfo.size() < 2) { ASSERT(FALSE); return; }

	std::vector<std_string> vVersion;
	SU_Split(vVersion, vInfo[1], _T("."));
	if(vVersion.size() < 4) { ASSERT(FALSE); return; }

	UC_COMPONENT_INFO c;
	c.strName = vInfo[0];
	c.qwVerAvailable = ((static_cast<UINT64>(_ttol(vVersion[0].c_str())) << 48) |
		(static_cast<UINT64>(_ttol(vVersion[1].c_str())) << 32) |
		(static_cast<UINT64>(_ttol(vVersion[2].c_str())) << 16) |
		static_cast<UINT64>(_ttol(vVersion[3].c_str())));

	vList.push_back(c);
}

void CUpdateCheckEx::GetInstalledComponents(UC_COMPONENTS_LIST& vList,
	bool bDefaultToUnknownStatus)
{
	vList.clear();

	UC_COMPONENT_INFO kp;
	kp.strName = PWM_PRODUCT_NAME_SHORT;
	kp.qwVerInstalled = PWM_VERSION_QW;
	vList.push_back(kp);

	const CPluginManager& pm = CPluginManager::Instance();

	typedef std::vector<KP_PLUGIN_INSTANCE>::const_iterator PlgIter;
	for(PlgIter it = pm.m_plugins.begin(); it != pm.m_plugins.end(); ++it)
	{
		if((it->hinstDLL == NULL) || (it->pInterface == NULL))
		{
			ASSERT(FALSE);
			continue;
		}

		UC_COMPONENT_INFO c;
		c.strName = it->strName;
		c.qwVerInstalled = it->qwVersion;

		vList.push_back(c);
	}

	for(size_t i = 0; i < vList.size(); ++i)
	{
		if(bDefaultToUnknownStatus)
		{
			if(i > 0) vList[i].strStatus = TRL("Plugin unknown.");
			else { vList[i].strStatus = TRL("Unknown"); vList[i].strStatus += _T("."); }

			vList[i].iStatusImage = 44;
		}

		vList[i].qwVerAvailable = 0;
	}

	SortComponentsList(vList);
}

bool _UC_Component_Compare(UC_COMPONENT_INFO x, UC_COMPONENT_INFO y)
{
	if(x.strName == PWM_PRODUCT_NAME_SHORT) return true;
	if(y.strName == PWM_PRODUCT_NAME_SHORT) return false;

	return (x.strName.compare(y.strName) <= 0);
}

void CUpdateCheckEx::SortComponentsList(UC_COMPONENTS_LIST& vList)
{
	std::sort(vList.begin(), vList.end(), _UC_Component_Compare);
}

void CUpdateCheckEx::CopyAvailableVersions(const UC_COMPONENTS_LIST& vAvailable,
	UC_COMPONENTS_LIST& vOut)
{
	for(size_t i = 0; i < vOut.size(); ++i)
	{
		for(size_t j = 0; j < vAvailable.size(); ++j)
		{
			if(vAvailable[j].strName == vOut[i].strName)
				vOut[i].qwVerAvailable = vAvailable[j].qwVerAvailable;
		}
	}
}

void CUpdateCheckEx::CompareVersions(UC_COMPONENTS_LIST& v, bool bSetStatus,
	std_string& strReport, DWORD& dwUpdates)
{
	DWORD dwAvUpdates = 0;
	for(size_t i = 0; i < v.size(); ++i)
	{
		const UINT64 dwIn = v[i].qwVerInstalled;
		const UINT64 dwAv = v[i].qwVerAvailable;

		if(dwIn == dwAv)
		{
			if(bSetStatus)
			{
				v[i].strStatus = TRL("Up to date.");
				v[i].iStatusImage = 53;
			}
		}
		else if(dwIn < dwAv)
		{
			if(bSetStatus)
			{
				v[i].strStatus = TRL("New version available.");
				v[i].iStatusImage = 61;
			}

			++dwAvUpdates;
		}
		else if((dwIn > dwAv) && (dwAv != 0))
		{
			if(bSetStatus)
			{
				v[i].strStatus = TRL("Pre-release version.");
				v[i].iStatusImage = 39;
			}
		}
	}

	dwUpdates = dwAvUpdates;
	if(dwAvUpdates == 0) strReport = TRL("No updates available.");
	else if(dwAvUpdates == 1) strReport = TRL("1 update is available!");
	else
	{
		CString strFmt;
		strFmt.Format(TRL("%u updates are available!"), dwAvUpdates);
		strReport = (LPCTSTR)strFmt;
	}
}

void CUpdateCheckEx::EnsureConfigured(BOOL* pCheck, BOOL* pConfig,
	HWND hParent, HINSTANCE hInstance)
{
	if((pCheck == NULL) || (pConfig == NULL)) { ASSERT(FALSE); return; }
	if((*pConfig) != FALSE) return;

#ifdef PWM_DEVSNAPSHOT
	UNREFERENCED_PARAMETER(hParent);
	UNREFERENCED_PARAMETER(hInstance);
#else
	// If the user has manually enabled the automatic update check
	// before, there's no need to ask him again
	if((*pCheck) == FALSE)
	{
		CString strHdr = TRL("KeePass can automatically check for updates on each program start.");
		
		CString strSub = TRL("Automatic update checks are performed unintrusively in the background. A notification is only displayed when an update is available. Updates are not downloaded or installed automatically.");
		strSub += _T("\r\n\r\n");
		strSub += TRL("No personal information is sent to the KeePass web server. KeePass just downloads a small version information file and compares the available version with the installed version.");

		CString strEnable = TRL("&Enable");
		strEnable += _T(" (");
		strEnable += TRL("recommended");
		strEnable += _T(")");

		CString strQ = TRL("Enable automatic update check?");

		CVistaTaskDialog dlg(hParent, hInstance, true);
		dlg.SetContent(strHdr);
		dlg.SetMainInstruction(strQ);
		dlg.SetWindowTitle(PWM_PRODUCT_NAME_SHORT);
		dlg.AddButton(strEnable, NULL, IDYES);
		dlg.AddButton(TRL("&Disable"), NULL, IDNO);
		dlg.SetIcon(MTDI_QUESTION);
		dlg.SetFooterText(strSub);
		dlg.SetFooterIcon(TD_INFORMATION_ICON);

		int dr = dlg.ShowDialog(NULL);
		if(dr < 0)
		{
			CString strMain = strHdr;
			strMain += _T("\r\n\r\n");
			strMain += strSub;
			strMain += _T("\r\n\r\n");
			strMain += strQ;

			dr = MessageBox(hParent, strMain, PWM_PRODUCT_NAME_SHORT,
				MB_YESNO | MB_ICONQUESTION);
		}

		*pCheck = (((dr == IDOK) || (dr == IDYES)) ? TRUE : FALSE);
	}
#endif

	*pConfig = TRUE;
}
