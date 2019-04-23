#pragma once
#include "global_include.h"

class CWidgetsDemoBase
{
public:
    CWidgetsDemoBase(HWND parent, RECT rc);
    ~CWidgetsDemoBase(void);

    void ShowWidgets_Test();

    virtual vtkSmartPointer<vtkRenderer> CreateRenderer();
    virtual vtkSmartPointer<vtkRenderWindow> CreateRenderWindow(vtkSmartPointer<vtkRenderer> renderer);
    virtual vtkSmartPointer<vtkRenderWindowInteractor> CreateInteractor(vtkSmartPointer<vtkRenderWindow> renderWindow);
    virtual void SetMyInteractorStyle(vtkSmartPointer<vtkRenderWindowInteractor> interactor) { }
    virtual void CreatePipeline(vtkSmartPointer<vtkRenderer> renderer) { }
    virtual void AddMyActor(vtkSmartPointer<vtkRenderWindowInteractor> interactor) { }
    virtual void StartWidgetsRender(vtkSmartPointer<vtkRenderer> renderer, 
        vtkSmartPointer<vtkRenderWindow> renderWindow, 
        vtkSmartPointer<vtkRenderWindowInteractor> interactor) { }

	void ResizeAndPosition(RECT rc);

protected:
    vtkSmartPointer<vtkRenderer>                m_renderer;
    vtkSmartPointer<vtkRenderWindow>            m_renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor>  m_interactor;

	HWND m_parentWnd;
	RECT m_rc;
};

