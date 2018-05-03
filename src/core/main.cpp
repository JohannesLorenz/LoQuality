/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2018                                               */
/* Johannes Lorenz, Philipp Lorenz                                       */
/* https://github.com/JohannesLorenz/LoQuality                           */
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

#include <csignal>

#include <QApplication>
#include <QMessageBox>

#include "params.h" // -> core
#include "core/SettingsReader.h"
#include "dbus/DBusConnector.h"
#include "SqlHelper.h"

void int_callback(int s){
	printf("Caught signal %d.\n",s);
	printf("Ignoring. Please close LQ correctly.\n");
	printf("You need to kill mplayer now!\n");
}

void init_signals()
{
	struct sigaction sig_handler;

	sig_handler.sa_handler = int_callback;
	sigemptyset(&sig_handler.sa_mask);
	sig_handler.sa_flags = 0;

	sigaction(SIGINT, &sig_handler, NULL);
}

int main(int argc, char** argv)
{
	if (read_params(argc, argv) == -1)
	 return 0;

	init_signals();

	int return_value = 0;

	// pass arguments to Qt first
	QApplication app(argc, argv);
	app.setOrganizationName("LoQuality");
	app.setOrganizationDomain("github.com/JohannesLorenz/LoQuality");
	app.setApplicationName("LoQuality");

	try {
		DBusConnector dbus_connector;

		/*
			Initialize Options
		*/
		globals::settings = new QSettings();
		SettingsReader settingsReader; // checks integrity

		// TODO: filename!!!
		globals::private_db = new SqlHelper("privatedb");

		const QString main_database = (globals::settings->
			value("main_database", "musicdb").toString());

		/*
			Initialize MainWindows
		*/
		MainWindowContainer mwContainer;
		mwContainer.openNewWindow(main_database);

		// dbus_connector shall for now only be connected to
		// the first main window
		dbus_connector.start(&mwContainer);

		/*
			Run
		*/
		return_value = app.exec();

		/*
			Cleanups
		*/
		delete globals::private_db;
		delete globals::settings;

	} catch (std::exception e) {
		QMessageBox::warning(NULL, "Error", e.what());
	} catch(...) {
		QMessageBox::warning(NULL, "Error",
			"*** TODO: unknown exception");
	}

	return return_value;
}
