void CGDI_TestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
 // TODO: Add extra validation here  
    HDC hDesktopDC = ::GetDC(NULL);  
    HDC hTmpDC = CreateCompatibleDC(hDesktopDC);  
    HBITMAP hBmp = CreateCompatibleBitmap(hDesktopDC, 351, 250);    //351x250, 示例数据  
    SelectObject(hTmpDC, hBmp);  
    BitBlt(hTmpDC, 0, 0, 351, 250, hDesktopDC, 0, 0, SRCCOPY);  
    DeleteObject(hTmpDC);  
  
    BITMAP bm;  
    PBITMAPINFO bmpInf;  
    if(GetObject(hBmp,sizeof(bm),&bm)==0)  
    {  
        ::ReleaseDC(NULL,hDesktopDC);  
        return ;  
    }  
  
    int nPaletteSize=0;  
    if(bm.bmBitsPixel<16)  
        nPaletteSize=(int)pow(2.0f,(int)bm.bmBitsPixel);  
  
    bmpInf=(PBITMAPINFO)LocalAlloc(LPTR,sizeof(BITMAPINFOHEADER)+  
        sizeof(RGBQUAD)*nPaletteSize+(bm.bmWidth+7)/8*bm.bmHeight*bm.bmBitsPixel);  
  
    BYTE* buf=((BYTE*)bmpInf) +   
        sizeof(BITMAPINFOHEADER)+  
        sizeof(RGBQUAD)*nPaletteSize;  
  
    //-----------------------------------------------  
    bmpInf->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);  
    bmpInf->bmiHeader.biWidth = bm.bmWidth;  
    bmpInf->bmiHeader.biHeight = bm.bmHeight;  
    bmpInf->bmiHeader.biPlanes = bm.bmPlanes;  
    bmpInf->bmiHeader.biBitCount = bm.bmBitsPixel;  
    bmpInf->bmiHeader.biCompression = BI_RGB;  
    bmpInf->bmiHeader.biSizeImage = (bm.bmWidth+7)/8*bm.bmHeight*bm.bmBitsPixel;  
    //-----------------------------------------------  
  
    if(!::GetDIBits(hDesktopDC,hBmp,0,(UINT)bm.bmHeight,buf,bmpInf,DIB_RGB_COLORS))  
    {  
        ::ReleaseDC(NULL,hDesktopDC);  
        LocalFree(bmpInf);  
        return ;  
    }  
  
    ::ReleaseDC(NULL,hDesktopDC);  
  
    CString sMsg;  
    sMsg.Format("BitsPixel:%d,width:%d,height:%d",  
        bm.bmBitsPixel,bm.bmWidth,bm.bmHeight);  
    //AfxMessageBox(sMsg);  
  
    //CClientDC dc(this);  
	HDC dc = ::GetWindowDC(this->GetSafeHwnd());
  
    int nOffset;  
    BYTE r,g,b;  
    int nWidth = bm.bmWidth*bm.bmBitsPixel/8;  
    nWidth = ((nWidth+3)/4)*4; //4字节对齐  
      
    if(bmpInf->bmiHeader.biBitCount == 8)  
    {         
        for(int i=0; i<bm.bmHeight; i++)  
        {  
            for(int j=0; j<bm.bmWidth; j++)  
            {  
                RGBQUAD rgbQ;  
                rgbQ = bmpInf->bmiColors[buf[i*nWidth+j]];  
                //dc.SetPixel(j,bm.bmHeight-i,RGB(rgbQ.rgbRed,rgbQ.rgbGreen,rgbQ.rgbBlue)); //测试显示  
				::SetPixel(dc,j,bm.bmHeight-i,RGB(rgbQ.rgbRed,rgbQ.rgbGreen,rgbQ.rgbBlue)); //测试显示  
            }  
        }  
    }  
    else if(bmpInf->bmiHeader.biBitCount == 16)  
    {  
        for(int i=0; i<bm.bmHeight; i++)  
        {  
            nOffset = i*nWidth;  
            for(int j=0; j<bm.bmWidth; j++)  
            {  
                b = buf[nOffset+j*2]&0x1F;  
                g = buf[nOffset+j*2]>>5;  
                g |= (buf[nOffset+j*2+1]&0x03)<<3;  
                r = (buf[nOffset+j*2+1]>>2)&0x1F;  
  
                r *= 8;  
                b *= 8;  
                g *= 8;  
                  
                //dc.SetPixel(j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
				::SetPixel(dc,j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
            }  
        }  
    }  
    else if(bmpInf->bmiHeader.biBitCount == 24)  
    {  
        for(int i=0; i<bm.bmHeight; i++)  
        {  
            nOffset = i*nWidth;  
            for(int j=0; j<bm.bmWidth; j++)  
            {  
                b = buf[nOffset+j*3];  
                g = buf[nOffset+j*3+1];  
                r = buf[nOffset+j*3+2];  
                  
                //dc.SetPixel(j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
				SetPixel(dc,j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
            }  
        }  
    }  
    else if(bmpInf->bmiHeader.biBitCount == 32)  
    {  
#if 0
		//从下往上 从左往右边
        for(int i=0; i<bm.bmHeight; i++)  
        {  
            nOffset = i*nWidth;  
            for(int j=0; j<bm.bmWidth; j++)  
            {  
                b = buf[nOffset+j*4];  
                g = buf[nOffset+j*4+1];  
                r = buf[nOffset+j*4+2];  
                  
                //dc.SetPixel(j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
				SetPixel(dc,j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
            }  
        }  
#else
		
        for(int i=bm.bmHeight; i>0; i--)  
        {    
			nOffset = i * nWidth;  
            for(int j=0; j<bm.bmWidth; j++)  
            {
				

                b = (BYTE)(buf[nOffset+j*4] * 0.11);  
                g = (BYTE)(buf[nOffset+j*4+1] * 0.59);  
                r = (BYTE)(buf[nOffset+j*4+2] * 0.3);  
				int temp = RGB(r,g,b);
				if(temp>128){
					//r=g=b=255;
				}
				else
					r=g=b=0;
				
                  
                //dc.SetPixel(j, bm.bmHeight-i, RGB(r,g,b)); //测试显示  
				SetPixel(dc,j, bm.bmHeight-i, RGB(r,g,b)); //测试显示
            }  
        }  
#endif
    }  
  
    DeleteObject(hBmp);  
    LocalFree(bmpInf);  
}


