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

#include <cstdlib>
#include <QDir>
#include "globals.h"
#include "flash_tools.h"

void addToFileList(QList<QFileInfo>* allFiles, const QString& basedir)
{
	QDir tmpDir(basedir, /*"Flash*",*/"", QDir::Time, QDir::Files);
	QFileInfoList files = tmpDir.entryInfoList();

	for(QList<QFileInfo>::const_iterator itr = files.begin(); itr != files.end(); itr++)
	 if( itr->canonicalFilePath().contains("Flash"))
	  allFiles->append(*itr);
}

void getFileList(QList<QFileInfo>* allFiles)
{
	QDir dir("/proc/");
	QFileInfoList list = dir.entryInfoList();
	for (int iList=0;iList<list.count();iList++)
	{
		QFileInfo info = list[iList];
		QString sFilePath = info.filePath();
		if (info.isDir())
		{
			if (info.fileName()!=".." && info.fileName()!=".")
			{
				QFile fp(sFilePath+"/cmdline");
				if( fp.open(QIODevice::ReadOnly))
				{
					QByteArray line = fp.readLine();
					fp.close();
					if(line.contains("flashplayer"))
					 addToFileList(allFiles, sFilePath+"/fd");
				}
			}
		}
	}
}

pid_t startOggConvertion(const char *infile, const char* outfile)
{
	// fork ffmpeg
	const pid_t ffmpegs_pid=fork();
	if(ffmpegs_pid == 0) {
		// ffmpeg -i flashfile -vn -acodec libmp3lame -y -ar 44100 -ab 128000 outfilename
		//execlp("ffmpeg", "ffmpeg", "-i", strchr((*itr)->text().toAscii().data(), ':') + 2, "-vn", "-acodec", "libmp3lame", "-y", "-ar", "44100", "-ab", "128000", curOutName.toAscii().data(), NULL);

		/*
			We need at most audio quality 6 = approx. 128 kbps. See:
			http://en.wikipedia.org/wiki/YouTube#Quality_and_codecs
			http://en.wikipedia.org/wiki/Vorbis
		*/
		// note: you might want to specify number of threads with "-threads n"
		// ffmpeg -i flashfile -vn -y -ar 44100 -aq 6 -acodec libvorbis -threads THREADNUM outfile
		const QString CPU_THREADS = globals::settings->value("number_of_cores",2).toString();
		const QString ffmpeg_fullpath = globals::settings->value("ffmpeg_fullpath").toString();
		//printf("%s -i %s ... -threads %s %s",,,CPU_THREADS, curOutName.toAscii().data())
		execlp(ffmpeg_fullpath.toAscii().data(), "ffmpeg", "-i", infile, "-vn", "-y", "-ar", "44100", "-aq", "6", "-acodec", "libvorbis", "-threads", CPU_THREADS.toAscii().data(), outfile, NULL);

		exit(0);
	}
	else
		return ffmpegs_pid; // -1 for error or ffmpegs pid
}

bool ForkedProcess::begin(pid_t _remote_pid, unsigned int msecs)
{
	if(_remote_pid == -1) // error calling fork()
	 return false;
	else
	{
		remote_pid = _remote_pid;
		lookupTimer.start(msecs);
		return true;
	}
}

void ForkedProcess::slotTimerTimeout()
{
	puts("TIMEOUT!");
	if( 0 != waitpid(remote_pid, NULL, WNOHANG) )  {
		puts("WAIT FINISHED!");
		lookupTimer.stop();
		emit finished();
	}
}

bool YouTubeDlSession::download(const char *url, const char *dest)
{
	//youtube-dl -w -cit --max-quality 60 --extract-audio --audio-format vorbis --audio-quality 320k <url>
	const pid_t _youtubedls_pid=fork();
	if(_youtubedls_pid == 0) {
		const QString youtubedl_fullpath = globals::settings->value("youtubedl_fullpath").toString();
		execlp(youtubedl_fullpath.toAscii().data(), "youtube-dl", "-w", "-cit", "--max-quality", "60", "--extract-audio", "--audio-format", "best", "--audio-quality", "320k", url, NULL);
		exit(0);
	}
	else
		return begin(_youtubedls_pid);
}




