#include "simple_app.h"
#include "StdAfx.h"
#include "simple_handler.h"
#include "include/wrapper/cef_helpers.h"

SimpleApp::SimpleApp() 
{

}

void SimpleApp::OnContextInitialized() {

	//ֻ������UI�߳���
	CEF_REQUIRE_UI_THREAD();

//	//����CefClientʵ��
//	CefRefPtr<SimpleHandler> handler(new SimpleHandler());
//
//	// �������������
//	CefBrowserSettings browser_settings;
//
//	// Information used when creating the native window.
//	CefWindowInfo window_info;
//	window_info.SetAsPopup(NULL, _T("cefsimple"));
//
//	// �������������
//	CefBrowserHost::CreateBrowser(window_info
//		, handler
//		, _T("http://www.baidu.com")
//		, browser_settings
//		, NULL);
//
}