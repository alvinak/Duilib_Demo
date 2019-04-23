#include "stdafx.h"
#include "WidgetsDemoBase.h"


CWidgetsDemoBase::CWidgetsDemoBase(HWND parent, RECT rc)
	: m_parentWnd(parent)
	, m_rc(rc)
{
}


CWidgetsDemoBase::~CWidgetsDemoBase(void)
{
}

void CWidgetsDemoBase::ShowWidgets_Test()
{
    // Create the RenderWindow, Renderer and both Actors
    m_renderer        = CreateRenderer();
    m_renderWindow    = CreateRenderWindow(m_renderer);
    m_interactor      = CreateInteractor(m_renderWindow);
    SetMyInteractorStyle(m_interactor);
    // Create a test pipeline
    CreatePipeline(m_renderer);
    // Create the widget
    AddMyActor(m_interactor);

    StartWidgetsRender(m_renderer, m_renderWindow, m_interactor);
}

vtkSmartPointer<vtkRenderer> CWidgetsDemoBase::CreateRenderer()
{
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    
    renderer->SetBackground(0.1, 0.2, 0.4);

    return renderer;
}

vtkSmartPointer<vtkRenderWindow> CWidgetsDemoBase::CreateRenderWindow(vtkSmartPointer<vtkRenderer> renderer)
{
    vtkSmartPointer< vtkRenderWindow > renderWindow = vtkSmartPointer< vtkRenderWindow >::New();

    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("AnnotationWidget");
    //renderWindow->SetSize(400, 400);

    return renderWindow;
}
vtkSmartPointer<vtkRenderWindowInteractor> CWidgetsDemoBase::CreateInteractor(vtkSmartPointer<vtkRenderWindow> renderWindow)
{
    vtkSmartPointer< vtkRenderWindowInteractor > interactor = 
        vtkSmartPointer< vtkRenderWindowInteractor >::New();

    interactor->SetRenderWindow(renderWindow);

    return interactor;
}

void CWidgetsDemoBase::ResizeAndPosition(RECT rc)
{
	if (m_parentWnd) {
		m_renderWindow->SetParentId(m_parentWnd);
		int title_height = 0;//32;
		int button_test_height = 0;//30;
		RECT rect;
		::GetClientRect(m_parentWnd, &rect);
		rect = rc;
		m_renderWindow->SetSize(rect.right - rect.left, rect.bottom - rect.top - title_height - button_test_height);
		m_renderWindow->SetPosition(rect.left, rect.top + title_height + button_test_height);
	}
}