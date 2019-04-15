#pragma once

#include <queue>

#include "include/cef_client.h"

#include "base/Cef3/common/process_message_handler.h"
#include "base/Cef3/common/cef_client_handler.h"
#include "base/Cef3/renderer/v8handler_function.h"

//using CefCacheTask = std::function<void(void)>;
typedef std::function<void(void)> CefCacheTask;
//������
const TCHAR CCefBrowserUIClassName[] = L"CefBrowserUI";		
//�ؼ�����
const TCHAR CCefBrowserUIItemInterface[] = L"CefBrowser";

class CCefBrowserUI : public CControlUI
	, public IDialogBuilderCallback
{
public:
	CCefBrowserUI(ICallbackDelegate* callbackDelegate, const CefString& url);
	virtual ~CCefBrowserUI();

	// Control
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void DoInit();
	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetVisible(bool bVisible);
	void SetInternVisible(bool bVisible);
	
	// CefClientHandler�ص��ӿ�
	void OnProcessMessageReceived(CefRefPtr<CefProcessMessage> message);
	void OnAfterCreated(CefRefPtr<CefBrowser> browser);

	//////////////////////////////////////////////////////////////////////////
	//�������Զ���ӿ�
	//�Ƿ����ǰ��
	bool CanGoForward();
	//ǰ��
	void GoForward();
	//���¼���
	void Reload();
	//���Ի������
	void ReloadIgnoreCache();
	//ֹͣ����
	void StopLoad();
	//�Ƿ���Ժ���
	bool CanGoBack();
	//����
	void GoBack();
	//��������
	void Navigate2(CefString url);
	//ִ��JS
	void RunJs(CefString JsCode);
	//�������ű���
	void SetZoomLevel(double zoomLevel);
	void SetNextChangeZoomValue(int step);
	//����ϵͳ��ӡ�Ի���
	void Print();
	//��ȡ��ǰ��ҳ�ĵ�ַ
	CefString GetCurrentUrl();
	//�����Ƿ������ת����
	void SetRedirectEnabled(bool enabled);
	//////////////////////////////////////////////////////////////////////////

	// Execute Delegate notifications on the main thread.
	// CefClientHandler�ص��ӿ�
	void OnAddressChange(const CefString& url);
	void OnTitleChange(const CefString& title);
	void OnFullscreenModeChange(bool fullscreen);
	void OnLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward);
	void OnDraggableRegionsChanged(const std::vector<CefDraggableRegion>& regions);
	void OnLoadError(CefRefPtr<CefFrame> frame
		, int errorCode
		, const CefString& errorText
		, const CefString& failedUrl);

	// ������Ϣ����
	void SetProcessMessageHandler(CefRefPtr<CProcessMessageHandler> pHandler);

	// ʵ���Զ���ؼ��Ĵ���
	CControlUI* CreateControl(LPCTSTR pstrClass) override
	{
		//if (_tcscmp(pstrClass, _T("Cef")) == 0) return new CCefUI();
		if (_tcscmp(pstrClass, _T("Cef")) == 0) return this;
		return NULL;
	}

private:
	void resize();
	//���ý���
	void SetFocus(bool enable);
	//��ȡ���ű���
	double GetZoomLevel();
	string GetErrorString(int code);

private:	
	CefRefPtr<CCefClientHandler>			m_pClientHandler;
	CefRefPtr<CefBrowser>					m_pBrowser;
	CefRefPtr<CProcessMessageHandler>		m_pProcessMessageHandler;
	CefString								m_sStartupUrl;
	ICallbackDelegate*						m_CallbackDelegate;
	// browser�������ǰ���������
	std::queue<std::function<void(void)>>	m_AfterCreatedCacheTasks;
	wstring									last_opened_url_;
};