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

#ifndef _PLAYER_ENGINE_H_
#define _PLAYER_ENGINE_H_ _PLAYER_ENGINE_H_

#include <QObject>
#include <QTimer>
class QTableWidget;
class QTableWidgetItem;

#include "MPlayerConnection.h"
#include "PlayListQueue.h"

/**
 * @class PlayerEngine
 * @brief The player engine
 *
 * This class is responsible for media playing, queuing, options etc.
 * The MPlayer wrapper is found here, too.
 */
class PlayerEngine : public QObject
{
	Q_OBJECT

	public:
		enum STATUS_FLAGS
		{
			STATUS_STOPPED,
			STATUS_PLAYING,
			STATUS_SONGOVER,
			STATUS_PAUSED
		};
	private:
		void switchPause();

	public slots:
		//! @param item the table widget item containing the full path
		void slotPlay(/*bool random, const QTableWidgetItem* item*/);
		void slotPause();
		void slotStop();
		void slotBackward();

		//! @param item the table widget item containing the full path
		void slotForward(bool random, const QTableWidgetItem* item, bool forcePlay=false);

		void slotAddFile();
		void slotRemoveSong();
		
		void slotTimerTimeout();
		/**
		 * Asks MPlayer to set the volume to a specified value
		 * @param newValue Volume percentage (range 0 to 100) for new value
		 */
		void slotChangeVolume(int newValue);

	private:
		STATUS_FLAGS player_status;
		QTimer songTimer;
		float playTime, curTime;

		QTableWidget* table;
		const QTableWidgetItem* curSong;
		PlayListQueue lastSongs, nextSongs;
		MPlayerConnection mPlayerConnection;
		int filterCount; //! number of visible items in table

		void playNextSong();

		//! (de)activates buttons depending on player_status variable
		void setStatus(STATUS_FLAGS new_status);

		//! finally, really starts the song! (does all the MPlayer communication, independent of our song playlist)
		//! note that you need to set curSong to the song you want to play next before calling this function
		void startSong();
		
	signals:
		//! is emitted when the status of the player has changed. see STATUS_FLAGS enum.
		void signalStatusChanged(STATUS_FLAGS new_status);

		//! is emitted by timer ticks to update percentage of playtime
		void signalUpdatePlaytime(int new_percentage);

	public:
		/**
		 * Constructor. Also initializes MPlayer (mplayer will be shut down in destructor)
		 */
		PlayerEngine(QTableWidget* table);

		inline const QTableWidgetItem* getCurSongItem(void) const {
			return curSong;
		}
		int getNextSongIndex(int totalSongNumber);
		inline void setFilterCount(int newValue) { filterCount = newValue; }
		inline int getCurrentTime() { return curTime; }
		inline STATUS_FLAGS getPlayerStatus() { return player_status; }
};

#endif // _PLAYER_ENGINE_H_
