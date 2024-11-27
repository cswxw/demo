#include "main.h"
#include "UIGroupBox.h"
/*
class  CDuiFrameWnd : public CWindowWnd, public INotifyUI
{
public:
	virtual LPCTSTR GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual void    Notify(TNotifyUI& msg)
	{
		if (msg.sType == _T("click"))
		{
			if (msg.pSender->GetName() == _T("btnHello"))
			{
				::MessageBox(NULL, _T("���ǰ�ť"), _T("����˰�ť"), NULL);
			}
		}
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;

		if (uMsg == WM_CREATE)
		{
			m_PaintManager.Init(m_hWnd);

			CDialogBuilder builder;
			CControlUI* pRoot = builder.Create(_T("duilib.xml"), (UINT)0, NULL, &m_PaintManager);   // duilib.xml��Ҫ�ŵ�exeĿ¼��
			ASSERT(pRoot && "Failed to parse XML");

			m_PaintManager.AttachDialog(pRoot);
			m_PaintManager.AddNotifier(this);   // ��ӿؼ�����Ϣ��Ӧ��������Ϣ�ͻᴫ�ﵽduilib����Ϣѭ�������ǿ�����Notify����������Ϣ����
			return lRes;
		}
		// ����3����ϢWM_NCACTIVATE��WM_NCCALCSIZE��WM_NCPAINT��������ϵͳ������
		else if (uMsg == WM_NCACTIVATE)
		{
			if (!::IsIconic(m_hWnd))
			{
				return (wParam == 0) ? TRUE : FALSE;
			}
		}
		else if (uMsg == WM_NCCALCSIZE)
		{
			return 0;
		}
		else if (uMsg == WM_NCPAINT)
		{
			return 0;
		}

		if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))
		{
			return lRes;
		}

		return __super::HandleMessage(uMsg, wParam, lParam);
	}

protected:
	CPaintManagerUI m_PaintManager;
};

*/
class CDuiFrameWnd : public WindowImplBase
{

	/*
	#define DUI_DECLARE_MESSAGE_MAP()                                         
		private:                                                                  
			static const DUI_MSGMAP_ENTRY _messageEntries[];                      
		protected:                                                                
			static const DUI_MSGMAP messageMap;                                   
			static const DUI_MSGMAP* PASCAL _GetBaseMessageMap();                 
			virtual const DUI_MSGMAP* GetMessageMap() const;                      
	*/
	DUI_DECLARE_MESSAGE_MAP();
public:
	virtual LPCTSTR    GetWindowClassName() const   { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile()                { return _T("duilib.xml"); }
	virtual CDuiString GetSkinFolder()              { return _T(""); }
	void OnOptSelChanged(TNotifyUI& msg){  //ָ������
		CDuiString name = msg.pSender->GetName();
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TabLayoutMain")));
		if (!name.Compare(_T("Option01"))){
			pControl->SelectItem(0);
		}
		else if (!name.Compare(_T("Option02"))){
			pControl->SelectItem(1);
		}

	}
	CControlUI* CreateControl(LPCTSTR pstrClass){
		if (_tcscmp(pstrClass, _T("GroupBox")) == 0)
		{
			return new CGroupBoxUI;
		}
		return NULL;

	}

	void Notify(TNotifyUI& msg){
		WindowImplBase::Notify(msg);
		if (msg.sType == _T("click")){
			if (msg.pSender->GetName() == _T("btn_updateLayout"))
			{
				//m_PaintManager.ReloadSkin();

			}
		}
	}
};
/*
#define DUI_BEGIN_MESSAGE_MAP(theClass, baseClass)                        \
const DUI_MSGMAP* PASCAL theClass::_GetBaseMessageMap()  { return &baseClass::messageMap; }                                \
const DUI_MSGMAP* theClass::GetMessageMap() const        { return &theClass::messageMap; }                                 \
UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
{ &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] }; \
UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =     \
{
*/
DUI_BEGIN_MESSAGE_MAP(CDuiFrameWnd, WindowImplBase)
/*
//�ṹ����
struct DUI_MSGMAP_ENTRY //����һ���ṹ�壬�������Ϣ��Ϣ
{
CDuiString sMsgType;          // DUI��Ϣ����
CDuiString sCtrlName;         // �ؼ�����
UINT       nSig;              // ��Ǻ���ָ������
DUI_PMSG   pfn;               // ָ������ָ��
};
*/
/*
#define DUI_ON_MSGTYPE(msgtype, memberFxn) 
		{ msgtype, _T(""), DuiSig_vn, (DUI_PMSG)&memberFxn},   //DuiSig_vn,      // void (TNotifyUI)               
*/
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnOptSelChanged)

/*
//��������
#define DUI_END_MESSAGE_MAP()                                             \
	{ _T(""), _T(""), DuiSig_end, (DUI_PMSG)0 }                           \
};
*/
DUI_END_MESSAGE_MAP()




int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());   // ������Դ��Ĭ��·�����˴�����Ϊ��exe��ͬһĿ¼��

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowModal();
	return 0;
}