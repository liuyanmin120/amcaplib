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

unsigned long __stdcall AMCapMainThread(void*)
{
	WinMain(ghInstApp, NULL, NULL, SW_SHOWDEFAULT);
	return 0;
}

bool StartDllWork(bool bStart) {
	if (bStart) {
		if (GetThreadId(hThreadMain) != 0) {	// 避免重复启动
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
		if (GetThreadId(hThreadMain) != 0) {
			::PostMessage(ghwndApp, WM_CLOSE, 0, 0);
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
		ghwndStyle = (WS_POPUP | /*WS_CAPTION |*/ WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	}
	else {
		ShowWindow(ghwndStatus, SW_HIDE);
		SetMenu(ghwndApp, NULL);
	}
}

/******************************接口****************************/
enum CmdType {
	cmd_startApp = 1,
	cmd_exitApp,
	cmd_startPreview,
	cmd_stopPreview,
	cmd_isPreview,
};
bool DoCommand(int cmd)
{
	CmdType cmdtype = (CmdType)cmd;

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
			TCHAR szVideoDisplayName[1024], szAudioDisplayName[1024];
			*szAudioDisplayName = *szVideoDisplayName = 0; // null terminate
			GetProfileString(TEXT("annie"), TEXT("VideoDevice2"), TEXT(""),
				szVideoDisplayName, NUMELMS(szVideoDisplayName));
			GetProfileString(TEXT("annie"), TEXT("AudioDevice2"), TEXT(""),
				szAudioDisplayName, NUMELMS(szAudioDisplayName));
			SelectDevices(szVideoDisplayName, szAudioDisplayName);
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
