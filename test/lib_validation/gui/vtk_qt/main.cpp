#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <QApplication>
#include <QDockWidget>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>

#include <log4qt/consoleappender.h>
#include <log4qt/fileappender.h>
#include <log4qt/loggerrepository.h>
#include <log4qt/logmanager.h>
#include <log4qt/logger.h>
#include <log4qt/patternlayout.h>

#include <cmath>
#include <cstdlib>
#include <random>

// ---------------------------------------------------------------------------
// Log4Qt helpers
// ---------------------------------------------------------------------------
static void initLogger()
{
	auto* logger = Log4Qt::Logger::rootLogger();

	Log4Qt::LayoutSharedPtr layout(new Log4Qt::PatternLayout(
		QStringLiteral("%d{yyyy-MM-dd HH:mm:ss.zzz} [%p] %c - %m%n")));
	layout->setName(QStringLiteral("MainLayout"));
	layout->activateOptions();

	auto* console =
		new Log4Qt::ConsoleAppender(layout, Log4Qt::ConsoleAppender::STDOUT_TARGET);
	console->setName(QStringLiteral("ConsoleAppender"));
	console->activateOptions();
	logger->addAppender(console);

	auto* file =
		new Log4Qt::FileAppender(layout,
								 QCoreApplication::applicationDirPath() + "/app.log",
								 /*append=*/true);
	file->setName(QStringLiteral("FileAppender"));
	file->activateOptions();
	logger->addAppender(file);

	logger->setLevel(Log4Qt::Level::DEBUG_INT);
	Log4Qt::LogManager::setHandleQtMessages(true);

	logger->info(QStringLiteral("Logger initialized"));
}

static void shutdownLogger()
{
	auto* logger = Log4Qt::Logger::rootLogger();
	logger->info(QStringLiteral("Logger shutting down"));
	logger->removeAllAppenders();
	logger->loggerRepository()->shutdown();
}

// ---------------------------------------------------------------------------
// VTK helpers
// ---------------------------------------------------------------------------
namespace {

	void randomize(vtkSphereSource* sphere,
				   vtkDataSetMapper* mapper,
				   vtkGenericOpenGLRenderWindow* window,
				   std::mt19937& rng)
	{
		auto* logger = Log4Qt::Logger::rootLogger();
		logger->debug(QStringLiteral("Randomizing sphere"));

		double amp = 0.2 + ((rng() % 1000) / 1000.0) * 0.2;
		double thetaFreq = 1.0 + (rng() % 9);
		double phiFreq = 1.0 + (rng() % 9);

		sphere->Update();
		vtkSmartPointer<vtkPolyData> mesh;
		mesh.TakeReference(sphere->GetOutput()->NewInstance());
		mesh->DeepCopy(sphere->GetOutput());

		vtkNew<vtkDoubleArray> height;
		height->SetName("Height");
		height->SetNumberOfComponents(1);
		height->SetNumberOfTuples(mesh->GetNumberOfPoints());
		mesh->GetPointData()->AddArray(height);

		for (int i = 0; i < mesh->GetNumberOfPoints(); ++i)
		{
			double pt[3] = {};
			mesh->GetPoint(i, pt);
			double theta = std::atan2(pt[1], pt[0]);
			double phi = std::atan2(pt[2], std::sqrt(pt[0] * pt[0] + pt[1] * pt[1]));
			double h = amp * std::cos(thetaFreq * theta) * std::sin(phiFreq * phi);
			height->SetValue(i, h);
			pt[0] += h * std::cos(theta) * std::cos(phi);
			pt[1] += h * std::sin(theta) * std::cos(phi);
			pt[2] += h * std::sin(phi);
			mesh->GetPoints()->SetPoint(i, pt);
		}
		mesh->GetPointData()->SetScalars(height);

		mapper->SetInputDataObject(mesh);
		mapper->SetScalarModeToUsePointData();
		mapper->ColorByArrayComponent("Height", 0);
		window->Render();

		logger->info(QStringLiteral("Sphere randomized: amp=%1 thetaFreq=%2 phiFreq=%3")
						 .arg(amp, 0, 'f', 3)
						 .arg(thetaFreq)
						 .arg(phiFreq));
	}

} // namespace

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

	QApplication app(argc, argv);

	initLogger();

	auto* logger = Log4Qt::Logger::rootLogger();
	logger->info(QStringLiteral("MinimalQtVTKApp starting"));

	// Main window
	QMainWindow mainWindow;
	mainWindow.resize(1200, 900);
	mainWindow.setWindowTitle(QStringLiteral("MinimalQtVTKApp"));

	// Control dock
	QDockWidget controlDock;
	mainWindow.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

	QLabel dockTitle(QStringLiteral("Controls"));
	dockTitle.setMargin(20);
	controlDock.setTitleBarWidget(&dockTitle);

	QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
	QWidget dockContainer;
	dockContainer.setLayout(dockLayout);
	controlDock.setWidget(&dockContainer);

	QPushButton randomizeButton(QStringLiteral("Randomize"));
	dockLayout->addWidget(&randomizeButton);

	// VTK render widget
	QPointer<QVTKOpenGLNativeWidget> vtkWidget = new QVTKOpenGLNativeWidget();
	mainWindow.setCentralWidget(vtkWidget);

	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	vtkWidget->setRenderWindow(renderWindow.Get());

	vtkNew<vtkSphereSource> sphere;
	sphere->SetRadius(1.0);
	sphere->SetThetaResolution(100);
	sphere->SetPhiResolution(100);

	vtkNew<vtkDataSetMapper> mapper;
	mapper->SetInputConnection(sphere->GetOutputPort());

	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);
	actor->GetProperty()->SetEdgeVisibility(true);
	actor->GetProperty()->SetRepresentationToSurface();

	vtkNew<vtkRenderer> renderer;
	renderer->AddActor(actor);
	renderWindow->AddRenderer(renderer);

	std::mt19937 rng(0);
	::randomize(sphere, mapper, renderWindow, rng);

	QObject::connect(&randomizeButton, &QPushButton::released, [&]() {
		::randomize(sphere, mapper, renderWindow, rng);
	});

	logger->info(QStringLiteral("Entering event loop"));
	mainWindow.show();
	int ret = app.exec();

	logger->info(QStringLiteral("Event loop exited with code %1").arg(ret));
	shutdownLogger();
	return ret;
}