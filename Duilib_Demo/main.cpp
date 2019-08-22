#pragma once
#include <UIlib.h>
using namespace DuiLib;

#include "FrameWndDemo/EntryFrameWnd.h"
#include "FrameWndDemo/CefBrowserClientFrameWnd.h"
#include "FrameWndDemo/DcmtkDLDicomDemoFrameWnd.h"

#include "cef_simple/simple_app.h"
#include "base/Cef3/common/client_app.h"
#include "base/Cef3/browser/client_browser_app.h"
#include "base/Cef3/renderer/client_renderer_app.h"
#include "base/Cef3/browser/ClientSchemeHandler.h"

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif

#ifdef _DEBUG
#ifdef _USED_CHROME_69_3497
#pragma comment(lib, "libcef_wrapper_wrapper_69_3497/libcef_wrapper_wrapper_d.lib")
#else
#pragma comment(lib, "libcef_wrapper_wrapper_49_2623/libcef_wrapper_wrapper_d.lib")
#endif
#else	
#ifdef _USED_CHROME_69_3497
#pragma comment(lib, "libcef_wrapper_wrapper_69_3497/libcef_wrapper_wrapper.lib")
#else
#pragma comment(lib, "libcef_wrapper_wrapper_49_2623/libcef_wrapper_wrapper.lib")
#endif
#endif


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	 // ������Դ��Ĭ��·�����˴�����Ϊ��exe��ͬһĿ¼��
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("..\\..\\Bin\\Skin\\"));  

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	// CEF ��ʼ�� ���
	CefMainArgs args(hInstance);
	//����CefAppʵ��
	//CefRefPtr<SimpleApp> app(new SimpleApp);
	// ���ݽ�����������ͬ��CefAppʵ��
	CefRefPtr<ClientApp> app;

	// ��ȡ��������
	ClientApp::ProcessType processType = ClientApp::GetProcessType();

	// ����Render���̣�����Render���̺�Browser���̽������ݽ���������JS��C++����
	// browser���̣�����ClientAppBrowserʵ��
	if (processType == ClientApp::ProcessType::BrowserProcess)
	{
		app = new ClientAppBrowser;
	}
	// render���̣�����ClientAppRenderʵ��
	else if (processType == ClientApp::ProcessType::RendererProcess)
	{
		app = new ClientAppRender;
	}
	// �������̣�����ClientAppʵ��
	else
	{
		app = new ClientApp;
	}


#if 1 //def RENDERER_DEBUG
	if (processType == ClientApp::ProcessType::RendererProcess){
		MessageBox(NULL, _T("To attach renderer process!"), _T("test"), 0);
	}
#endif

	// ���������ִ����browser����ʱ���������أ�
	// ���ӽ���ʱ�����ֱ���˳�ʱ���ء�
	int exitCode = CefExecuteProcess(args, app, NULL);
	if (exitCode >= 0)
	{
		return exitCode;
	}
	//�������ṹ�壬���ڶ���CEF����Ϊ��
	CefSettings settings;
	CefSettingsTraits::init(&settings);
	CefString(&settings.locale).FromString("zh-CN");
	//��ʼ��CEF
	bool ret = CefInitialize(args, settings, app, NULL);

	if (ret){
		// ע��ClientSchemeHandler
		// ��ʽ��client://resources/
		//CefRegisterSchemeHandlerFactory(HANDLER_SCHEME_NAME, HANDLER_DOMAIN_NAME,
		//	new ClientSchemeHandlerFactory());
		// ʹ��XMLHttpRequest���н�������1����ע��Scheme
		CefRegisterSchemeHandlerFactory("http", HANDLER_DOMAIN_NAME, new ClientSchemeHandlerFactory);
	};

	//CFirstFrameWnd duiFrame;
	//CSecondFrameWnd duiFrame;
	//CCefBrowserFrameWnd duiFrame;
	//CEntryFrameWnd duiFrame;
	DcmtkDLDicomDemoFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, 0L);
	duiFrame.CenterWindow();
	duiFrame.ShowWindow();


	// CEF �����������
	//ִ����Ϣѭ��,��ʱ�������ֱ��CefQuitMessageLoop()���������á�
	CefRunMessageLoop();
	// �ر�CEF���ͷ���Դ  
	CefShutdown();


	/*CPaintManagerUI::MessageLoop();*/

	return 0;
}