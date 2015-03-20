#include "stdafx.h"
#include <Windows.h>
#include <atlstr.h>
#include <WinGDI.h>
#include <WinBase.h>
#include <WinUser.h>
#include <WinDef.h>
#include <GdiPlus.h>
#include <GdiPlusHeaders.h>
#pragma once
//#pragma    comment(lib,"User32.lib")
using namespace Gdiplus;
void SaveCapturedBitmap(HBITMAP hCaptureBitmap);
void CaptureScreen();
bool SaveBmp(HBITMAP hBitmap,CString FileName);
void useGdi();
int GetCodecClsid(const WCHAR* format, CLSID* pClsid);