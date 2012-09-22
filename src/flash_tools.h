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

#ifndef FLASH_TOOLS_H
#define FLASH_TOOLS_H

#include <QObject>
#include <QList>
#include <QString>
#include <QFileInfo>
#include <QTimer>
#include <sys/wait.h>

//! adds all flash file infos from @a basedir to @a allFiles
void addToFileList(QList<QFileInfo>* allFiles, const QString& basedir);
//! adds all flash file infos to @a allFiles
void getFileList(QList<QFileInfo>* allFiles);

/**
	Takes flv video and converts it to ogg file
	@param infile flv video
	@param outfile ogg file name, should end on .ogg
	@return pid to wait for until ffmpeg finishes
*/
pid_t startOggConvertion(const char *infile, const char* outfile);

inline bool oggConvertionFinished(pid_t ffmpegs_pid) {
	return 0 != waitpid(ffmpegs_pid, NULL, WNOHANG);
}

/**
	@class ForkedProcess
	@brief Class to keep track of a downloaded process.

	This class can be reused.
*/
class ForkedProcess : public QObject
{
	Q_OBJECT
private:
	QTimer lookupTimer;
	pid_t remote_pid;
private slots:
	void slotTimerTimeout();
public:
	inline ForkedProcess() {
		connect(&lookupTimer, SIGNAL(timeout()), this, SLOT(slotTimerTimeout()));
	}
	bool begin(pid_t _remote_pid, unsigned int msecs = 1000);
signals:
	void finished(void);
};

class OggConvertion : public ForkedProcess // NOTE: THIS CLASS IS NOT FINISHED YET
{
public:
	inline bool start(const char *infile, const char* outfile) {
		return begin(startOggConvertion(infile, outfile));
	}
};

class YouTubeDlSession : public ForkedProcess
{
public:
	bool download(const char* url, const char* dest, const char *audio_format = "best");
};

inline void unpack_playlist()
{

}

#endif // FLASH_TOOLS_H
