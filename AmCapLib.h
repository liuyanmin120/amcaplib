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
	AMCAP_EXPORT bool DoCommand(int cmd, void* pAMCmdPar);
	

}

enum DIRECTION {
	UPPER = 0,
	LOWER = 1,
	LEFT,
	RIGHT,
	LEFTUPPER,
	LEFTLOWER,
	RIGHTLOWER,
	RIGHTUPPER,
	NONE
};

typedef struct _AppParam
{
	_AppParam()
	{
		rLimitRect = { -MAXINT, -MAXINT, MAXINT, MAXINT };
		bSetPos = false;
		rStartRect = { 300,300,320,240 };
		eDirection = NONE;
		szMinSize = { 32,32 };
		nBorder = 4;
		bMouseIsPressed = false;
	}
	RECT  rLimitRect;
	bool  bSetPos;
	RECT  rStartRect;
	DIRECTION eDirection;
	SIZE   szMinSize;
	bool	  bMouseIsPressed;
	int		  nBorder;
}AppParam;
extern AppParam g_AppParam;