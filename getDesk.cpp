#include "stdafx.h"
#include "getDesk.h"
void SaveCapturedBitmap(HBITMAP hCaptureBitmap){
	SaveBmp(hCaptureBitmap,   "testRemotDeskBMP.bmp");
}
//VC下把HBITMAP保存为bmp图片 
bool  SaveBmp(HBITMAP hBitmap,CString FileName)       
{       
	HDC     hDC;       
	//当前分辨率下每象素所占字节数       
	int     iBits;       
	//位图中每象素所占字节数       
	WORD     wBitCount;       
	//定义调色板大小，     位图中像素字节大小     ，位图文件大小     ，     写入文件字节数           
	DWORD     dwPaletteSize=0,   dwBmBitsSize=0,   dwDIBSize=0,   dwWritten=0;           
	//位图属性结构           
	BITMAP     Bitmap;               
	//位图文件头结构       
	BITMAPFILEHEADER     bmfHdr;               
	//位图信息头结构           
	BITMAPINFOHEADER     bi;               
	//指向位图信息头结构               
	LPBITMAPINFOHEADER     lpbi;               
	//定义文件，分配内存句柄，调色板句柄           
	HANDLE     fh,   hDib,   hPal,hOldPal=NULL;           

	//计算位图文件每个像素所占字节数     
	hDC  = CreateDC(TEXT("DISPLAY"),   NULL,   NULL,   NULL);       
	iBits  = GetDeviceCaps(hDC,   BITSPIXEL)     *     GetDeviceCaps(hDC,   PLANES);           
	DeleteDC(hDC);           
	if(iBits <=  1)                                                 
		wBitCount = 1;           
	else  if(iBits <=  4)                             
		wBitCount  = 4;           
	else if(iBits <=  8)                             
		wBitCount  = 8;           
	else                                                                                                                             
		wBitCount  = 24;           

	GetObject(hBitmap,   sizeof(Bitmap),   (LPSTR)&Bitmap);       
	bi.biSize= sizeof(BITMAPINFOHEADER);       
	bi.biWidth = Bitmap.bmWidth;       
	bi.biHeight =  Bitmap.bmHeight;       
	bi.biPlanes =  1;       
	bi.biBitCount = wBitCount;       
	bi.biCompression= BI_RGB;       
	bi.biSizeImage=0;       
	bi.biXPelsPerMeter = 0;       
	bi.biYPelsPerMeter = 0;       
	bi.biClrImportant = 0;       
	bi.biClrUsed =  0;       

	dwBmBitsSize  = ((Bitmap.bmWidth *wBitCount+31) / 32)*4* Bitmap.bmHeight;       

	//为位图内容分配内存           
	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));           
	lpbi  = (LPBITMAPINFOHEADER)GlobalLock(hDib);           
	*lpbi  = bi;           

	//     处理调色板               
	hPal  = GetStockObject(DEFAULT_PALETTE);           
	if (hPal)           
	{           
		hDC  = ::GetDC(NULL);           
		hOldPal = ::SelectPalette(hDC,(HPALETTE)hPal, FALSE);           
		RealizePalette(hDC);           
	}       

	//     获取该调色板下新的像素值           
	GetDIBits(hDC,hBitmap, 0,(UINT)Bitmap.bmHeight,
		(LPSTR)lpbi+ sizeof(BITMAPINFOHEADER)+dwPaletteSize, 
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);           

	//恢复调色板               
	if (hOldPal)           
	{           
		::SelectPalette(hDC,   (HPALETTE)hOldPal,   TRUE);           
		RealizePalette(hDC);           
		::ReleaseDC(NULL,   hDC);           
	}           

	//创建位图文件               
	fh  = CreateFile(FileName,   GENERIC_WRITE,0,   NULL,   CREATE_ALWAYS,         
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,   NULL);           

	if (fh     ==  INVALID_HANDLE_VALUE)         return     FALSE;           

	//     设置位图文件头           
	bmfHdr.bfType  = 0x4D42;     //     "BM"           
	dwDIBSize  = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;               
	bmfHdr.bfSize  = dwDIBSize;           
	bmfHdr.bfReserved1  = 0;           
	bmfHdr.bfReserved2  = 0;           
	bmfHdr.bfOffBits  = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;           
	//     写入位图文件头           
	WriteFile(fh,   (LPSTR)&bmfHdr,   sizeof(BITMAPFILEHEADER),   &dwWritten,   NULL);           
	//     写入位图文件其余内容           
	WriteFile(fh,   (LPSTR)lpbi,   dwDIBSize,   &dwWritten,   NULL);           
	//清除               
	GlobalUnlock(hDib);           
	GlobalFree(hDib);           
	CloseHandle(fh);           

	return     TRUE;       
}

void CaptureScreen()
{
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	HBITMAP hCaptureBitmap =CreateCompatibleBitmap(hDesktopDC, 
		nScreenWidth, nScreenHeight);
	SelectObject(hCaptureDC,hCaptureBitmap); 
	BitBlt(hCaptureDC,0,0,nScreenWidth,nScreenHeight,hDesktopDC,0,0,SRCCOPY); 
	SaveCapturedBitmap(hCaptureBitmap); //Place holder - Put your code
	//here to save the captured image to disk
	ReleaseDC(hDesktopWnd,hDesktopDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);
}
void useGdi(){
	CDC * pDeskDC = GetDesktopWindow()->GetDC();
	CRect rc;
	GetDesktopWindow()->GetClientRect(rc);
	CDC memDC;
	memDC.CreateCompatibleDC(pDeskDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDeskDC,rc.Width(),rc.Height());
	memDC.SelectObject(&bmp);
	BITMAP bitmap;
	bmp.GetBitmap(&bitmap);
	int panelsize = 0;
	if(bitmap.bmBitsPixel<16){
		panelsize = pow(2,bitmap.bmBitsPixel*sizeof(RGBQUAD));
	}
	BITMAPINFO * pBInfo = (BITMAPINFO*)LocalAlloc(LPTR,sizeof(BITMAPINFO)+panelsize);
	pBInfo->bmiHeader.biBitCount = bitmap.bmBitsPixel;
	pBInfo->bmiHeader.biClrImportant = 0;
	pBInfo->bmiHeader.biCompression = 0;
	pBInfo->bmiHeader.biHeight = bitmap.bmHeight;
	pBInfo->bmiHeader.biPlanes = bitmap.bmPlanes;
	pBInfo->bmiHeader.biSize = sizeof(BITMAPINFO);

	pBInfo->bmiHeader.biSizeImage = bitmap.bmWidthBytes*bitmap.bmHeight;
	pBInfo->bmiHeader.biWidth = bitmap.bmWidth;
	pBInfo->bmiHeader.biXPelsPerMeter = 0;
	pBInfo->bmiHeader.biYPelsPerMeter = 0;

	memDC.BitBlt(0,0,bitmap.bmWidth,bitmap.bmHeight,pDeskDC,0,0,SRCCOPY);

	char * pData = new char[bitmap.bmWidthBytes * bitmap.bmHeight];
	::GetDIBits(memDC.m_hDC,bmp,0,bitmap.bmHeight,pData,pBInfo,DIB_RGB_COLORS);
	int BufSize = panelsize + sizeof(BITMAPINFO) + bitmap.bmWidthBytes*bitmap.bmHeight;
	Bitmap * mmage;
	mmage = Bitmap::FromBITMAPINFO(pBInfo,pData);
	CLSID clsid;
	GetCodecClsid(L"image/jpeg",&clsid);
	HGLOBAL m_hMem = GlobalAlloc(GMEM_MOVEABLE,0);
	IStream * pstm = NULL;
	CreateStreamOnHGlobal(m_hMem,TRUE,&pstm);
	mmage->Save(pstm,&clsid,NULL);
	mmage->Save(L"test.jpg",&clsid,NULL);
	int m_JpegSize = GlobalSize(m_hMem);
	LPBYTE lpData = (LPBYTE)GlobalLock(m_hMem);
	memDC.DeleteDC();
	pDeskDC->DeleteDC();
	pstm->Release();
	if (mmage)
	{
		delete mmage;
	}
	delete [] pData;
	GlobalUnlock(m_hMem);
	GlobalFree(m_hMem);
	::LocalFree((HGLOBAL)pBInfo);
	bmp.DeleteObject();
}
int GetCodecClsid(const WCHAR* format, CLSID* pClsid)  
{  
	UINT  codenum = 0;     
	UINT  size = 0;     
	ImageCodecInfo * pImageCodecInfo = NULL;  
	GetImageEncodersSize(&codenum, &size);  
	if(size == 0)  
		return -1 ;    
	pImageCodecInfo = new ImageCodecInfo[size];  
	if(pImageCodecInfo == NULL)  
		return -1;   
	GetImageEncoders(codenum, size, pImageCodecInfo);  
	for(UINT j = 0; j < codenum; ++j)  
	{  
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )  
		{  
			*pClsid = pImageCodecInfo[j].Clsid;  
			delete []pImageCodecInfo;  
			return 0;    
		}      

	}   
	delete []pImageCodecInfo;  
	return -1;    
}