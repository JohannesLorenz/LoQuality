// TODO: License

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

#include "PlayerEngine.h"

#define _MPLAYER_REMOTE_PIPE "/tmp/lq_remote_pipe"
#define _MPLAYER_OUTPUT_PIPE "/tmp/lq_output_pipe"

void PlayerEngine::setStatus(STATUS_FLAGS new_status) {
	player_status = new_status;
	emit signalStatusChanged(new_status);
}

/*
	BUTTONS SLOTS
*/
void PlayerEngine::slotPlay(bool random, const QTableWidgetItem* item)
{
	if(player_status == STATUS_PAUSED && !item) { // the !item is important, if next song was deserved, ignore the pause...
		mPlayerConnection.pass_remote_command("pause\n"); // unpause song
		setStatus(STATUS_PLAYING);
		songTimer.start();
	}
	else slotForward(random, item);
}

void PlayerEngine::slotPause() {
	mPlayerConnection.pass_remote_command("pause\n");
	setStatus(STATUS_PAUSED);
	songTimer.stop();
}

void PlayerEngine::slotStop() {
	mPlayerConnection.pass_remote_command("stop\n");
	setStatus(STATUS_STOPPED);
	songTimer.stop();
	emit signalUpdatePlaytime(100);
}

void PlayerEngine::startSong()
{
	mPlayerConnection.pass_remote_command((QString("loadfile \"%1\"\n").arg(curSong->text()).toAscii().data()));
	setStatus(STATUS_PLAYING);

	playTime = 0.0f;
	playTime = mPlayerConnection.fetchValue("get_time_length\n", "ANS_LENGTH=", true).toFloat();
	curTime = 0.0f;
	songTimer.start();
}

void PlayerEngine::slotForward(bool random, const QTableWidgetItem* item)
{
	setStatus(STATUS_STOPPED);
	songTimer.stop();
	emit signalUpdatePlaytime(0);

	int numOfSongs = table->rowCount();
	if(curSong)
	 lastSongs.enqueue(curSong);

	if(item) { // user wishes a specified next song by double clicking
		// TODO: could not we simply push it to the nextSongs list?
		curSong = item;
		table->selectRow(curSong->row());
	}
	else if( nextSongs.isEmpty() )
	{
		int nextSongNumber;
		if(random)
		{
			// disallow playing a song that was lately played...
			do {
				// do some random...
				nextSongNumber = (int) (numOfSongs*1.0*rand()/(RAND_MAX+1.0));
				nextSongNumber += (time(NULL) % numOfSongs);
				nextSongNumber %= numOfSongs;
			} while (lastSongs.contains(table->item(nextSongNumber, 13)));
		}
		else
		 nextSongNumber = (curSong==NULL) ? 0 : ((curSong->row()) + 1) % (table->rowCount());

		printf("songNumber: %d\n",nextSongNumber);

		curSong = table->item(nextSongNumber, 13);
		table->selectRow(nextSongNumber);
	}
	else
	{
		curSong = nextSongs.dequeue();
		table->selectRow(curSong->row());
	}

	startSong();

	puts("DONE");

}

void PlayerEngine::slotBackward()
{
	if( ! lastSongs.isEmpty() )
	{
		setStatus(STATUS_STOPPED);
		songTimer.stop();
		emit signalUpdatePlaytime(0);

		puts("NOT EMPTY!!");
		nextSongs.enqueue(curSong);
		curSong = lastSongs.dequeue();
		table->selectRow(curSong->row());

		startSong();
	}
}

void PlayerEngine::slotAddFile() {

}

void PlayerEngine::slotRemoveSong() {

}

void PlayerEngine::slotChangeVolume (int newValue) {
	char remote_command[32];
	sprintf(remote_command, "volume %f 100\n", (float)newValue);
	mPlayerConnection.pass_remote_command(remote_command);
}

void PlayerEngine::slotTimerTimeout() {
	curTime += 1.0f;
	printf("playTime: %f, curTime: %f\n",playTime, curTime);
	emit signalUpdatePlaytime((int)(100.0f * curTime/playTime));
	if(curTime >= playTime && curTime >= 3.0f) // let mplayer 3 seconds to get playTime
	 emit signalStatusChanged(STATUS_SONGOVER);
}


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

PlayerEngine::PlayerEngine (QTableWidget* _table) : table(_table), curSong(NULL), mPlayerConnection(_MPLAYER_REMOTE_PIPE)
{
	setStatus(STATUS_STOPPED);

	srandom((unsigned int)time(NULL) + getpid() + getppid() + 42);

	songTimer.setInterval(1000);
	QObject::connect(&songTimer, SIGNAL(timeout()), this, SLOT(slotTimerTimeout()));
}

