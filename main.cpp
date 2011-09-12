#include "MainWindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char** argv)
{
	// pass arguments to Qt first
	QApplication app(argc, argv);
	app.setOrganizationName("WeWantYourMoney");
	app.setOrganizationDomain("TODO.com");
	app.setApplicationName("LoQuality");

	try {
		// start program
		MainWindow mainwindow;
		mainwindow.show();
		return app.exec();

	} catch (std::exception e) {
		QMessageBox::warning(NULL, "Error", e.what());
	} catch(...) {
		QMessageBox::warning(NULL, "Error", "*** TODO: unknown exception");
	}
	return 0;
}
