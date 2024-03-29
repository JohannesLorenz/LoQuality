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
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <QMessageBox>
#include <QThread>

#include "globals.h"
#include "MPlayerConnection.h"

int MPlayerConnection::connection_count = 0;

bool MPlayerConnection::pass_remote_command(const char* command) const
{
	// NOTE: mplayer -input cmdlist
	FILE* fp = fopen(REMOTE_PIPE_NAME, "w");
	if(fp == NULL) {
		QMessageBox::information(NULL, "Verbinden fehlgeschlagen", "Konnte keine Lese-Verbindung zu MPlayer aufbauen - sorry...");
		return false;
	}
	printf("passing remote command to MPlayer: %s\n",command);
	fputs(command, fp);
	fclose(fp);
	return true;
}

QString MPlayerConnection::fetchValue(const char* remoteCommand, const char* ansStr, bool msgBoxOnErr)
{
	static char line[256];
	*line = 0;
	while(read(mplayer_output, line, 255)>0) ; // read any rubbish
	pass_remote_command( remoteCommand );

	// wait for a reply
	fd_set rfds;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_SET(mplayer_output, &rfds);

	int ret_val = select(mplayer_output+1, &rfds, NULL, NULL, &tv);
	if(ret_val == -1) {
	 if(msgBoxOnErr)
	  QMessageBox::information(NULL, "Sorry...", "... select() ging nicht!");
	}
	else if(ret_val == 0) {
	 if(msgBoxOnErr)
	  QMessageBox::information(NULL, "Sorry...", "... mplayer antwortet nicht!");
	}
	else for(unsigned int ret; (ret = read(mplayer_output, line, 255))>0; ) { // TODO : ANS_LENGTH can be parted into two buffers
		line[255] = 0;
		line[ret] = 0;
		printf("ANSWER: %s, ret = %d\n",strstr(line, ansStr),ret);
		const char* begin = strstr(line, ansStr);
		if(ret > strlen(ansStr) && begin) {
			QString retString(begin+strlen(ansStr));
			retString.resize(retString.indexOf('\n'));
			printf("string : %s\n", retString.toLatin1().data());
			return retString;
		}
		else {
			if(msgBoxOnErr)
			 QMessageBox::information(NULL, "Sorry...", "... Kommunikation mit mplayer schlug fehl!");
			break;
		}
	}

	return QString("''");
}

MPlayerConnection::MPlayerConnection(bool nulldevice)
{

	strcpy(REMOTE_PIPE_NAME, "/tmp/lq_remote_pipe_");
	snprintf(REMOTE_PIPE_NAME + strlen(REMOTE_PIPE_NAME), 16, "%d", ++connection_count);

	if( access( REMOTE_PIPE_NAME, F_OK ) != -1 )
	 remove(REMOTE_PIPE_NAME);
	mkfifo(REMOTE_PIPE_NAME, 0600);

	int pipefd[2];

	if (pipe(pipefd) == -1) {
		QMessageBox::information(NULL, "Sorry...", "... pipe() ging nicht, kein MPlayer!");
		return;
	}
	fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

	// fork MPlayer
	printf("MPLAYER EXE: %s\n",globals::MPLAYER_EXE.toLatin1().data());
	mplayers_pid=fork();
	if(mplayers_pid < 0) {
		QMessageBox::information(NULL, "Sorry...", "... fork() ging nicht, kein MPlayer!");
		return;
	}
	else if(mplayers_pid == 0) {

		close(pipefd[0]); /* Close unused read end */

		dup2(pipefd[1], STDOUT_FILENO);

		const char* _MPLAYER_EXE = globals::MPLAYER_EXE.toLatin1().data();
		printf("%s -input-file %s -idle -msg-level all=no -vo null\n",_MPLAYER_EXE,REMOTE_PIPE_NAME);

		const QString& eq = globals::settings->value("equalizer","0:0:0:0:0:0:0:0:0:0").toString();
		if(nulldevice)
		 execlp(_MPLAYER_EXE, _MPLAYER_EXE, "-input-file", REMOTE_PIPE_NAME, "-idle", "-msg-level", "all=no", "-ao", "null", "-vo", "null", "-af", ("equalizer="+eq).toLatin1().data(), NULL);
		else
		 execlp(_MPLAYER_EXE, _MPLAYER_EXE, "-input-file", REMOTE_PIPE_NAME, "-idle", "-msg-level", "all=no", "-vo", "null", "-af", ("equalizer="+eq).toLatin1().data(), NULL);

		close(pipefd[1]); /* Reader will see EOF */
		exit(0);
	}

	close(pipefd[1]); /* Close unused write end */
	mplayer_output = pipefd[0];
}

MPlayerConnection::~MPlayerConnection()
{
	// shut down MPlayer
	pass_remote_command("stop\n");
	pass_remote_command("quit\n");
	waitpid(mplayers_pid, NULL, WCONTINUED); // wait for MPlayer to not shoot our console!

	close(mplayer_output);
	remove(REMOTE_PIPE_NAME);
}
