#pragma once
#include <UIlib.h>
using namespace DuiLib;

#include "FrameWndDemo/FirstFrameWnd.h"
#include "FrameWndDemo/SecondFrameWnd.h"
#include "FrameWndDemo/CefBrowserFrameWnd.h"

#include "cef\simple_app.h"


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
	CefRefPtr<SimpleApp> app(new SimpleApp);
	// ���������ִ����browser����ʱ���������أ�
	// ���ӽ���ʱ�����ֱ���˳�ʱ���ء�
	int exitCode = CefExecuteProcess(args, app, NULL);
	if (exitCode >= 0)
	{
		return exitCode;
	}
	//�������ṹ�壬���ڶ���CEF����Ϊ��
	CefSettings settings;
	//��ʼ��CEF
	CefInitialize(args, settings, app, NULL);



	//CFirstFrameWnd duiFrame;
	//CSecondFrameWnd duiFrame;
	CCefBrowserFrameWnd duiFrame;
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