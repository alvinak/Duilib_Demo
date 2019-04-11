#include "FirstFrameWnd.h"


CFirstFrameWnd::CFirstFrameWnd(void)
{
}


CFirstFrameWnd::~CFirstFrameWnd(void)
{
}

LPCTSTR CFirstFrameWnd::GetWindowClassName() const 
{ 
	return _T("FirstFrameWnd"); 
}

void    CFirstFrameWnd::Notify(TNotifyUI& msg) 
{

}

LRESULT CFirstFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;

	if( uMsg == WM_CREATE ) {
		CControlUI *pWnd = new CButtonUI;
		pWnd->SetName(_T("btnHello"));		// ���ÿؼ������ƣ�����������ڱ�ʶÿһ���ؼ�������Ψһ
		pWnd->SetText(_T("Hello World in first"));   // ��������
		pWnd->SetBkColor(0xFF00FF00);       // ���ñ���ɫ
		
		m_PaintManager.Init(m_hWnd);
		m_PaintManager.AttachDialog(pWnd);
		// ��ӿؼ�����Ϣ��Ӧ��������Ϣ�ͻᴫ�ﵽduilib����Ϣѭ����
		// ���ǿ�����Notify����������Ϣ����
		m_PaintManager.AddNotifier(this);
		return lRes;
	}  
	// ����3����ϢWM_NCACTIVATE��WM_NCCALCSIZE��WM_NCPAINT��������ϵͳ������
	//else if (uMsg == WM_NCACTIVATE) {
	//	if (!::IsIconic(m_hWnd)) {
	//		return (wParam == 0) ? true :false;
	//	}
	//} else if (uMsg == WM_NCCALCSIZE) {
	//	return 0;
	//} else if (uMsg == WM_NCPAINT) {
	//	return 0;
	//}

	if( m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes) ) {
		return lRes;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}
