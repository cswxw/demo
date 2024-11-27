/////////////////////////////////////////////////////////////////////
#if !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
#define AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_
/////////////////////////////////////////////////////////////////////
// CaptureVideo.h : header file
/////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlbase.h>
#include <windows.h>
#include <Qedit.h>
#include <DShow.h>
#include <UUIDS.H>

#define  MYDEBUG   1
#ifndef srelease
#define srelease(x) \
	if ( NULL != x ) \
{ \
	x->Release( ); \
	x = NULL; \
}
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x ) \
	if ( NULL != x ) \
{ \
	x->Release( ); \
	x = NULL; \
}
#endif

#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)

//���ͺͿ����� ״̬
enum{
	CMD_CAN_COPY,
	CMD_CAN_SEND
};
//����һ�������Ķ���Ķ���
class CSampleGrabberCB : public ISampleGrabberCB 
{
       public:
           long       lWidth ; 
           long       lHeight ; 
            //CVdoFrameHandler * frame_handler ; 
            BOOL       bGrabVideo ; 
			BYTE* buf;         //λͼ����
			BYTE  state;       //1������copy ,2 ���Է���
        	DWORD bufSize;      //���ݴ�С
			BITMAPINFO	*lpbmi;     //λͼ�ļ�ͷ 
			BYTE*   m_lpFullBits;     //ͼ������
			DWORD   dwSize;             //���ݴ�С
			BOOL   bStact;           //0���Ը���  1���Է���

public:
       CSampleGrabberCB(){ 
            lWidth = 0 ; 
            lHeight = 0 ; 
             bGrabVideo = FALSE ; 
			 state=0;
			 lpbmi=NULL;
		     m_lpFullBits=NULL;
			 dwSize=0;
			 bStact=CMD_CAN_COPY;
//             frame_handler = NULL ; 
	   } 
        ~CSampleGrabberCB()
		{
			if (m_lpFullBits!=NULL)
			{
				delete[] m_lpFullBits;
			}
			if (lpbmi!=NULL)
			{
				delete[] lpbmi;
			}
		}
       STDMETHODIMP_(ULONG) AddRef() { return 2; }
       STDMETHODIMP_(ULONG) Release() { return 1; }

STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
	   
{
             if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ){ 
                     *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
                     return NOERROR;
			  } 
              return E_NOINTERFACE;
	   
}
STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample ) 
 {
             return 0;
	 
 }
	  //�ص����� ������õ� bmp ������
 STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
	  {
		  if (!pBuffer)return E_POINTER;
		 
		  if (bStact==CMD_CAN_COPY)         //δ��ʼ�� ���͵�ͬ�����һ��
		  {
               memcpy(m_lpFullBits,pBuffer,lBufferSize);     //������õ���Ƶ�����ݣ���Ϊʲô�ܵõ���Ƶ�������أ���
			                                                  //���ǵ�������������һ��
			   InterlockedExchange((LPLONG)&bStact,CMD_CAN_SEND);      //ԭ���������Է���  
		  }

		return 0;
}
LPBITMAPINFO ConstructBI(int biBitCount, bool m_bIsGray, int biWidth, int biHeight)
	 {
	 /*
	 biBitCount Ϊ1 (�ڰ׶�ɫͼ) ��4 (16 ɫͼ) ��8 (256 ɫͼ) ʱ����ɫ������ָ����ɫ���С
	 biBitCount Ϊ16 (16 λɫͼ) ��24 (���ɫͼ, ��֧��) ��32 (32 λɫͼ) ʱû����ɫ��
		 */
		 
		 int	color_num = biBitCount <= 8 ? 1 << biBitCount : 0;
		 
		 int nBISize = sizeof(BITMAPINFOHEADER) + (color_num * sizeof(RGBQUAD));
		 lpbmi = (BITMAPINFO *) new BYTE[nBISize];
		 
		 BITMAPINFOHEADER	*lpbmih = &(lpbmi->bmiHeader);
		 lpbmih->biSize = sizeof(BITMAPINFOHEADER);
		 lpbmih->biWidth = biWidth;
		 lpbmih->biHeight = biHeight;
		 lpbmih->biPlanes = 1;
		 lpbmih->biBitCount = biBitCount;
		 lpbmih->biCompression = BI_RGB;
		 lpbmih->biXPelsPerMeter = 0;
		 lpbmih->biYPelsPerMeter = 0;
		 lpbmih->biClrUsed = 0;
		 lpbmih->biClrImportant = 0;
		 lpbmih->biSizeImage = (((lpbmih->biWidth * lpbmih->biBitCount + 31) & ~31) >> 3) * lpbmih->biHeight;
		 
		 // 16λ���Ժ��û����ɫ��ֱ�ӷ���
		 
		 /////////////////////��ʼ����Ա����///////////////////////////////////////////////
		 dwSize=lpbmih->biSizeImage;
		 m_lpFullBits=new BYTE[dwSize+10];
		 ZeroMemory(m_lpFullBits,dwSize+10);
		 if (biBitCount >= 16)
			 return lpbmi;
			 /*
			 Windows 95��Windows 98�����lpvBits����ΪNULL����GetDIBits�ɹ��������BITMAPINFO�ṹ����ô����ֵΪλͼ���ܹ���ɨ��������
			 
			   Windows NT�����lpvBits����ΪNULL����GetDIBits�ɹ��������BITMAPINFO�ṹ����ô����ֵΪ��0���������ִ��ʧ�ܣ���ô������0ֵ��Windows NT�������ø��������Ϣ�������callGetLastError������
		 */
		 
		 HDC	hDC = GetDC(NULL);
		 HBITMAP hBmp = CreateCompatibleBitmap(hDC, 1, 1); // �߿���Ϊ0
		 GetDIBits(hDC, hBmp, 0, 0, NULL, lpbmi, DIB_RGB_COLORS);
		 ReleaseDC(NULL, hDC);
		 DeleteObject(hBmp);
		 
		 if (m_bIsGray)
		 {
			 for (int i = 0; i < color_num; i++)
			 {
				 int color = RGB2GRAY(lpbmi->bmiColors[i].rgbRed, lpbmi->bmiColors[i].rgbGreen, lpbmi->bmiColors[i].rgbBlue);
				 lpbmi->bmiColors[i].rgbRed = lpbmi->bmiColors[i].rgbGreen = lpbmi->bmiColors[i].rgbBlue = color;
			 }
		 }
		 return lpbmi;	
}

LPBITMAPINFO GetBmpInfo()	  
{
	if (lpbmi==NULL)
	{
		lpbmi=ConstructBI(24, 0, lWidth, lHeight);
	}
    return lpbmi;	  
}
BYTE* GetNextScreen(DWORD &nSize)       //�õ���һ֡���� 
{
		  nSize=dwSize;
		  return (BYTE*)m_lpFullBits;      //���Ǻܼ� ���Ƿ���һ����������ָ�룬�����ָ������ô��ȡ����Ƶ���ݵ��أ�
		  //���ǹ���������һ�°�
	 
}	 
};
class CSampleGrabberCB;
class CCaptureVideo 
{
	friend class CSampleGrabberCB;public:
		DWORD GetBufferSize();
		LPBYTE GetDIB(DWORD& nSize);
		LPBITMAPINFO GetBmpInfo();
		void Init();
//	void GrabVideoFrames(BOOL bGrabVideoFrames, CVdoFrameHandler * frame_handler); 
    HRESULT Open(int iDeviceID,int iPress);
	HRESULT Close(); 
    int EnumDevices(BYTE* buf);
    CCaptureVideo();
    virtual ~CCaptureVideo();protected:
    HWND      m_hWnd;
    IGraphBuilder *    m_pGB;
    ICaptureGraphBuilder2* m_pCapture;
    IBaseFilter*    m_pBF;
    IMediaControl*    m_pMC;
    IVideoWindow*    m_pVW;
    ISampleGrabber*    m_pGrabber;protected:
    void FreeMediaType(AM_MEDIA_TYPE& mt);
    bool BindVideoFilter(int deviceId, IBaseFilter **pFilter);
    void ResizeVideoWindow();
    HRESULT SetupVideoWindow();
    HRESULT InitCaptureGraphBuilder();
	IPin* FindPin(IBaseFilter *pFilter, PIN_DIRECTION dir);
public:
	BYTE* EnumCompress();
	void SendEnd();                //���ͽ���
};
#endif // !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
