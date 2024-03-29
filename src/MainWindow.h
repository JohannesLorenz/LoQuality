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

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_ _MAIN_WINDOW_H_

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>
#include <QToolBar>
#include <QSplitter>

#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QDial>
#include <QToolBox>
#include <QSpinBox>
#include <QTabWidget>
#include <QSqlTableModel>

#include <QAction>
#include <QStatusBar>
#include <QMessageBox>

#include "globals.h"
#include "MainWindowContainer.h"
#include "SongTableWidget.h"
#include "AddEntries.h"
#include "PlayerEngine.h"
#include "SqlHelper.h"
#include "dbus/DBusConnector.h"
#include "UpdateDlg.h"
#include "src/UpdateInfoDlg.h"
#include "src/DownloadImageDlg.h"
#include "dialogs.h"
#include "PlaylistDownloader.h"

/**
 * @class MainWindow
 * @brief The applications main window.
 *
 * MainWindow is the main window class for the TODO GUI.
 * All subwidgets are referenced here.
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	typedef PlayerEngine::STATUS_FLAGS STATUS_FLAGS; // this is needed for Qt's slot syntax :-/

	// This enum is only important for the number in the menubar.
	enum MENU {
		MENU_FILE, MENU_EDIT, MENU_VIEW, MENU_HELP, MENU_SIZE
	};
	
	enum ACTION {
		ACTION_FILE_OPEN, ACTION_FILE_UPDATE, ACTION_FILE_CLOSE, ACTION_FILE_QUIT,
		ACTION_EDIT_EDITTITLE, ACTION_EDIT_DOWNLOAD, ACTION_EDIT_OPTIONS,
		ACTION_VIEW_SCROLL_TO_SONG, ACTION_VIEW_FULLSCREEN, ACTION_VIEW_SWITCH_ALIGN,
		ACTION_HELP_ABOUT, ACTION_HELP_ABOUTQT,
		ACTION_SIZE
	};
	
	enum BTN1 {
		BTN1_PLAY, BTN1_PAUSE, BTN1_STOP, BTN1_RANDOM, BTN1_BWD, BTN1_FWD, BTN1_REPEAT, BTN1_SIZE
	};
	
	enum BTN2 {
		BTN2_SPECIAL_FILTER, BTN2_SYNCH_USB, BTN2_SYNCH_MP3, BTN2_ADD, BTN2_REMOVE, BTN2_FLASH, BTN2_SIZE
	};
public:
	enum LISTVIEW_COLUMNS {
		COL_NUMBER=0, COL_TITEL, COL_ARTIST, COL_ALBUM, COL_TAG, COL_GENRE, COL_YEAR, COL_P, COL_J, COL_TYPE,
		COL_QUALITY, COL_RATE_J, COL_RATE_P, COL_RATE, COL_SIZE
	};
private:
	enum TOOLBOX_ITEMS {
		TOOLBOX_FILTER = 0,
		TOOLBOX_METAINFO
	};

private slots:
	/**
	 * If no song is currently played, this slot is called to ask the player engine to play a new song.
	 * If a song is currently played and paused, asks the player engine to continue.
	 * If a song is currently played and not paused, the next song will be played
	 * Might apply a new value to the volume sliders.
	 * @param row the row of the song to be played (if none playing), -1 if no wish for which song shall follow
	 * @param column will be ignored in this function
	 */
	void slotPlay(int row, int column = 0);

	//! argument-less wrapper to allow slot binding
	inline void slotPlay() {
		slotPlay(-1, 0);
	}

	void slotPause(); // eng
	void slotStop(); // eng
	inline void slotStopAfter() {
		time_to_stop = stopButtonMenu.actions().at(1)->isChecked() ? time(NULL) : 0;
	}

	void slotBackward();
	inline void slotForward() { slotForward(-1, -1); }
	void slotForward(int x, int y);

	void slotAddFile();
	void slotAddFileChooser();
	inline void slotRemoveSong() { tableWidget.slotRemoveSong(); }

	void slotStoreFlash();
	void slotSynch();

	inline void slotOpenNewMainWindow() { mwContainer->openNewWindow(); }

	void slotFileCloseAction();
	void slotFileQuitAction();

	inline void slotFileUpdateAction() {
		UpdateDlg udlg;
		udlg.show();
		udlg.exec();
	}

	inline void slotViewFullscreenAction() {
		if( isFullScreen() ) showNormal(); else showFullScreen();
	}

	inline void slotEditDownload() {
		PlaylistDownloader downloader(&tableWidget, sqlhelper);
		downloader.show(); downloader.exec();
	}

	inline void slotViewSwitchAlignment()
	{
		mainSplitter.setOrientation(
				(mainSplitter.orientation() == Qt::Horizontal)
				? Qt::Vertical : Qt::Horizontal
				);
	}

	inline void slotScrollToSong() { tableWidget.slotScrollToSong(); }

	inline void slotInfoDownload()
	{
		//QString currentDir = player.getCurSongItem()->text();
		//currentDir.resize();
		if(player.getCurSongItem() != NULL)
		{
			DownloadImageDlg idlg(player.getCurSongItem()->text());
			idlg.show();
			idlg.exec();
		//	if( idlg.exec() == QDialog::Accepted && toolBox->currentIndex() == TOOLBOX_METAINFO)
		//	{
		//		slotToolBoxChanged(TOOLBOX_METAINFO); // reload image
		//	}
		}
	}

	void slotHelpAboutAction();
	void slotHelpAboutQtAction();

	void slotFilterChanged(const QString & text);

	/**
	 * Asks MPlayer to set the volume to a specified value
	 * @param newValue Volume percentage (range 0 to 100) for new value
	 */
	void slotChangeVolume(int newValue);

	inline void slotItemEdit() {
		slotItemEdit(0, 0); // TODO ;)
	}

	inline void slotItemEdit(int row, int column)
	{
		tableWidget.slotItemEdit(row, column);
	}

	//! (de)activates buttons depending on player_status variable
	void onSetStatus(STATUS_FLAGS new_status);

	void slotToolBoxChanged(int newIndex);

	inline void showPopupMenu(const QPoint& p) {
		popupMenu.exec(p);
	}

	void slotSplitterMoved(int pos, int idx) {
		Q_UNUSED(idx);
		imageLabel.setFixedHeight(pos);
		//imageLabel.pixmap()->resize(pos,);
		imageLabel.setPixmap(QPixmap::fromImage(tmpImage.scaledToHeight(pos-150)));
	}

	void slotOpenOptionsDlg() {
		QMessageBox::information(NULL, "Not yet implemented! Edit file per text editor:", globals::settings->fileName().toLatin1().data());
	}

private:
	// MainWindowContainer
	MainWindowContainer* mwContainer;

	//database
	SqlHelper sqlhelper;

	//player engine
	PlayerEngine player;

	//QMap<QString, QMap<QString, QTree>> artistAlbumList;

	//QList<QString> played_songs;

	// menu bar
	QMenuBar menubar;
	QMenu topMenus[MENU_SIZE];

	// tool bar
	QToolBar toolBar;

	// central widget
	QWidget centralWidget;
	QVBoxLayout verticalLayout;

	// mobile stuff
	QTabWidget mobileTab;
	QWidget mobileTab1;
	QVBoxLayout mobileButtonsVBox;
	QWidget mobileTab3;
	QVBoxLayout mobileTableVBox;
	QWidget mobileTab4;
	QHBoxLayout mobileSpecialHBox;

	QHBoxLayout hbox_buttons1;
	QLineEdit filter;
	QPushButton buttons1[BTN1_SIZE];
	QMenu stopButtonMenu;

	QHBoxLayout hbox_buttons2;
	QPushButton buttons2[BTN2_SIZE];
	QMenu insertButtonMenu;
	QProgressBar progressBar;
	QDial volumeSlider;

	// splitter between toolBox and tableWidget
	QSplitter mainSplitter;
	QToolBox* toolBox;

	// information toolbox item
	QHBoxLayout informationBox;
	QLabel imageLabel;
	QImage tmpImage;
	QPushButton optionsButton;
	QMenu infoOptionsMenu;
	QAction infoActionDownload;

	// special filters toolbox item
	QWidget specialFiltersContainer;
	QWidget TODO;
	QHBoxLayout specialFilters;
	QLabel labelYearFilter, labelRatingFilter;
	QSpinBox minYearFilter, maxYearFilter;
	QSpinBox minRatingFilter, maxRatingFilter;
	QCheckBox playUnratedFilter;
//	QCheckBox x1Filter, x2Filter;

	// table Widget
	SongTableWidget tableWidget;

	// other stuff
	QVector<QAction*> Actions;
	QMenu popupMenu;
	bool quitProgram; // only set to true if the program is exited with the menu bar
	time_t time_to_stop;

	// status bar
	QStatusBar statusBar;

	/**
	 * @brief Helper function to set up an action.
	 * This function allocates a QAction, sets it up and assigns it
	 *
	 * @param menuNo Determines the number in the menubar where the action is appended.
	 *      Use MENU_SIZE if you do not want to append it.
	 * @param slotName The slot, as returned by Qt's SLOT() macro, or NULL if none.
	 * @param shortKeySequence The Qt object for a shortkey or empty QKeySequence for none
	 * @param xpmStr The 2-dimensional xpm char array, or NULL if no pixmap shall be used.
	 * @return Pointer to the new allocated object.
	 */
	QAction* initAction (enum MENU menuNo, enum ACTION actionNo, const char* slotName,
		const QKeySequence& shortKeySequence = QKeySequence(), const char* const xpmStr[] = NULL);
	/**
	 * Helper function to initialize a button in the first button row (media buttons)
	 * @param btn1No ID of button
	 * @param slotName Slot of the MainWindow to be called back
	 * @param xpmStr C string to the button's XPM icon
	 */
	void initButton1 (enum BTN1 btn1No, const char* slotName, const char* const xpmStr[]);

	/**
	 * Helper function to initialize a button in the second button row (functional buttons)
	 * @param btn2No ID of button
	 * @param slotName Slot of the MainWindow to be called back
	 */
	void initButton2 (enum BTN2 btn2No, const char* slotName);

	//! (re)translates the GUI into another language
	void retranslateUi();

	//! layout stuff that needs to be redone for other layouts
	void layoutWidgets(bool mobile = false);
	void freeLayout();

signals:


public:
	/**
	 * Constructor. Builds up the whole Main Window and sub widgets, connects all signals
	 * @param parent parent pointer as it will be passed to QMainWindow. Not used otherwise.
	 */
	MainWindow (MainWindowContainer* _mwContainer, const QString& dbname, const bool mobile, QWidget* parent=NULL);
	~MainWindow();
	inline PlayerEngine* get_player() { return &player; }
	//inline void setVisible(bool visible) { setVisible(true); }

protected:
	bool event(QEvent *event);
};

#endif // _MAIN_WINDOW_H_
