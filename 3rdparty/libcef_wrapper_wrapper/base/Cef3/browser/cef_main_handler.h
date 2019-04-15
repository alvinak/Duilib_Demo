#pragma once

#include "../browser/client_browser_app.h"
#include "../renderer/client_renderer_app.h"
#include "ClientSchemeHandler.h"
#include "include/cef_parser.h"

class CCefMainHandler
{
public:

	bool Initialize(HINSTANCE hInstance, BOOL _multi_threaded_message_loop = TRUE)
	{
		m_multi_threaded_message_loop = _multi_threaded_message_loop;

		CefMainArgs args(hInstance);

		// ��ȡ��������
		ClientApp::ProcessType processType = ClientApp::GetProcessType();

		// ���ݽ�����������ͬ��CefAppʵ��
		CefRefPtr<CefApp> app;

		// browser���̣�����ClientAppBrowserʵ��
		if (processType == ClientApp::ProcessType::BrowserProcess)
		{
			CGLogger::Info("Browser process is created...");

			app = new ClientAppBrowser;
		}
		// render���̣�����ClientAppRenderʵ��
		else if (processType == ClientApp::ProcessType::RendererProcess)
		{
			CGLogger::Info("Renderer process is created...");

			app = new ClientAppRender;
		}
		// �������̣�����ClientAppʵ��
		else
		{
			app = new ClientApp;
		}

		// �ӽ���������������ֱ���˳�
		int exit_code = CefExecuteProcess(args, app, NULL);
		if (exit_code >= 0)
		{
			exit(exit_code);
		}

		CefSettings settings;
		CefSettingsTraits::init(&settings);
		settings.log_severity = LOGSEVERITY_DISABLE;			// ���������ļ�
		settings.multi_threaded_message_loop = _multi_threaded_message_loop;
#ifdef _USED_CHROME_69_3497        
#else
        settings.single_process = false;
#endif
		//////////////////////////////////////////////////////////////////////////
		// If empty then browsers will be created in "incognito mode" where in-memory caches are
		// used for storage and no data is persisted to disk. HTML5 databases such as
		// localStorage will only persist across sessions if a cache path is
		// specified. Can be overridden for individual CefRequestContext instances via
		// the CefRequestContextSettings.cache_path value.
		wstring cachePath = Configurations::getInstance()->GetUserDataDir() + _T("cache\\");
		CefString(&settings.cache_path).FromWString(cachePath);
		//////////////////////////////////////////////////////////////////////////
		// Opaque background color used for accelerated content. By default the
		// background color will be white. Only the RGB compontents of the specified
		// value will be used. The alpha component must greater than 0 to enable use
		// of the background color but will be otherwise ignored.
		// default background color #1E2124
#ifdef _USED_CHROME_69_3497 
		//û��ʹ�������ļ������ɫ
		cef_color_t background_color = CefColorSetARGB(255, 30, 33, 36);
		settings.background_color = background_color;
#else 
		cef_color_t background_color = CefColorSetARGB(255, 30, 33, 36);
		// Parse the background color value.
		wstring bk_color_from_config = Configurations::getInstance()->GetBackgroundColor();
		CefParseCSSColor(bk_color_from_config, false, background_color);
		settings.background_color = background_color;
#endif
		//////////////////////////////////////////////////////////////////////////
		// Value that will be returned as the User-Agent HTTP header. If empty the
		// default User-Agent string will be used. Also configurable using the
		// "user-agent" command-line switch.
		CefString(&settings.user_agent).FromString("Dr.Wise Client V1.0");
		CefString(&settings.product_version).FromString("V1.0");
		//////////////////////////////////////////////////////////////////////////

		CefString(&settings.locale).FromString("zh-CN");

		
		return CefInitialize(args, settings, app, NULL);
		//if (ret){
		//	// ע��ClientSchemeHandler
		//	// ��ʽ��client://resources/
		//	CefRegisterSchemeHandlerFactory(HANDLER_SCHEME_NAME, HANDLER_DOMAIN_NAME,
		//		new ClientSchemeHandlerFactory());
		//};

		//return ret;
	}

	void RunMessageLoop()
	{
		if (m_multi_threaded_message_loop)
		{
			CPaintManagerUI::MessageLoop();
		}
		else
		{
			CefRunMessageLoop();
		}	
	}

	void Quit()
	{
		if (m_multi_threaded_message_loop)
		{
			PostQuitMessage(IDOK);
		}
		else
		{
			CefQuitMessageLoop();
		}	
	}

	void Shutdown()
	{
		CefShutdown();
	}

public:
	BOOL m_multi_threaded_message_loop;
};