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
				::MessageBox(NULL, _T("我是按钮"), _T("点击了按钮"), NULL);
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
			CControlUI* pRoot = builder.Create(_T("duilib.xml"), (UINT)0, NULL, &m_PaintManager);   // duilib.xml需要放到exe目录下
			ASSERT(pRoot && "Failed to parse XML");

			m_PaintManager.AttachDialog(pRoot);
			m_PaintManager.AddNotifier(this);   // 添加控件等消息响应，这样消息就会传达到duilib的消息循环，我们可以在Notify函数里做消息处理
			return lRes;
		}
		// 以下3个消息WM_NCACTIVATE、WM_NCCALCSIZE、WM_NCPAINT用于屏蔽系统标题栏
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
	void OnOptSelChanged(TNotifyUI& msg){  //指针类型
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
//结构定义
struct DUI_MSGMAP_ENTRY //定义一个结构体，来存放消息信息
{
CDuiString sMsgType;          // DUI消息类型
CDuiString sCtrlName;         // 控件名称
UINT       nSig;              // 标记函数指针类型
DUI_PMSG   pfn;               // 指向函数的指针
};
*/
/*
#define DUI_ON_MSGTYPE(msgtype, memberFxn) 
		{ msgtype, _T(""), DuiSig_vn, (DUI_PMSG)&memberFxn},   //DuiSig_vn,      // void (TNotifyUI)               
*/
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnOptSelChanged)

/*
//声明结束
#define DUI_END_MESSAGE_MAP()                                             \
	{ _T(""), _T(""), DuiSig_end, (DUI_PMSG)0 }                           \
};
*/
DUI_END_MESSAGE_MAP()




int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());   // 设置资源的默认路径（此处设置为和exe在同一目录）

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowModal();
	return 0;
}