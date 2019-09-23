#include "stdafx.h"

#include "VtkFunctionDemo.h"

CVtkFunctionDemo::CVtkFunctionDemo(HWND parent, RECT rc)
	: m_parentWnd(parent)
	, m_rc(rc)
{
   
}


CVtkFunctionDemo::~CVtkFunctionDemo(void)
{
}

void CVtkFunctionDemo::ResizeAndPosition(RECT rc)
{
	if (m_parentWnd) {
		renWin->SetParentId(m_parentWnd);
		int title_height = 0;//32;
		int button_test_height = 0;//30;
		RECT rect;
		::GetClientRect(m_parentWnd, &rect);
		rect = rc;
		renWin->SetSize(rect.right - rect.left, rect.bottom - rect.top - title_height - button_test_height);
		renWin->SetPosition(rect.left, rect.top + title_height + button_test_height);
	}
}

void CVtkFunctionDemo::Function1()
{
	vtkSmartPointer< vtkJPEGReader > reader = 
		vtkSmartPointer< vtkJPEGReader >::New();
	reader->SetFileName("..\\Bin\\Skin\\image\\texture.jpg");

	vtkSmartPointer< vtkTexture > texture = 
		vtkSmartPointer< vtkTexture >::New();
	texture->SetInputConnection( reader->GetOutputPort() );
	texture->InterpolateOn();


	vtkSmartPointer<vtkCylinderSource> cylinder = vtkSmartPointer<vtkCylinderSource>::New();
	cylinder->SetHeight( 3.0 );
	cylinder->SetRadius( 1.0 );
	cylinder->SetResolution( 10 ); 

	vtkSmartPointer<vtkPolyDataMapper> cylinderMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	cylinderMapper->SetInputConnection( cylinder->GetOutputPort() ); 

	vtkSmartPointer<vtkActor> cylinderActor = vtkSmartPointer<vtkActor>::New();
	cylinderActor->SetMapper( cylinderMapper );
	cylinderActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	cylinderActor->SetTexture(texture);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor( cylinderActor );
	renderer->SetBackground( 1.0, 1.0, 1.0 );

	renWin = vtkSmartPointer<vtkRenderWindow>::New();
	renWin->AddRenderer( renderer );

	//renWin->SetSize( 640, 480 );
	ResizeAndPosition(m_rc);

	renWin->Render();
	renWin->SetWindowName("RenderCylinder");

	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	iren->SetInteractorStyle(style);

	iren->Initialize();
	iren->Start();
}

void CVtkFunctionDemo::InitVtk()
{
	/*vtkSmartPointer<vtkJPEGReader>*/ reader = vtkSmartPointer<vtkJPEGReader>::New();
	reader->SetFileName("..\\Bin\\Skin\\image\\ct.jpg");
	reader->Update();

	/*vtkSmartPointer<vtkImageActor>*/ imgActor = vtkSmartPointer<vtkImageActor>::New();
	imgActor->SetInput(reader->GetOutput());

	/*vtkSmartPointer<vtkRenderer>*/ render = vtkSmartPointer<vtkRenderer>::New();
	render->AddActor(imgActor);
	render->SetBackground(0, 0, 0);
	render->ResetCamera();

	renWin = vtkSmartPointer<vtkRenderWindow>::New();
	renWin->AddRenderer(render);
	renWin->SetWindowName("MeasurementDistanceApp");
	//rw->SetSize(320, 320);
	ResizeAndPosition(m_rc);

	/*vtkSmartPointer<vtkRenderWindowInteractor>*/ rwi = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	rwi->SetRenderWindow(renWin);

	/*vtkSmartPointer<vtkInteractorStyleImage>*/ style = vtkSmartPointer<vtkInteractorStyleImage>::New();
	rwi->SetInteractorStyle(style);
	
	renWin->Render();
	rwi->Initialize();
	// �ؼ��㣺�˴�ʹ��Start������Ϣѭ����������VTK
	//rwi->Start();
	
}

void CVtkFunctionDemo::Function2(int WidgetType)
{	
	//vtkDistanceWidget
	if (WidgetType == 0)
	{
		//ʵ����Widget
		/*vtkSmartPointer<vtkDistanceWidget>*/ distanceWidget = vtkSmartPointer<vtkDistanceWidget>::New();
		//ָ����Ⱦ���ڽ�����,�������û��¼�
		distanceWidget->SetInteractor(rwi);
		//��Ҫʱʹ�ù۲���/����ģʽ�����ص�����(�˴�û��)
		//�������α��ʵ�塣��SetRepresentation()���¼���Widget��������
		//����ʹ��WidgetĬ�ϵļ��α��ʵ��
		distanceWidget->CreateDefaultRepresentation();
		static_cast<vtkDistanceRepresentation*> (distanceWidget->GetRepresentation())
			->SetLabelFormat("%-#6.3g px");
		//����Widget
		distanceWidget->On();
	}
	//vtkAngleWidget
	if (WidgetType == 1)
	{
		/*vtkSmartPointer<vtkAngleWidget>*/ angleWiget = vtkSmartPointer<vtkAngleWidget>::New();
		angleWiget->SetInteractor(rwi);
		//�������Ի���ʵ��ͼ��
		vtkSmartPointer<vtkAngleRepresentation2D> angleRep =
			vtkSmartPointer<vtkAngleRepresentation2D>::New();
		angleRep->GetRay1()->GetProperty()->SetColor(0, 1, 0);
		angleRep->GetRay1()->GetProperty()->SetLineWidth(3);
		angleRep->GetRay2()->GetProperty()->SetColor(0, 1, 0);
		angleRep->GetRay1()->GetProperty()->SetLineWidth(3);
		angleRep->GetArc()->GetProperty()->SetColor(0, 1, 0);
		angleRep->GetArc()->GetProperty()->SetLineWidth(3);
		angleWiget->SetRepresentation(angleRep);
		angleWiget->On();
	}
	//vtkBiDimensionalWidget
	if (WidgetType == 2)
	{
		/*vtkSmartPointer<vtkBiDimensionalWidget>*/ bidimensionalWidget =
			vtkSmartPointer<vtkBiDimensionalWidget>::New();
		bidimensionalWidget->SetInteractor(rwi);
		//����Ĭ�ϵ�ͼ��
		bidimensionalWidget->CreateDefaultRepresentation();
		bidimensionalWidget->On();
	}
}

#include <fstream>

vtkSmartPointer<vtkPoints> CVtkFunctionDemo::GetPoints(std::string file_name)
{
	printf("%s\n", file_name.c_str());
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	std::fstream f(file_name.c_str());

	//std::vector<std::string> lines;
	std::string line;
	int index = 1;
	while (getline(f, line)) {
		printf("%s\n", line.c_str());
		int x = 0;
		int y = 0;
		int z = 0;
		sscanf(line.c_str(), "%d %d %d", &x, &y, &z);
		//lines.push_back(line);
		double a = double(x);
		double b = double(y);
		double c = double(z);
		points->InsertNextPoint(a, b, c);
		printf("index :%d : %.2f, %.2f, %.2f\n", index, a, b, c);
	}

	f.close();
	return points;
}


void CVtkFunctionDemo::Function_for_cta()
{
	double color[4][3] = {
		{1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0},
		{1.0, 0.0, 1.0}
	};

	std::vector<std::string> files;
	// �ϴ���һ���ļ���git��·��Ϊ��Duilib_Demo\data
	files.push_back("E:\\GinkgoCADx-3.7.1.1573.41_dev\\source\\SvrCallImageEngine\\lib\\Debug\\4F00D5B0_112\\curve_source\\L_ICA.txt");
	files.push_back("E:\\GinkgoCADx-3.7.1.1573.41_dev\\source\\SvrCallImageEngine\\lib\\Debug\\4F00D5B0_112\\curve_source\\R_ICA.txt");
	files.push_back("E:\\GinkgoCADx-3.7.1.1573.41_dev\\source\\SvrCallImageEngine\\lib\\Debug\\4F00D5B0_112\\curve_source\\L_VA.txt");
	files.push_back("E:\\GinkgoCADx-3.7.1.1573.41_dev\\source\\SvrCallImageEngine\\lib\\Debug\\4F00D5B0_112\\curve_source\\R_VA.txt");


	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();	

	for (int Index = 0; Index < files.size() ; Index++) {
		// ����������ɫʱ���������4����ר����cta
		if (files.size() > 4) {
			assert(0);
		}
		vtkSmartPointer<vtkPoints> points = GetPoints(files[Index]);
		vtkIdType number_points = points->GetNumberOfPoints();

		vtkSmartPointer<vtkPolyLine> polyLine =	vtkSmartPointer<vtkPolyLine>::New();
		polyLine->GetPointIds()->SetNumberOfIds(points->GetNumberOfPoints());
		for(unsigned int i = 0; i < points->GetNumberOfPoints(); i++)
		{
			polyLine->GetPointIds()->SetId(i,i);
		}

		vtkSmartPointer<vtkCellArray> cellsArray = vtkSmartPointer<vtkCellArray>::New();
		cellsArray->InsertNextCell(polyLine);

		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		polyData->SetPoints(points);
		polyData->SetLines(cellsArray);
		polyData->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInput(polyData);

		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetLineWidth(2);
		actor->GetProperty()->SetColor(color[Index]);

		renderer->AddActor(actor);
	}
	
	
	vtkSmartPointer<vtkRenderWindow> renderwindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderwindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderwindowinteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderwindowinteractor->SetRenderWindow(renderwindow);

	renderwindow->Render();
	renderwindowinteractor->Start();
}