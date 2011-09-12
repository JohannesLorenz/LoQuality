/**
 * @file PlayerEngine.h
 * @author Johannes Lorenz, Philipp Lorenz
 * @brief declaration of class PlayerEngine
 */

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
		enum STATUS_FLAGS {
			STATUS_STOPPED,
			STATUS_PLAYING,
			STATUS_SONGOVER,
			STATUS_PAUSED
		};

	public slots:
		//! @param item the table widget item containing the full path
		void slotPlay(bool random, const QTableWidgetItem* item);
		void slotPause();
		void slotStop();
		void slotBackward();

		//! @param item the table widget item containing the full path
		void slotForward(bool random, const QTableWidgetItem* item);

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
};

#endif // _PLAYER_ENGINE_H_
