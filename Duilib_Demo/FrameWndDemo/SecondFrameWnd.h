#pragma once
#include <UIlib.h>
using namespace DuiLib;

//////////////////////////////////////////////////////////////////////////
// ����ʹ��xml�ļ���������
//////////////////////////////////////////////////////////////////////////

class CSecondFrameWnd : public CWindowWnd, public INotifyUI
{
public:
	CSecondFrameWnd(void);
	~CSecondFrameWnd(void);

	virtual LPCTSTR GetWindowClassName() const ;
	virtual void    Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CPaintManagerUI m_PaintManager;
};

