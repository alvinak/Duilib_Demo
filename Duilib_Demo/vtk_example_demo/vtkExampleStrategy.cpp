#include "stdafx.h"
#include "vtk_example_demo/vtkExampleStrategy.h"


vtkExampleStrategyBase::vtkExampleStrategyBase(void)
{
}


vtkExampleStrategyBase::~vtkExampleStrategyBase(void)
{
}

//////////////////////////////////////////////////////////////////////////
vtkShrinkAndMaginifyStrategy::vtkShrinkAndMaginifyStrategy()
{}

vtkShrinkAndMaginifyStrategy::~vtkShrinkAndMaginifyStrategy()
{}

void vtkShrinkAndMaginifyStrategy::AlgrithmInterface()
{
	//
	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetFileName ("..\\Bin\\Skin\\data\\slices1\\2");
	reader->Update();

	// ������ ����
	vtkSmartPointer<vtkImageShrink3D> shrinkFilter = 
		vtkSmartPointer<vtkImageShrink3D>::New();
	shrinkFilter->SetInputConnection(reader->GetOutputPort());
	shrinkFilter->SetShrinkFactors(16,16,1);
	shrinkFilter->Update();

	// ������ ����
	vtkSmartPointer<vtkImageMagnify> magnifyFilter =
		vtkSmartPointer<vtkImageMagnify>::New();
	magnifyFilter->SetInputConnection(reader->GetOutputPort());
	magnifyFilter->SetMagnificationFactors(10,10,1);
	magnifyFilter->Update();

	int originalDims[3];
	reader->GetOutput()->GetDimensions(originalDims);

	double originalSpace[3];
	reader->GetOutput()->GetSpacing(originalSpace);

	// ������ ͼ���С
	int shrinkDims[3];
	shrinkFilter->GetOutput()->GetDimensions(shrinkDims);

	double shrinkSpace[3];
	shrinkFilter->GetOutput()->GetSpacing(shrinkSpace);

	// ������ ͼ���С
	int magnifyDims[3];
	magnifyFilter->GetOutput()->GetDimensions(magnifyDims);

	double magnifySpace[3];
	magnifyFilter->GetOutput()->GetSpacing(magnifySpace);

	std::cout<<"origin dimesions      : "<<originalDims[0] << " "<<originalDims[1]<<" "<<originalDims[2]<<std::endl;
	std::cout<<"origin space          : "<<originalSpace[0] << " "<<originalSpace[1]<<" "<<originalSpace[2]<<std::endl;
	std::cout<<"shrink dimesions      : "<<shrinkDims[0] << " "<<shrinkDims[1]<<" "<<shrinkDims[2]<<std::endl;
	std::cout<<"shrink space          : "<<shrinkSpace[0] << " "<<shrinkSpace[1]<<" "<<shrinkSpace[2]<<std::endl;
	std::cout<<"magnify dimensisons   :"<<magnifyDims[0] << " "<<magnifyDims[1]<<" "<<magnifyDims[2]<<std::endl;
	std::cout<<"magnify space         :"<<magnifySpace[0] << " "<<magnifySpace[1]<<" "<<magnifySpace[2]<<std::endl;

	vtkSmartPointer<vtkImageActor> originalActor =
		vtkSmartPointer<vtkImageActor>::New();
	originalActor->SetInput( reader->GetOutput());

	// ������ ��actor
	vtkSmartPointer<vtkImageActor> shrinkActor =
		vtkSmartPointer<vtkImageActor>::New();
	shrinkActor->SetInput(shrinkFilter->GetOutput());

	// ������ ��actor
	vtkSmartPointer<vtkImageActor> magnifyActor =
		vtkSmartPointer<vtkImageActor>::New();
	magnifyActor->SetInput(magnifyFilter->GetOutput());

	double originalViewport[4] = {0.0, 0.0, 0.33, 1.0};
	double shrinkViewport[4] = {0.33, 0.0, 0.66, 1.0};
	double magnifyViewport[4] = {0.66, 0.0, 1.0, 1.0};

	vtkSmartPointer<vtkRenderer> originalRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	originalRenderer->SetViewport(originalViewport);
	originalRenderer->AddActor(originalActor);
	originalRenderer->ResetCamera();
	originalRenderer->SetBackground(1.0, 1.0, 1.0);

	// ������ �� renderer
	vtkSmartPointer<vtkRenderer> shrinkRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	shrinkRenderer->SetViewport(shrinkViewport);
	shrinkRenderer->AddActor(shrinkActor);
	shrinkRenderer->ResetCamera();
	shrinkRenderer->SetBackground(1.0, 1.0, 1.0);

	// ������ �� renderer
	vtkSmartPointer<vtkRenderer> magnifyRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	magnifyRenderer->SetViewport(magnifyViewport);
	magnifyRenderer->AddActor(magnifyActor);
	magnifyRenderer->ResetCamera();
	magnifyRenderer->SetBackground(1.0, 1.0, 1.0);

	// renderwindow �� ���renderer
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(originalRenderer);
	renderWindow->AddRenderer(shrinkRenderer);
	renderWindow->AddRenderer(magnifyRenderer);
	renderWindow->SetSize(640, 320);
	renderWindow->Render();
	renderWindow->SetWindowName("ImageShrink3DExample");

	// vtk �Ľ�����ʽ
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkInteractorStyleImage> style =
		vtkSmartPointer<vtkInteractorStyleImage>::New();

	renderWindowInteractor->SetInteractorStyle(style);
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Initialize();
	renderWindowInteractor->Start();
}
//////////////////////////////////////////////////////////////////////////
vtkMathematicsStrategy::vtkMathematicsStrategy()
{}

vtkMathematicsStrategy::~vtkMathematicsStrategy()
{}

void vtkMathematicsStrategy::AlgrithmInterface()
{
	vtkSmartPointer<vtkImageCanvasSource2D> imageSource = 
		vtkSmartPointer<vtkImageCanvasSource2D>::New();
	imageSource->SetNumberOfScalarComponents(3);
	imageSource->SetScalarTypeToUnsignedChar();
	imageSource->SetExtent(0, 4, 0, 4, 0, 0);
	imageSource->SetDrawColor(100.0, 0, 0);
	imageSource->FillBox(0, 4, 0, 4);
	imageSource->Update();

	vtkSmartPointer<vtkImageMathematics> imageMath = 
		vtkSmartPointer<vtkImageMathematics>::New();
	imageMath->SetOperationToMultiplyByK();		// ����ʹ����ѧ����Ϊ������ͼ��ֵ�����Գ���K. ���������Ϊ2.
	imageMath->SetConstantK(2.0);
	imageMath->SetInputConnection(imageSource->GetOutputPort());
	imageMath->Update();

	vtkSmartPointer<vtkImageActor> originalActor =
		vtkSmartPointer<vtkImageActor>::New();
	originalActor->SetInput(
		imageSource->GetOutput());

	vtkSmartPointer<vtkImageActor> mathActor =
		vtkSmartPointer<vtkImageActor>::New();
	mathActor->SetInput(imageMath->GetOutput());

	double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
	double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

	vtkSmartPointer<vtkRenderer> originalRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	originalRenderer->SetViewport(leftViewport);
	originalRenderer->AddActor(originalActor);
	originalRenderer->ResetCamera();
	originalRenderer->SetBackground(1.0, 1.0, 1.0);

	vtkSmartPointer<vtkRenderer> gradientMagnitudeRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	gradientMagnitudeRenderer->SetViewport(rightViewport);
	gradientMagnitudeRenderer->AddActor(mathActor);
	gradientMagnitudeRenderer->ResetCamera();
	gradientMagnitudeRenderer->SetBackground(1.0, 1.0, 1.0);

	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(originalRenderer);
	renderWindow->AddRenderer(gradientMagnitudeRenderer);
	renderWindow->SetSize(640, 480);
	renderWindow->Render();
	renderWindow->SetWindowName("ImageMathematicsExample");

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkInteractorStyleImage> style =
		vtkSmartPointer<vtkInteractorStyleImage>::New();

	renderWindowInteractor->SetInteractorStyle(style);
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Initialize();
	renderWindowInteractor->Start();
}
//////////////////////////////////////////////////////////////////////////
vtkMathLogicStrategy::vtkMathLogicStrategy()
{}

vtkMathLogicStrategy::~vtkMathLogicStrategy()
{}

void vtkMathLogicStrategy::AlgrithmInterface()
{
	vtkSmartPointer<vtkImageCanvasSource2D> imageSource1 =
		vtkSmartPointer<vtkImageCanvasSource2D>::New();
	imageSource1->SetScalarTypeToUnsignedChar();
	imageSource1->SetNumberOfScalarComponents(1);
	imageSource1->SetExtent(0, 100, 0, 100, 0, 0);
	imageSource1->SetDrawColor(0.0);
	imageSource1->FillBox(0,100,0,100);
	imageSource1->SetDrawColor(255);
	imageSource1->FillBox(20,60,20,60);
	imageSource1->Update();

	vtkSmartPointer<vtkImageCanvasSource2D> imageSource2 = 
		vtkSmartPointer<vtkImageCanvasSource2D>::New();
	imageSource2->SetNumberOfScalarComponents(1);
	imageSource2->SetScalarTypeToUnsignedChar();
	imageSource2->SetExtent(0, 100, 0, 100, 0, 0);
	imageSource2->SetDrawColor(0.0);
	imageSource2->FillBox(0,100,0,100);
	imageSource2->SetDrawColor(255.0);
	imageSource2->FillBox(40,80,40,80);
	imageSource2->Update();

	vtkSmartPointer<vtkImageLogic> imageLogic = 
		vtkSmartPointer<vtkImageLogic>::New();
	imageLogic->SetInput1(imageSource1->GetOutput());
	imageLogic->SetInput2(imageSource2->GetOutput());
	imageLogic->SetOperationToXor();		// ����������
	imageLogic->SetOutputTrueValue(50);		// ����������Ϊtrueʱ ������ֵ������2��ͼ������ز�ͬʱ����ʾΪ50
	imageLogic->Update();

	vtkSmartPointer<vtkImageActor> originalActor1 =
		vtkSmartPointer<vtkImageActor>::New();
	originalActor1->SetInput(imageSource1->GetOutput());

	vtkSmartPointer<vtkImageActor> originalActor2 =
		vtkSmartPointer<vtkImageActor>::New();
	originalActor2->SetInput(imageSource2->GetOutput());

	vtkSmartPointer<vtkImageActor> logicActor =
		vtkSmartPointer<vtkImageActor>::New();
	logicActor->SetInput(imageLogic->GetOutput());

	double leftViewport[4] = {0.0, 0.0, 0.33, 1.0};
	double midViewport[4] = {0.33, 0.0, 0.66, 1.0};
	double rightViewport[4] = {0.66, 0.0, 1.0, 1.0};

	vtkSmartPointer<vtkRenderer> originalRenderer1 =
		vtkSmartPointer<vtkRenderer>::New();
	originalRenderer1->SetViewport(leftViewport);
	originalRenderer1->AddActor(originalActor1);
	originalRenderer1->ResetCamera();
	originalRenderer1->SetBackground(1.0, 1.0, 1.0);

	vtkSmartPointer<vtkRenderer> originalRenderer2 =
		vtkSmartPointer<vtkRenderer>::New();
	originalRenderer2->SetViewport(midViewport);
	originalRenderer2->AddActor(originalActor2);
	originalRenderer2->ResetCamera();
	originalRenderer2->SetBackground(1.0, 1.0, 1.0);//(0.8, 0.8, 0.8);

	vtkSmartPointer<vtkRenderer> logicRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	logicRenderer->SetViewport(rightViewport);
	logicRenderer->AddActor(logicActor);
	logicRenderer->ResetCamera();
	logicRenderer->SetBackground(1.0, 1.0, 1.0);//(0.6, 0.6, 0.6);

	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(originalRenderer1);
	renderWindow->AddRenderer(originalRenderer2);
	renderWindow->AddRenderer(logicRenderer);
	renderWindow->SetSize(640, 320);
	renderWindow->Render();
	renderWindow->SetWindowName("ImageLogicExample");

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkInteractorStyleImage> style =
		vtkSmartPointer<vtkInteractorStyleImage>::New();

	renderWindowInteractor->SetInteractorStyle(style);
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Initialize();
	renderWindowInteractor->Start();
}
//////////////////////////////////////////////////////////////////////////
vtkImageThreholdStrategy::vtkImageThreholdStrategy()
{}

vtkImageThreholdStrategy::~vtkImageThreholdStrategy()
{}

void vtkImageThreholdStrategy::AlgrithmInterface()
{
	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetFileName ("..\\Bin\\Skin\\data\\slices1\\2");
	reader->Update();

	vtkSmartPointer<vtkImageThreshold> thresholdFilter =
		vtkSmartPointer<vtkImageThreshold>::New();
	thresholdFilter->SetInputConnection(reader->GetOutputPort());
	thresholdFilter->ThresholdByUpper(100); // ���ô�������ֵ100������Ϊ��Ч����
	thresholdFilter->SetInValue(255);		// ����Ч�����ڵ������ڣ�����Ϊ255
	thresholdFilter->SetOutValue(0);		// ����Ч�����ڵ������⣬����Ϊ0

	vtkSmartPointer<vtkImageActor> originalActor =
		vtkSmartPointer<vtkImageActor>::New();
	originalActor->SetInput(reader->GetOutput());

	vtkSmartPointer<vtkImageActor> binaryActor =
		vtkSmartPointer<vtkImageActor>::New();
	binaryActor->SetInput(thresholdFilter->GetOutput());

	double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
	double binaryviewport[4] = {0.5, 0.0, 1.0, 1.0};

	vtkSmartPointer<vtkRenderer> originalRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	originalRenderer->SetViewport(originalViewport);
	originalRenderer->AddActor(originalActor);
	originalRenderer->ResetCamera();
	originalRenderer->SetBackground(1.0, 1.0, 1.0);

	vtkSmartPointer<vtkRenderer> binaryRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	binaryRenderer->SetViewport(binaryviewport);
	binaryRenderer->AddActor(binaryActor);
	binaryRenderer->ResetCamera();
	binaryRenderer->SetBackground(1.0, 1.0, 1.0);

	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(originalRenderer);
	renderWindow->AddRenderer(binaryRenderer);
	renderWindow->SetSize(640, 320);
	renderWindow->Render();
	renderWindow->SetWindowName("ImageBinaryExample");

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkInteractorStyleImage> style =
		vtkSmartPointer<vtkInteractorStyleImage>::New();

	renderWindowInteractor->SetInteractorStyle(style);
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Initialize();
	renderWindowInteractor->Start();
}
//////////////////////////////////////////////////////////////////////////
vtkImageGradientStrategy::vtkImageGradientStrategy()
{}

vtkImageGradientStrategy::~vtkImageGradientStrategy()
{}

void vtkImageGradientStrategy::AlgrithmInterface()
{
	// �����ӣ������ݶ����ӣ������Ե���⡣
	// ����������sobel��Canny���ӡ�������˹���ӵ��㷨�����Բο��챦��
	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetFileName ("..\\Bin\\Skin\\data\\slices1\\2");
	reader->Update();

	vtkSmartPointer<vtkImageGradient> gradientFilter =
		vtkSmartPointer<vtkImageGradient>::New();
	gradientFilter->SetInputConnection(reader->GetOutputPort());
	gradientFilter->SetDimensionality(2);

	vtkSmartPointer<vtkImageMagnitude> magnitudeFilter =
		vtkSmartPointer<vtkImageMagnitude>::New();
	magnitudeFilter->SetInputConnection(gradientFilter->GetOutputPort());
	magnitudeFilter->Update();

	double range[2];
	magnitudeFilter->GetOutput()->GetScalarRange(range);

	vtkSmartPointer<vtkImageShiftScale> ShiftScale =
		vtkSmartPointer<vtkImageShiftScale>::New();
	ShiftScale->SetOutputScalarTypeToUnsignedChar();
	ShiftScale->SetScale( 255 / range[1] );
	ShiftScale->SetInputConnection(magnitudeFilter->GetOutputPort());
	ShiftScale->Update();

	vtkSmartPointer<vtkImageActor> originalActor =
		vtkSmartPointer<vtkImageActor>::New();
	originalActor->SetInput(reader->GetOutput());

	vtkSmartPointer<vtkImageActor> gradActor =
		vtkSmartPointer<vtkImageActor>::New();
	gradActor->SetInput(ShiftScale->GetOutput());

	double originalViewport[4] = {0.0, 0.0, 0.5, 1.0};
	double gradviewport[4] = {0.5, 0.0, 1.0, 1.0};

	vtkSmartPointer<vtkRenderer> originalRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	originalRenderer->SetViewport(originalViewport);
	originalRenderer->AddActor(originalActor);
	originalRenderer->ResetCamera();
	originalRenderer->SetBackground(1.0, 1.0, 1.0);

	vtkSmartPointer<vtkRenderer> gradRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	gradRenderer->SetViewport(gradviewport);
	gradRenderer->AddActor(gradActor);
	gradRenderer->ResetCamera();
	gradRenderer->SetBackground(1.0, 1.0, 1.0);

	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(originalRenderer);
	renderWindow->AddRenderer(gradRenderer);
	renderWindow->SetSize( 640, 320 );
	renderWindow->Render();
	renderWindow->SetWindowName("ImageGradientExample");

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkInteractorStyleImage> style =
		vtkSmartPointer<vtkInteractorStyleImage>::New();

	renderWindowInteractor->SetInteractorStyle(style);
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Initialize();
	renderWindowInteractor->Start();
}
//////////////////////////////////////////////////////////////////////////

vtkImageMeanFilterStrategy::vtkImageMeanFilterStrategy()
{}

vtkImageMeanFilterStrategy::~vtkImageMeanFilterStrategy()
{}

void vtkImageMeanFilterStrategy::AlgrithmInterface()
{
	// ��ʾ��Ϊʹ���˾�ֵ�˲���ͼ��ƽ������
	// ͼ��ƽ��������ٽ����ݶȼ��㣬���Լ����������ݶȵ�Ӱ�졣
	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetFileName ("..\\Bin\\Skin\\data\\slices1\\2");
	reader->Update();

	// ����ľ������Ҫ����ֵΪ�����ͣ����ԣ���Ҫ��ͼ���ֵ������ת��
	vtkSmartPointer<vtkImageCast> originalCastFilter =
		vtkSmartPointer<vtkImageCast>::New();
	originalCastFilter->SetInputConnection(reader->GetOutputPort());
	originalCastFilter->SetOutputScalarTypeToFloat();  // ����ת��
	originalCastFilter->Update();

	// ���þ��ģ��
	vtkSmartPointer<vtkImageConvolve> convolveFilter =
		vtkSmartPointer<vtkImageConvolve>::New();
	convolveFilter->SetInputConnection(originalCastFilter->GetOutputPort());

	double kernel[25] = {0.04,0.04,0.04,0.04,0.04,
		0.04,0.04,0.04,0.04,0.04,
		0.04,0.04,0.04,0.04,0.04,
		0.04,0.04,0.04,0.04,0.04,
		0.04,0.04,0.04,0.04,0.04 };
	convolveFilter->SetKernel5x5(kernel);
	convolveFilter->Update();

	// �����ɺ�Ϊ��ͼ����ʾ����Ҫ����ֵת��Ϊunsigned char���͡�
	vtkSmartPointer<vtkImageCast> convCastFilter = 
		vtkSmartPointer<vtkImageCast>::New();
	convCastFilter->SetInput(convolveFilter->GetOutput());
	convCastFilter->SetOutputScalarTypeToUnsignedChar();  // ����ת��
	convCastFilter->Update();

	vtkSmartPointer<vtkImageActor> originalActor =
		vtkSmartPointer<vtkImageActor>::New();
	originalActor->SetInput(reader->GetOutput());

	vtkSmartPointer<vtkImageActor> convolvedActor =
		vtkSmartPointer<vtkImageActor>::New();
	convolvedActor->SetInput(convCastFilter->GetOutput());

	double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
	double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

	vtkSmartPointer<vtkRenderer> originalRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	originalRenderer->SetViewport(leftViewport);
	originalRenderer->AddActor(originalActor);
	originalRenderer->SetBackground(1.0, 1.0, 1.0);
	originalRenderer->ResetCamera();

	vtkSmartPointer<vtkRenderer> convolvedRenderer =
		vtkSmartPointer<vtkRenderer>::New();
	convolvedRenderer->SetViewport(rightViewport);
	convolvedRenderer->AddActor(convolvedActor);
	convolvedRenderer->SetBackground(1.0, 1.0, 1.0);
	convolvedRenderer->ResetCamera();

	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();;
	renderWindow->AddRenderer(originalRenderer);
	renderWindow->AddRenderer(convolvedRenderer);
	renderWindow->SetSize(640, 320);
	renderWindow->Render();
	renderWindow->SetWindowName("MeanFilterExample");

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkInteractorStyleImage> style =
		vtkSmartPointer<vtkInteractorStyleImage>::New();

	renderWindowInteractor->SetInteractorStyle(style);
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Initialize();
	renderWindowInteractor->Start();
}
//////////////////////////////////////////////////////////////////////////
