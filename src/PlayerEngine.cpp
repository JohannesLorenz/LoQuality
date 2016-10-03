/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2016                                               */
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
#include <cstdlib>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>

#include "taglib/tag.h"
#include "taglib/fileref.h"
#include "taglib/audioproperties.h"

#include "PlayerEngine.h"

#define _MPLAYER_REMOTE_PIPE "/tmp/lq_remote_pipe"

void PlayerEngine::exit_illegal_state(const char* action) {
	fputs("ERROR: illegal button input in current state\n", stderr);
	fprintf(stderr, "Current state: %d, action: %s\n",
		(int)player_status, action);
	exit(1);
}

void PlayerEngine::setStatus(STATUS_FLAGS new_status) {
	player_status = new_status;
	emit signalStatusChanged(new_status);
}

/*
	BUTTONS SLOTS
*/
void PlayerEngine::slotPlay(/*bool random, const QTableWidgetItem* item*/)
{
	puts("slotPlay()");
	if(!curSong) {
		slotForward(0,0, true);
	}
	else
	{
		// error: STATUS_PLAYING here after songover
		switch(player_status)
		{
			case STATUS_PAUSED:
				slotStartPlayback();
				break;
			case STATUS_STOPPED:
				// => we assume that LQ is stopped at the track you want to play next
			case STATUS_SONGOVER: // => this seams buggy to me...
				setStatus(STATUS_SONGLOADED); break;
			default: exit_illegal_state("slotPlay()");
		}
		//songTimer.start();
		//setStatus(STATUS_SONGLOADED);
	}
}

void PlayerEngine::slotPause()
{
	puts("slotPause()");
	switch(player_status)
	{
		case STATUS_PAUSED:
			slotStartPlayback();
			break;
		case STATUS_PLAYING:
			songTimer.stop();
			setStatus(STATUS_PAUSED);
			mPlayerConnection.pass_remote_command("pause\n");
			break;
		default: exit_illegal_state("slotPause()");
	}
}

void PlayerEngine::slotStop()
{
	puts("slotStop()");
	printf("status: %d\n",(int)player_status);
	if(player_status == STATUS_PAUSED)
	 mPlayerConnection.pass_remote_command("pause\n");
	mPlayerConnection.pass_remote_command("stop\n");

	setStatus(STATUS_STOPPED);
	songTimer.stop();
	emit signalUpdatePlaytime(100);
}

void PlayerEngine::slotStartPlayback()
{
	puts("slotStartPlayback()");
	switch(player_status)
	{
		case STATUS_SONGLOADED:
			mPlayerConnection.pass_remote_command((QString("loadfile \"%1\"\n").arg(curSong->text()).toAscii().data()));

			{
				TagLib::FileRef fp(curSong->text().toStdString().c_str());

				if(fp.isNull()) {
					printf("Warning: File not found.");
					return;
				}

				TagLib::AudioProperties* audio_props = fp.audioProperties();

				playTime = audio_props->length();
			}

			curTime = 0.0f;

			break;
		case STATUS_PAUSED:
			mPlayerConnection.pass_remote_command("pause\n");
			break;
		default:
			exit_illegal_state("slotStartPlayback()");
	}
	setStatus(STATUS_PLAYING);
	songTimer.start();
}

void PlayerEngine::slotForward(bool random, const QTableWidgetItem* item, bool forcePlay)
{
	puts("slotForward()");
	if(player_status == STATUS_PAUSED)
	 mPlayerConnection.pass_remote_command("pause\n"); // unpause song
	mPlayerConnection.pass_remote_command("stop\n");

//	setStatus(STATUS_STOPPED);
	songTimer.stop();
	emit signalUpdatePlaytime(0);

//	int numOfSongs = table->rowCount();
	if(curSong)
	 lastSongs.enqueue(curSong);

	if(item) { // user wishes a specified next song by double clicking
		curSong = item;
	}
	else if( nextSongs.isEmpty() )
	{
		int nextSongNumber;
		if(random)
		{
			// disallow playing a song that was lately played...
			do {
				// do some random...
				nextSongNumber = (int) (filterCount*1.0*rand()/(RAND_MAX+1.0));
				nextSongNumber += (time(NULL) % filterCount);
				nextSongNumber %= filterCount;
			} while (lastSongs.contains(table->item(nextSongNumber, 13)));

			if(table->rowCount() != filterCount) // translate value
			{
				int searchRow = -1;
				for(int i = -1; i < nextSongNumber; i++) {
					while(table->isRowHidden(++searchRow)) ;
				}

				nextSongNumber = searchRow;
			}

		}
		else {
			const int rowCount = table->rowCount();

			nextSongNumber = (curSong==NULL) ? 0 : ((curSong->row()) + 1);
			nextSongNumber %= rowCount;
			while(table->isRowHidden(nextSongNumber))
			 nextSongNumber = (nextSongNumber + 1) % rowCount;
		}

		printf("songNumber: %d, filterCount: %d\n",nextSongNumber, filterCount);


		curSong = table->item(nextSongNumber, 13);
	}
	else
	 curSong = nextSongs.dequeue();

	table->selectRow(curSong->row());

	if(forcePlay || player_status != STATUS_STOPPED)
	 setStatus(STATUS_SONGLOADED);
}

void PlayerEngine::slotBackward()
{
	if(player_status == STATUS_PAUSED)
	 mPlayerConnection.pass_remote_command("pause\n"); // unpause song

	if( ! lastSongs.isEmpty() )
	{
//		setStatus(STATUS_STOPPED);
		songTimer.stop();
		emit signalUpdatePlaytime(0);

		puts("NOT EMPTY!!");
		nextSongs.enqueue(curSong);
		curSong = lastSongs.dequeue();
		table->selectRow(curSong->row());

		if(player_status != STATUS_STOPPED)
		 setStatus(STATUS_SONGLOADED);
	}
}

void PlayerEngine::slotAddFile() {

}

void PlayerEngine::slotRemoveSong() {

}

void PlayerEngine::slotChangeVolume (int newValue)
{
	char remote_command[32];
	sprintf(remote_command, "volume %f 100\n", (float)newValue);
	mPlayerConnection.pass_remote_command(remote_command);
}

void PlayerEngine::slotTimerTimeout()
{
	curTime += 1.0f;
	printf("playTime: %f, curTime: %f\n",playTime, curTime);
	emit signalUpdatePlaytime((int)(100.0f * curTime/playTime));
	if(curTime >= playTime && curTime >= 3.0f) { // let mplayer 3 seconds to get playTime
		//emit signalStatusChanged(STATUS_SONGOVER);
		setStatus(STATUS_SONGOVER);
	}
}


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

PlayerEngine::PlayerEngine (QTableWidget* _table) : table(_table), curSong(NULL)
{
	setStatus(STATUS_STOPPED);

	srandom((unsigned int)time(NULL) + getpid() + getppid() + 42);

	songTimer.setInterval(1000);
	QObject::connect(&songTimer, SIGNAL(timeout()), this, SLOT(slotTimerTimeout()));
}



