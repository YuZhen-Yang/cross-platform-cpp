#include <QApplication>
#include <QMainWindow>
#include <QStatusBar>
#include <QString>

// Log4Qt
#include <log4qt/logger.h>
#include <log4qt/logmanager.h>
#include <log4qt/consoleappender.h>
#include <log4qt/fileappender.h>
#include <log4qt/ttcclayout.h>

// VTK
#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <QVTKOpenGLNativeWidget.h>

static Log4Qt::Logger* logger = Log4Qt::Logger::logger("main");

static void initLogger()
{
	auto* layout = new Log4Qt::TTCCLayout();
	layout->setName("layout");
	layout->activateOptions();

	auto* consoleAppender =
		new Log4Qt::ConsoleAppender(layout, Log4Qt::ConsoleAppender::STDOUT_TARGET);
	consoleAppender->setName("console");
	consoleAppender->activateOptions();

	auto* fileAppender = new Log4Qt::FileAppender(layout, "app.log", true);
	fileAppender->setName("file");
	fileAppender->activateOptions();

	auto* root = Log4Qt::Logger::rootLogger();
	root->addAppender(consoleAppender);
	root->addAppender(fileAppender);
	root->setLevel(Log4Qt::Level::DEBUG_INT);

	Log4Qt::LogManager::setHandleQtMessages(true);
}

static void shutdownLogger()
{
	auto* root = Log4Qt::Logger::rootLogger();
	root->removeAllAppenders();
	root->loggerRepository()->shutdown();
}

int main(int argc, char* argv[])
{
	QVTKOpenGLNativeWidget::initializeOpenGLFunctions();
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

	QApplication app(argc, argv);
	app.setApplicationName("CrossPlatformTest");
	app.setApplicationVersion("0.1.0");

	initLogger();

	logger->info("Application started: Qt %1, VTK %2, Log4Qt %3",
				 QString(qVersion()),
				 QString(vtkVersion::GetVTKVersionFull()),
				 QString(Log4Qt::LOG4QT_VERSION_STR));

	// --- VTK pipeline ---
	vtkNew<vtkConeSource> cone;
	cone->SetResolution(64);

	vtkNew<vtkPolyDataMapper> mapper;
	mapper->SetInputConnection(cone->GetOutputPort());

	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);

	vtkNew<vtkRenderer> renderer;
	renderer->AddActor(actor);
	renderer->SetBackground(0.1, 0.2, 0.4);
	renderer->ResetCamera();

	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	renderWindow->AddRenderer(renderer);

	// --- Qt window ---
	QMainWindow mainWindow;
	mainWindow.setWindowTitle("Qt + VTK + Log4Qt — Minimal Demo");
	mainWindow.resize(800, 600);

	auto* vtkWidget = new QVTKOpenGLNativeWidget(&mainWindow);
	vtkWidget->setRenderWindow(renderWindow);
	mainWindow.setCentralWidget(vtkWidget);
	mainWindow.statusBar()->showMessage(
		"Rotate: left-drag | Zoom: scroll | Pan: middle-drag");

	mainWindow.show();
	logger->info("Main window shown — entering event loop");

	int ret = QApplication::exec();

	logger->info("Event loop exited with code %1", ret);
	shutdownLogger();
	return ret;
}