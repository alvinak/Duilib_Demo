#pragma once

#include "global_include.h"

class CDicomSplitView
{
public:
    CDicomSplitView(HWND parent, RECT rc);
    ~CDicomSplitView(void);

	void ResizeAndPosition(RECT rc);
private:
    vtkSmartPointer<vtkRenderer> CreateVtkRender();

    vtkSmartPointer<vtkRenderer> CreateDicomVtkRender();
    // ������һ����ʾ��ά�߹ǵ�VTK
    vtkSmartPointer<vtkRenderer> CreateDicomRibVtkRender();

    vtkSmartPointer<vtkRenderer> m_render_left_down ;
    vtkSmartPointer<vtkRenderer> m_render_left_up   ;
    vtkSmartPointer<vtkRenderer> m_render_right_down;
    vtkSmartPointer<vtkRenderer> m_render_right_up  ;

    vtkSmartPointer<vtkDICOMImageReader> m_reader;

	vtkSmartPointer<vtkRenderWindow> renWin;

	HWND m_parentWnd;
	RECT m_rc;
};

