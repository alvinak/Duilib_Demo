#include "SecondFrameWnd.h"


CSecondFrameWnd::CSecondFrameWnd(void)
{
}


CSecondFrameWnd::~CSecondFrameWnd(void)
{
}

LPCTSTR CSecondFrameWnd::GetWindowClassName() const 
{ 
	return _T("SecondFrameWnd"); 
}

void    CSecondFrameWnd::Notify(TNotifyUI& msg) 
{
	if (msg.sType == _T("click")) {
		LPCTSTR pszCtrlName = msg.pSender->GetName().GetData();
		if (_tcscmp(pszCtrlName, _T("btn_sysmin")) == 0) {
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		} else if (_tcscmp(pszCtrlName, _T("btn_sysmax")) == 0) {
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			// ��󻯺ͻ�ԭ2��button֮����л�
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("btn_sysmax")));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("btn_restore")));
			if( pControl ) pControl->SetVisible(true);
		} else if (_tcscmp(pszCtrlName, _T("btn_restore")) == 0) {
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
			// ��󻯺ͻ�ԭ2��button֮����л�
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("btn_sysmax")));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("btn_restore")));
			if( pControl ) pControl->SetVisible(false);
		} else if (_tcscmp(pszCtrlName, _T("btn_sysclose")) == 0) {
			Close(IDOK);
		}
	}
}

LRESULT CSecondFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;

	if( uMsg == WM_CREATE ) {
		m_PaintManager.Init(m_hWnd);

		// �˴�ʱ��ͬ��
		//CControlUI *pWnd = new CButtonUI;
		//pWnd->SetName(_T("btnHello"));	// ���ÿؼ������ƣ�����������ڱ�ʶÿһ���ؼ�������Ψһ
		//pWnd->SetText(_T("Hello World")); // ��������
		//pWnd->SetBkColor(0xFF00FF00);     // ���ñ���ɫ
		
		CDialogBuilder builder;
		CControlUI* pWnd = builder.Create(_T("frame_wnd_demo\\second_frame.xml"), (UINT)0, NULL, &m_PaintManager);

		ASSERT(pWnd && "Failed to parse XML");

		m_PaintManager.AttachDialog(pWnd);
		// ��ӿؼ�����Ϣ��Ӧ��������Ϣ�ͻᴫ�ﵽduilib����Ϣѭ����
		// ���ǿ�����Notify����������Ϣ����
		m_PaintManager.AddNotifier(this);

		return lRes;
	}  
	// ����3����ϢWM_NCACTIVATE��WM_NCCALCSIZE��WM_NCPAINT��������ϵͳ������
	else if (uMsg == WM_NCACTIVATE) {
		if (!::IsIconic(m_hWnd)) {
			return (wParam == 0) ? true :false;
		}
	} else if (uMsg == WM_NCCALCSIZE) {
		return 0;
	} else if (uMsg == WM_NCPAINT) {
		return 0;
	}

	if( m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes) ) {
		return lRes;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}
