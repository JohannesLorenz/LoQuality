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

#include <cstdio>
#include <QMessageBox>
#include <QVariant>
#include <QString>

#include "globals.h"
#include "dialogs.h"
#include "UpdateInfoDlg.h"
#include "SettingsReader.h"

void SettingsReader::shouldBe(const char *option_name,
	QVariant initial_value, bool first_start)
{
	if(globals::settings->value(option_name) == QVariant())
	{
		// on first start, do not inform user about updates.
		if(!first_start)
			QMessageBox::information(NULL, "Updated value",
				QString("Inserted option \"%1\" due to update. "
					"Default value \"%2\" will be used.")
					.arg(option_name, initial_value.toString()));
		globals::settings->setValue(option_name, initial_value);
	}
}

QString SettingsReader::getHomePath(bool type_is_music) {
	return QString(getenv("HOME")) + (type_is_music ? "/Music/" : "/Video/");
}

void SettingsReader::checkIntegrity(bool first_start)
{
	shouldBe("ffmpeg_fullpath", "/usr/bin/ffmpeg", first_start);
	shouldBe("youtubedl_fullpath", "/usr/bin/youtube-dl", first_start);
	shouldBe("mplayer_name", "mplayer2", first_start);
	shouldBe("music_root", getHomePath(true), first_start);
	shouldBe("video_root", getHomePath(false), first_start);
	shouldBe("main_database", "musicdb", first_start);
	shouldBe("number_of_cores", 2, first_start);
	shouldBe("update_interval_days", 1, first_start);
	shouldBe("do_updates", true, first_start);
	shouldBe("target", "pc", first_start);
	shouldBe("equalizer","0:0:0:0:0:0:0:0:0:0", first_start);
	shouldBe("restrict_to_ascii",true, first_start);
}

SettingsReader::SettingsReader()
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
	globals::VIDEO_ROOT = globals::settings->value("video_root").toString();
	globals::MPLAYER_EXE = globals::settings->value("mplayer_name").toString();
}
