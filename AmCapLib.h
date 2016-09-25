#pragma once

#if defined(AMCAP_EXPORT)
#define AMCAP_EXPORT     __declspec(dllimport)
#else
#define AMCAP_EXPORT     __declspec(dllexport)
#endif

#include <windows.h>


bool SelectDevices(TCHAR *szVideo, TCHAR *szAudio);
void ModifyAMCap(bool bInit);
bool StartDllWork(bool bStart);

extern "C" {
	AMCAP_EXPORT bool DoCommand(int cmd);
	

}