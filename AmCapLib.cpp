#include "AmCapLib.h"
#include <windows.h>
#include <map>
#include <string>
using namespace std;
#include "amcap.h"
#include "streams.h"

HANDLE	hThreadMain = 0;

extern HINSTANCE ghInstApp;
extern DWORD ghwndStyle;
extern HWND ghwndApp, ghwndStatus;
extern map<wstring, IMoniker*>	gMapVideoMoniker;
extern TCHAR	gCamName[1024];
extern TCHAR	gMicName[1024];
extern int gnPox;
extern int gnPoy;


unsigned long __stdcall AMCapMainThread(void*)
{
	WinMain(ghInstApp, NULL, NULL, SW_SHOWDEFAULT);
	return 0;
}

bool StartDllWork(bool bStart) {
	if (bStart) {
		if (hThreadMain) {	// 避免重复启动
			return true;
		}
		hThreadMain = ::CreateThread(NULL, 0, AMCapMainThread, NULL, 0, NULL);
		if (hThreadMain) {
			int nTimes = 0;
			while (ghwndApp == NULL || !::IsWindow(ghwndApp)) {
				Sleep(10);
				if (nTimes++ >= 100) // 最多等待1秒
					break;
			}
		}
		return hThreadMain != 0;
	}
	else {
		if (hThreadMain) {
			::PostMessage(ghwndApp, WM_CLOSE, 0, 0);
			DWORD dwRt = ::WaitForSingleObject(hThreadMain, 3000);
			if (dwRt == WAIT_TIMEOUT) {
				//PLOG("wait thread exit time out: %lu, %p", timeout, this);
			}
			::CloseHandle(hThreadMain);
			hThreadMain = NULL;
			return true;
		}
	}
	return false;
}

void ModifyAMCap(bool bInit)
{
	if (bInit) {
		//ghwndStyle = (WS_OVERLAPPED | /*WS_CAPTION | WS_SYSMENU |*/ WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE);
		ghwndStyle = (WS_OVERLAPPED | /*WS_CAPTION |*/ WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	}
	else {
		ShowWindow(ghwndStatus, SW_HIDE);
		SetMenu(ghwndApp, NULL);
	}
}

/******************************接口****************************/
enum AMCmdType {
	cmd_startApp = 1,
	cmd_exitApp,
	cmd_startPreview,
	cmd_stopPreview,
	cmd_isPreview,
};

struct AMCmdPar
{
	TCHAR camName[1024];
	TCHAR micName[1024];
	int nPar0;
	int nPar1;
};

bool DoCommand(int cmd, void* pAMCmdPar)
{
	AMCmdType cmdtype = (AMCmdType)cmd;
	AMCmdPar* pPar = (AMCmdPar*)pAMCmdPar;
	wcscpy(gCamName, pPar->camName);
	wcscpy(gMicName, pPar->micName);
	gnPox = pPar->nPar0;
	gnPoy = pPar->nPar1;

	bool bReturn = false;
	switch (cmdtype)
	{
	case cmd_startApp:
		bReturn = StartDllWork(true);
		break;
	case cmd_exitApp:
		bReturn = StartDllWork(false);
		break;
	case cmd_startPreview:
		if (ghwndApp && !IsWindowVisible(ghwndApp)) {
			SelectDevices(gCamName, gMicName);
			PostMessage(ghwndApp, WM_COMMAND, MENU_PREVIEW, 0);
		}
		break;
	case cmd_stopPreview:
		if (ghwndApp && IsWindowVisible(ghwndApp)) {
			PostMessage(ghwndApp, WM_COMMAND, MENU_PREVIEW, 0);
		}
		break;
	case cmd_isPreview:
		bReturn = IsWindowVisible(ghwndApp);
		break;
	default:
		break;
	}
	return bReturn;
}
