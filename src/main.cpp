/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2012                                               */
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

#include "MainWindow.h"
#include "params.h"

#include <QApplication>
#include <QMessageBox>

class SettingsReader // todo: extra file?
{
	inline void shouldBe(const char* option_name, QVariant initial_value, bool first_start = false)
	{
		if(globals::settings->value(option_name) == QVariant())
		{
			// on first start, do not inform user about updates.
			if(!first_start)
			QMessageBox::information(NULL, "Updated value",
				QString("Inserted option \"%1\" due to update. "
					"Default value \"%2\" will be used.").arg(option_name, initial_value.toString()));
			globals::settings->setValue(option_name, initial_value);
		}
	}

	void checkIntegrity(bool first_start = false)
	{
		shouldBe("ffmpeg_fullpath", "/usr/bin/ffmpeg", first_start);
		shouldBe("youtubedl_fullpath", "/usr/bin/youtube-dl", first_start);
		shouldBe("mplayer_name", "mplayer2", first_start);
		shouldBe("music_root", "insert_your_path", first_start);
		shouldBe("main_database", "musicdb", first_start);
		shouldBe("number_of_cores", 2, first_start);
		shouldBe("update_interval_days", 1, first_start);
		shouldBe("do_updates", true, first_start);
		shouldBe("target", "pc", first_start);
		shouldBe("equalizer","0:0:0:0:0:0:0:0:0:0", first_start);
		shouldBe("restrict_to_ascii",true, first_start);
	}

public:
	SettingsReader()
	{
		const bool first_start = globals::settings->value("first_start", true).toBool();
		if(first_start) {
			globals::settings->setValue("first_start", false);
			QMessageBox::information(NULL, "Please close LQ and then edit:", globals::settings->fileName().toAscii().data());
		}
		if(globals::settings->value("update_applied").toBool()) {
			UpdateInfoDlg u;
			u.show();
			u.exec();
			globals::settings->setValue("update_applied", false);
		}
		printf("Writing options to %s\n",
			globals::settings->fileName().toAscii().data());
		checkIntegrity(first_start);

		if(first_start) {
			const bool mobile = question("Is this a mobile?",
			"Are you running this on your mobile phone?");
			globals::settings->setValue("target",
				(mobile)?"mobile":"pc");
		}

		globals::MUSIC_ROOT = globals::settings->value("music_root").toString();
		globals::MPLAYER_EXE = globals::settings->value("mplayer_name").toString();
	}
};

int main(int argc, char** argv)
{
	if (read_params(argc, argv) == -1)
	 return 0;

	int return_value = 0;

	// pass arguments to Qt first
	QApplication app(argc, argv);
	app.setOrganizationName("LoQuality");
	app.setOrganizationDomain("github.com/JohannesLorenz/LoQuality");
	app.setApplicationName("LoQuality");

	try {
		DBusConnector dbus_connector;

		// start program
		globals::settings = new QSettings();
		SettingsReader settingsReader;

		const QString main_database = (globals::settings->value("main_database", "musicdb").toString());

		MainWindowContainer mwContainer;
		MainWindow* firstMainWindow = mwContainer.openNewWindow(main_database);

		// dbus_connector shall for now only be connected to the first main window
		dbus_connector.start(firstMainWindow->get_player());

		return_value = app.exec();
		delete globals::settings;

	} catch (std::exception e) {
		QMessageBox::warning(NULL, "Error", e.what());
	} catch(...) {
		QMessageBox::warning(NULL, "Error", "*** TODO: unknown exception");
	}

	return return_value;
}
