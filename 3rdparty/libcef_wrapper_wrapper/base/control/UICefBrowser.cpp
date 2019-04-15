#include "stdafx.h"
#include "UICefBrowser.h"
#include "include/cef_parser.h"

#define ERROR_URL_PREFIX "http://client/html/error/"
#define ERR_CONNECTION_TIMED_OUT -118
// ����TerminationStatusö�ٶ����������������룬�μ�OnRenderProcessTerminated�ص� [12/29/2018 Modified by zhangjian]
#define ERR_RENDER_ABNORMAL_TERMINATION -10000
#define ERR_RENDER_PROCESS_WAS_KILLED -10001
#define ERR_RENDER_PROCESS_CRASHED -10002

CCefBrowserUI::CCefBrowserUI(ICallbackDelegate* callbackDelegate, const CefString& url)
	: m_pProcessMessageHandler(new CProcessMessageHandler)
	, m_pClientHandler(new CCefClientHandler(this))
	, m_CallbackDelegate(callbackDelegate)
	, m_sStartupUrl(url)
{
	//BOOL FLAGG = IsDragEnabled();
	//SetDragEnable(TRUE);
	//bool eis = IsMouseEnabled();
	//SetMouseEnabled(TRUE);
}

CCefBrowserUI::~CCefBrowserUI()
{
	if (m_pClientHandler != nullptr){
		m_pClientHandler->Close();
		if (m_pBrowser != nullptr)
		{
			m_pClientHandler->CloseBrowser(m_pBrowser);
			m_pBrowser = NULL;
		}
		m_pClientHandler = NULL;
	}
}

LPCTSTR CCefBrowserUI::GetClass() const {
	return CCefBrowserUIClassName;

}

LPVOID CCefBrowserUI::GetInterface(LPCTSTR pstrName) {
	if( _tcscmp(pstrName, CCefBrowserUIItemInterface) == 0 ) 
		return static_cast<CCefBrowserUI*>(this);  
	return CControlUI::GetInterface(pstrName);  
}

void CCefBrowserUI::DoInit()
{
	if (m_sStartupUrl.empty())
		m_sStartupUrl = _CEF_BLANK_;

	m_pClientHandler->CreateBrowser(m_pManager->GetPaintWindow(), m_rcItem, m_sStartupUrl);
}

bool CCefBrowserUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
	resize();

	return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
}

void CCefBrowserUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("url")) == 0) {
		// url����˳��CCefBrowserUI��ʼ�������url >> xmlģ����url��ֵ
		if (m_sStartupUrl.empty())
			m_sStartupUrl = pstrValue;
	}
	else return CControlUI::SetAttribute(pstrName, pstrValue);
}

void CCefBrowserUI::SetVisible(bool bVisible)
{
	if (m_pBrowser != nullptr)
	{
#ifndef _USED_CHROME_69_3497
        m_pBrowser->GetHost()->SetWindowVisibility(bVisible);
#endif
	}
	else
	{
		m_AfterCreatedCacheTasks.push([bVisible, this]{ SetVisible(bVisible); });
	}

	return CControlUI::SetVisible(bVisible);
}

void CCefBrowserUI::SetInternVisible(bool bVisible)
{
	if (m_pBrowser != nullptr)
	{
#ifndef _USED_CHROME_69_3497
		m_pBrowser->GetHost()->SetWindowVisibility(bVisible);
#else
		CefWindowHandle hwnd = m_pBrowser->GetHost()->GetWindowHandle();
		if (hwnd) {
			if (bVisible) {
				::ShowWindow(hwnd, SW_SHOW);
			} 
			else {
				::ShowWindow(hwnd, SW_HIDE);
			}
		}
#endif
	}
	else
	{
		m_AfterCreatedCacheTasks.push([bVisible, this]{ SetInternVisible(bVisible); });
	}

	return CControlUI::SetInternVisible(bVisible);
}

void CCefBrowserUI::SetProcessMessageHandler(CefRefPtr<CProcessMessageHandler> pHandler)
{
	//m_pProcessMessageHandler = pHandler;
}

void CCefBrowserUI::OnProcessMessageReceived(CefRefPtr<CefProcessMessage> message)
{
	//if (m_pProcessMessageHandler != nullptr)
	//{
	//	m_pProcessMessageHandler->OnProcessMessageReceived(message);
	//}

	CefString name = message->GetName();
	CefRefPtr<CefListValue> args = message->GetArgumentList();
	//�ڴ˴�������Ϣ
	if (name == V8WindowFunciotn::loginFunc)
	{
		//�з���ֵ����renderer�����д���

	}else if(name == V8WindowFunciotn::maxFunc){

		//LOG(INFO) << "Maximize window.";

		if (m_CallbackDelegate){
			m_CallbackDelegate->OnSetFullscreen(true);
		}

	}else if(name == V8WindowFunciotn::restoreFunc){

		//LOG(INFO) << "Restore window size.";

		if (m_CallbackDelegate){
			m_CallbackDelegate->OnSetFullscreen(false);
		}

	}else if(name == V8WindowFunciotn::minFunc){

		//LOG(INFO) << "Minimize window.";

		if (m_CallbackDelegate)
			m_CallbackDelegate->OnMinimized();

	}else if(name == V8WindowFunciotn::ExitFunc){

		//LOG(INFO) << "Close current window.";

		if (m_CallbackDelegate)
			m_CallbackDelegate->OnExit();
	}
}

void CCefBrowserUI::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	// ִֻ��һ��
	if (m_pBrowser != nullptr)
		return;

	m_pBrowser = browser;

	// ִ�л��������
	CefCacheTask task;
	while (!m_AfterCreatedCacheTasks.empty())
	{
		task = move(m_AfterCreatedCacheTasks.front());
		m_AfterCreatedCacheTasks.pop();

		task();
	}
}


//////////////////////////////////////////////////////////////////////////
//�Ƿ����ǰ��
bool CCefBrowserUI::CanGoForward() 
{
	if (m_pBrowser != nullptr)
	{
		return m_pBrowser->CanGoForward();
	}
	return false;
}
//ǰ��
void CCefBrowserUI::GoForward() 
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->GoForward();
	}
}
//���¼���
void CCefBrowserUI::Reload() 
{
	if (m_pBrowser != nullptr)
	{
		CefRefPtr<CefFrame> frame = m_pBrowser->GetMainFrame();
		if (last_opened_url_.length() == 0 ||
			frame->GetURL() == last_opened_url_)
			m_pBrowser->Reload();
		else
			m_pBrowser->GetMainFrame()->LoadURL(last_opened_url_);
	}
}
//���Ի������
void CCefBrowserUI::ReloadIgnoreCache() 
{
	if (m_pBrowser != nullptr)
	{
		CefRefPtr<CefFrame> frame = m_pBrowser->GetMainFrame();
		if (frame->GetURL() == last_opened_url_)
			m_pBrowser->ReloadIgnoreCache();
		else
			m_pBrowser->GetMainFrame()->LoadURL(last_opened_url_);
	}
}
//ֹͣ����
void CCefBrowserUI::StopLoad() 
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->StopLoad();
	}
}
//�Ƿ���Ժ���
bool CCefBrowserUI::CanGoBack() 
{
	if (m_pBrowser != nullptr)
	{
		return m_pBrowser->CanGoBack();
	}
	return false;
}
//����
void CCefBrowserUI::GoBack() 
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->GoBack();
	}
}
//��������
void CCefBrowserUI::Navigate2(CefString url)
{
//  	if (url.empty()){
//  		url = m_sStartupUrl;
// 	}
	if (m_pBrowser != nullptr)
	{
		last_opened_url_ = url;		//��¼�����ʵĵ�ַ������ˢ��
		m_pBrowser->GetMainFrame()->LoadURL(url);
	}
	else
	{
		m_AfterCreatedCacheTasks.push([url, this]
		{
			Navigate2(url);
		});
	}
}
//ִ��JS
void CCefBrowserUI::RunJs(CefString JsCode)
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->GetMainFrame()->ExecuteJavaScript(JsCode, "", 0);
	}
	else
	{
		m_AfterCreatedCacheTasks.push([JsCode, this]{ RunJs(JsCode); });
	}
}

//���ý���
void CCefBrowserUI::SetFocus(bool enable) 
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->GetHost()->SetFocus(enable);
	}
}
//��ȡ���ű���
double CCefBrowserUI::GetZoomLevel() 
{
	if (m_pBrowser != nullptr)
	{
		return m_pBrowser->GetHost()->GetZoomLevel();
	}
	return -1;
}
//�������ű���
void CCefBrowserUI::SetZoomLevel(double zoomLevel) 
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->GetHost()->SetZoomLevel(zoomLevel);
	}
}

void CCefBrowserUI::SetNextChangeZoomValue(int step)
{
	if (m_CallbackDelegate)
	{
		m_CallbackDelegate->OnZoomValueChanged(step);
	}
}
//end
//////////////////////////////////////////////////////////////////////////

void CCefBrowserUI::resize()
{
	if (m_pBrowser != nullptr)
	{
		POINT pt	= { m_rcItem.left, m_rcItem.top };
		HWND hwnd	= m_pBrowser->GetHost()->GetWindowHandle();

		if (GetWindowStyle(hwnd) & WS_POPUP)
		{
			::ClientToScreen(GetManager()->GetPaintWindow(), &pt);
		}

		::MoveWindow(hwnd
			, pt.x
			, pt.y
			, m_rcItem.right - m_rcItem.left
			, m_rcItem.bottom - m_rcItem.top
			, TRUE);
	}
	else
	{
		m_AfterCreatedCacheTasks.push([this]{ resize(); });
	}
}

void CCefBrowserUI::Print()
{
	if (m_pBrowser != nullptr)
	{
		m_pBrowser->GetHost()->Print();
	}
}

CefString CCefBrowserUI::GetCurrentUrl()
{
	if (m_pBrowser != nullptr)
	{
		return m_pBrowser->GetMainFrame()->GetURL();
	}
	return "";
}

void CCefBrowserUI::SetRedirectEnabled(bool enabled)
{
	if (m_pClientHandler != nullptr){
		m_pClientHandler->SetRedirectEnabled(enabled);
	}
}

//////////////////////////////////////////////////////////////////////////
// Ϊduilib���崫��Ļص��ӿ�
void CCefBrowserUI::OnAddressChange(const CefString& url) 
{
	if (m_CallbackDelegate)
		m_CallbackDelegate->OnSetAddress(url);
}

void CCefBrowserUI::OnTitleChange(const CefString& title) 
{
	//LOG(INFO) << "Set window title to be " << title.ToString();
	
	if (m_CallbackDelegate)
		m_CallbackDelegate->OnSetTitle(title);
}

void CCefBrowserUI::OnFullscreenModeChange(bool fullscreen) 
{
	if (m_CallbackDelegate)
		m_CallbackDelegate->OnSetFullscreen(fullscreen);
}

void CCefBrowserUI::OnLoadingStateChange(bool isLoading,
									   bool canGoBack,
									   bool canGoForward) 
{
	if (m_pBrowser && !isLoading){
		CefString url = m_pBrowser->GetMainFrame()->GetURL();
		if (url.length() > 0 && url.ToString().find(ERROR_URL_PREFIX) != 0)
			last_opened_url_ = url;		//��¼�����ʵĵ�ַ������ˢ��
	}

	if (m_CallbackDelegate)
		m_CallbackDelegate->OnSetLoadingState(isLoading, canGoBack, canGoForward);
}

void CCefBrowserUI::OnDraggableRegionsChanged(
	const std::vector<CefDraggableRegion>& regions) 
{
	if (m_CallbackDelegate)
		m_CallbackDelegate->OnSetDraggableRegions(regions);
}

void CCefBrowserUI::OnLoadError(CefRefPtr<CefFrame> frame
								, int errorCode
								, const CefString& errorText
								, const CefString& failedUrl) 
{
	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED){
		return;
	}

// 	// Don't display an error for external protocols that we allow the OS to
// 	// handle. See OnProtocolExecution().
// 	if (errorCode == ERR_UNKNOWN_URL_SCHEME) {
// 		CGLogger::Debug("OnLoadError ERR_UNKNOWN_URL_SCHEME. Message:" + (errorText.ToString()) + "; URL:" + failedUrl.ToString());
// 		wstring urlStr = failedUrl.ToWString();
// 		wstring wpro = Configurations::getInstance()->GetLocalAppProtocol();
// 		if (urlStr.find(wpro) == 0)
// 			return;
// 	}

	string error_code = to_string(errorCode);
	string error_name = GetErrorString(errorCode);

	
	string error_url = ERROR_URL_PREFIX"error.html?url=" + CefURIEncode(failedUrl, false).ToString() + "&code=" + error_name;
	frame->LoadURL(error_url);

// 	if (m_CallbackDelegate)
// 		m_CallbackDelegate->OnLoadError(frame, errorCode, errorText, failedUrl);
}
//////////////////////////////////////////////////////////////////////////

std::string CCefBrowserUI::GetErrorString(int code) {
	// Case condition that returns |code| as a string.
#define CASE(code) case code: return #code

	switch (code) {
		CASE(ERR_NONE);
		CASE(ERR_FAILED);
		CASE(ERR_ABORTED);
		CASE(ERR_INVALID_ARGUMENT);
		CASE(ERR_INVALID_HANDLE);
		CASE(ERR_FILE_NOT_FOUND);
		CASE(ERR_TIMED_OUT);
		CASE(ERR_FILE_TOO_BIG);
		CASE(ERR_UNEXPECTED);
		CASE(ERR_ACCESS_DENIED);
		CASE(ERR_NOT_IMPLEMENTED);
		CASE(ERR_CONNECTION_CLOSED);
		CASE(ERR_CONNECTION_RESET);
		CASE(ERR_CONNECTION_REFUSED);
		CASE(ERR_CONNECTION_ABORTED);
		CASE(ERR_CONNECTION_FAILED);
		CASE(ERR_NAME_NOT_RESOLVED);
		CASE(ERR_INTERNET_DISCONNECTED);
		CASE(ERR_SSL_PROTOCOL_ERROR);
		CASE(ERR_ADDRESS_INVALID);
		CASE(ERR_ADDRESS_UNREACHABLE);
		CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED);
		CASE(ERR_TUNNEL_CONNECTION_FAILED);
		CASE(ERR_NO_SSL_VERSIONS_ENABLED);
		CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH);
		CASE(ERR_SSL_RENEGOTIATION_REQUESTED);
		CASE(ERR_CERT_COMMON_NAME_INVALID);
		CASE(ERR_CERT_DATE_INVALID);
		CASE(ERR_CERT_AUTHORITY_INVALID);
		CASE(ERR_CERT_CONTAINS_ERRORS);
		CASE(ERR_CERT_NO_REVOCATION_MECHANISM);
		CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION);
		CASE(ERR_CERT_REVOKED);
		CASE(ERR_CERT_INVALID);
		CASE(ERR_CERT_END);
		CASE(ERR_INVALID_URL);
		CASE(ERR_DISALLOWED_URL_SCHEME);
		CASE(ERR_UNKNOWN_URL_SCHEME);
		CASE(ERR_TOO_MANY_REDIRECTS);
		CASE(ERR_UNSAFE_REDIRECT);
		CASE(ERR_UNSAFE_PORT);
		CASE(ERR_INVALID_RESPONSE);
		CASE(ERR_INVALID_CHUNKED_ENCODING);
		CASE(ERR_METHOD_NOT_SUPPORTED);
		CASE(ERR_UNEXPECTED_PROXY_AUTH);
		CASE(ERR_EMPTY_RESPONSE);
		CASE(ERR_RESPONSE_HEADERS_TOO_BIG);
		CASE(ERR_CACHE_MISS);
		CASE(ERR_INSECURE_RESPONSE);
		CASE(ERR_CONNECTION_TIMED_OUT);
		CASE(ERR_RENDER_ABNORMAL_TERMINATION);
		CASE(ERR_RENDER_PROCESS_WAS_KILLED);
		CASE(ERR_RENDER_PROCESS_CRASHED);
	default:
		return "UNKNOWN";
	}
}