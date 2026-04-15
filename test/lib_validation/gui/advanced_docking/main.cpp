#include <DockManager.h>
#include <DockWidget.h>

#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QStatusBar>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
	ads::CDockManager::setConfigFlag(ads::CDockManager::XmlCompressionEnabled, false);

	QMainWindow mainWindow;
	mainWindow.setWindowTitle(
		QStringLiteral("AdvancedDockingValidation — qt-advanced-docking-system"));
	mainWindow.resize(1024, 768);
	mainWindow.statusBar()->showMessage(
		QStringLiteral("qt-advanced-docking-system validation"));

	auto* dockManager = new ads::CDockManager(&mainWindow);

	auto* label1 =
		new QLabel(QStringLiteral("Panel 1 — ads OK\nDrag this panel to re-dock it."));
	label1->setAlignment(Qt::AlignCenter);
	auto* dock1 = new ads::CDockWidget(QStringLiteral("Panel 1"));
	dock1->setWidget(label1);
	dockManager->addDockWidget(ads::LeftDockWidgetArea, dock1);

	auto* label2 =
		new QLabel(QStringLiteral("Panel 2 — ads OK\nDrag this panel to re-dock it."));
	label2->setAlignment(Qt::AlignCenter);
	auto* dock2 = new ads::CDockWidget(QStringLiteral("Panel 2"));
	dock2->setWidget(label2);
	dockManager->addDockWidget(ads::RightDockWidgetArea, dock2);

	mainWindow.show();
	return app.exec();
}
