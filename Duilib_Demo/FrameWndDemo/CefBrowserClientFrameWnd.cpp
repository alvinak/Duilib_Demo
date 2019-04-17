#include "CefBrowserClientFrameWnd.h"

#include "cef_simple/simple_app.h"
#include "cef_simple/simple_handler.h"

#include "base/control/UICefBrowser.h"


//////////////////////////////////////////////////////////////////////////
CCefBrowserClientFrameWnd::CCefBrowserClientFrameWnd(void)
{

}

CCefBrowserClientFrameWnd::~CCefBrowserClientFrameWnd(void)
{

}

void CCefBrowserClientFrameWnd::OnPrepare()
{

}

CDuiString CCefBrowserClientFrameWnd::GetSkinFile()
{
	return _T("frame_wnd_demo\\cef_browser_frame_client.xml");
}

void CCefBrowserClientFrameWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("windowinit"))
		OnPrepare();
	else if (msg.sType == _T("click"))
	{
		LPCTSTR pszCtrlName = msg.pSender->GetName().GetData();
		if (_tcscmp(pszCtrlName, _T("btn_sysmin")) == 0)
		{
			Minimize();
		}
		if (_tcscmp(pszCtrlName, _T("btn_sysmax")) == 0)
		{
			Maximize();
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("btn_sysmax")));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("btn_restore")));
			if( pControl ) pControl->SetVisible(true);
		}
		if (_tcscmp(pszCtrlName, _T("btn_restore")) == 0)
		{
			Restore();
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("btn_sysmax")));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("btn_restore")));
			if( pControl ) pControl->SetVisible(false);
		}
		else if (_tcscmp(pszCtrlName, _T("btn_sysclose")) == 0)
		{
			Close(IDOK);
		}
		else if (_tcscmp(pszCtrlName, _T("btn_ok")) == 0)
		{

		}
		else if (_tcscmp(pszCtrlName, _T("btn_back")) == 0)
		{
			if (m_pBrowserUI && m_pBrowserUI->CanGoBack())
				m_pBrowserUI->GoBack();
		}
		else if (_tcscmp(pszCtrlName, _T("btn_forward")) == 0)
		{
			if (m_pBrowserUI && m_pBrowserUI->CanGoForward())
				m_pBrowserUI->GoForward();
		}
		else if (_tcscmp(pszCtrlName, _T("btn_refresh")) == 0
			|| _tcscmp(pszCtrlName, _T("btn_refresh_new")) == 0)
		{
			if (m_pBrowserUI)
				m_pBrowserUI->Reload();
		}
		else if (_tcscmp(pszCtrlName, _T("btn_stop")) == 0)
		{
			if (m_pBrowserUI)
				m_pBrowserUI->StopLoad();
		}
		else if (_tcscmp(pszCtrlName, _T("btn_run_js")) == 0)
		{
			if (m_pBrowserUI)
				m_pBrowserUI->RunJs(_T("alert('ExecuteJavaScript works!  This is test');"));
		}
	}
	else if (msg.sType == _T("return"))
	{
		LPCTSTR pszCtrlName = msg.pSender->GetName().GetData();
		if (_tcscmp(pszCtrlName, _T("edit_address")) == 0)
		{
			wstring strUrl = msg.pSender->GetText().GetData();
			if (strUrl.length() == 0){
				return;
			}

			if (strUrl.length() < 5){
				strUrl = L"http://" + strUrl;
			}
			else{
				wstring start = strUrl.substr(0,5);
				transform(start.begin(), start.end(), start.begin(), toupper);

				if (start != L"HTTP:" && start != L"HTTPS"){
					strUrl = L"http://" + strUrl;
				}
			}

			LoadUrl(strUrl);	
		}
	} 
	else if (_tcsicmp(msg.sType, _T("selectchanged")) == 0) {
		OnSelChanged(msg.pSender);
	}
}

void CCefBrowserClientFrameWnd::OnSelChanged(CControlUI* pSender)
{
	CTabLayoutUI* pTabTest = static_cast <CTabLayoutUI*>(m_pm.FindControl(_T("tabTest")));
	COptionUI *pCefControlTest = static_cast<COptionUI*>(m_pm.FindControl(_T("CEF_Control_Test")));
	if (pTabTest == NULL)
	{
		return;
	}
	CDuiString strSelName = pSender->GetName();
	if (strSelName == _T("CEF_Control_Test")) 
	{
		if (m_pBrowserUI) {
			m_pBrowserUI->Navigate2(_T("http://192.168.1.251/main"));
		}
		ShowCefWindow();
		pTabTest->SelectItem(0);
	} else if (strSelName == _T("OptionDemo2")) {
		HideCefWindow();
		pTabTest->SelectItem(1);
	}
}

CControlUI* CCefBrowserClientFrameWnd::CreateControl(LPCTSTR pstrClass/*, CPaintManagerUI *pManager*/)
{
	if( _tcsicmp(pstrClass, _T("Cef")) == 0){
		m_pBrowserUI = new CCefBrowserUI(this, _T(""));
		return m_pBrowserUI;
	}

	return NULL;
}

LRESULT CCefBrowserClientFrameWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	__super::OnSize(uMsg, wParam, lParam, bHandled);
	return 0;
}

void CCefBrowserClientFrameWnd::ShowCefWindow()
{	
	if (m_pBrowserUI != NULL)
	{
		m_pBrowserUI->SetVisible(true);
	}
}

void CCefBrowserClientFrameWnd::HideCefWindow()
{
	if (m_pBrowserUI != NULL)
	{
		m_pBrowserUI->SetVisible(false);
	}
}
void CCefBrowserClientFrameWnd::Minimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void CCefBrowserClientFrameWnd::Maximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
}

void CCefBrowserClientFrameWnd::Restore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
}

void CCefBrowserClientFrameWnd::SetMinInfo(int cx, int cy)
{
	m_pm.SetMinInfo(cx, cy);
}

void CCefBrowserClientFrameWnd::LoadUrl(wstring url)
{
	if (m_pBrowserUI){
		if (url.empty()) {
			std::wstring text = _T("address invalid");
			return;
		}
		
		m_pBrowserUI->Navigate2(url);
	}
}

wstring CCefBrowserClientFrameWnd::GetUrl()
{
	if (m_pBrowserUI){
		return m_pBrowserUI->GetCurrentUrl();
	}
	return L"";
}

wstring CCefBrowserClientFrameWnd::GetTitle()
{
	if (m_title_label){
		return m_title_label->GetText().GetData();
	}
	return L"";
}

LRESULT CCefBrowserClientFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg,wParam,lParam);
}

void CCefBrowserClientFrameWnd::InitWindow()
{
	m_pOkBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_ok")));
	m_pAddressEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("edit_address")));
	m_pBackBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_back")));
	m_pForwardBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_forward")));
	m_pRefreshBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_refresh")));
	m_pRefreshBtnNew = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_refresh_new")));
	m_pStopBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("btn_stop")));
	m_title_label = static_cast<CLabelUI*>(m_pm.FindControl(L"title_label"));
}

LRESULT CCefBrowserClientFrameWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32)
	BOOL bIconic = ::IsIconic(m_hWnd);
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed){

		bool isMaximized = !bZoomed;

		CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("btn_sysmax")));
		if( pControl ) pControl->SetVisible(!isMaximized);
		pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("btn_restore")));
		if( pControl ) pControl->SetVisible(isMaximized);
	}
	// ˫���ҷֱ��ʲ�ͬ�����������ָ�����ʱ��󻯳ߴ粻�ԣ�ʵ����С��Ļ�ķֱ��ʣ�
	// ԭ��WinImplBase����Ӧ����WM_GETMINMAXINFO��Ϣ���ڶ���MonitorFromWindow��ȡ������Ļ��ʹ����Ĭ�ϵ�����Ļ�ߴ����
	// Ӧ�������������������������ڳߴ粻�ԣ���Դ�����Restore��Maximize��ǿ����󻯴��ڵ���ȷ�ߴ�
	if (bIconic && (::IsIconic(m_hWnd) != bIconic)){
		// ����С������ԭ���ҵ�ǰ�Ѿ������
		if (::IsZoomed(m_hWnd)){
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		}
	} 
#else
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ICallbackDelegate events
void CCefBrowserClientFrameWnd::OnSetAddress(const std::wstring& url)
{
	if (m_pAddressEdit != nullptr){
		m_pAddressEdit->SetText(url.c_str());
	}
}

void CCefBrowserClientFrameWnd::OnSetTitle(const std::wstring& title) 
{
	if (m_title_label){
		m_title_label->SetText(title.c_str());
	}
	SetWindowText(m_hWnd, title.c_str());
}

void CCefBrowserClientFrameWnd::OnSetFullscreen(bool fullscreen) 
{
	if (fullscreen){
		Maximize();
	}else{
		Restore();
	}
}

void CCefBrowserClientFrameWnd::OnSetLoadingState(bool isLoading,
									  bool canGoBack,
									  bool canGoForward)
{
	if (m_pBackBtn){
		m_pBackBtn->SetEnabled(canGoBack);
	}
	if (m_pForwardBtn){
		m_pForwardBtn->SetEnabled(canGoForward);
	}
	if (m_pRefreshBtn){
		m_pRefreshBtn->SetEnabled(!isLoading);
	}
	if (m_pRefreshBtnNew){
		m_pRefreshBtnNew->SetEnabled(!isLoading);
	}
	if (m_pStopBtn){
		m_pStopBtn->SetEnabled(isLoading);
	}
}


//////////////////////////////////////////////////////////////////////////
// ICallbackDelegate methods
void CCefBrowserClientFrameWnd::OnSetDraggableRegions(
	const std::vector<CefDraggableRegion>& regions)
{

}

// Set load error message log
void CCefBrowserClientFrameWnd::OnLoadError(CefRefPtr<CefFrame> frame
								, int errorCode
								, const CefString& errorText
								, const CefString& failedUrl)
{

}

void CCefBrowserClientFrameWnd::OnMinimized()
{
	Minimize();
}

void CCefBrowserClientFrameWnd::OnExit()
{
	Close(IDOK);
}

void CCefBrowserClientFrameWnd::OnZoomValueChanged(int step)
{
	double value = m_pBrowserUI->GetZoomLevel(); 
	//value = value + step * 0.2;
	m_pBrowserUI->SetZoomLevel(value);
}

//////////////////////////////////////////////////////////////////////////

void CCefBrowserClientFrameWnd::NotifyWindowDestroyed()
{
	if (m_pDelegate)
		m_pDelegate->OnRootWindowDestroyed(this);
}

void CCefBrowserClientFrameWnd::SetBrowserZoomLevel(double level)
{
	if (m_pBrowserUI) {
		m_pBrowserUI->SetZoomLevel(level);
	}
}