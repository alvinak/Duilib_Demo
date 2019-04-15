#pragma once
#include <UIlib.h>
using namespace DuiLib;

//////////////////////////////////////////////////////////////////////////
// ����ʹ��xml�ļ���������,���У���cef������ؼ�
//////////////////////////////////////////////////////////////////////////

class CCefBrowserSimpleFrameWnd : public WindowImplBase
{
public:
	CCefBrowserSimpleFrameWnd(void);
	virtual ~CCefBrowserSimpleFrameWnd(void);

	virtual LPCTSTR GetWindowClassName() const ;
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	virtual void    Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void FirstLoadCef();
	void OnSelChanged(CControlUI* pSender);

	//cef
	void AdjustCefWindow();
	void HideCefWindow();

protected:
	BOOL     m_bIsInit;
};

