/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2012                                               */
/* Johannes Lorenz, Philipp Lorenz                                       */
/* https://github.com/DrSegfault/LoQuality                               */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#include "MainWindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char** argv)
{
	// pass arguments to Qt first
	QApplication app(argc, argv);
	app.setOrganizationName("LoQuality");
	app.setOrganizationDomain("github.com/DrSegfault/LoQuality");
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
