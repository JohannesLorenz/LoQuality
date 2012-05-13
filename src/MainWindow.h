/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2012                                               */
/* Johannes Lorenz, Philipp Lorenz                                       */
/* https://github.com/DrSegfault/LoQuality                               */
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
#include <QTableWidget>
#include <QTabWidget>
#include <QSqlTableModel>

#include <QAction>
#include <QtGui/QStatusBar>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include "globals.h"
#include "AddEntries.h"
#include "PlayerEngine.h"
#include "SqlHelper.h"
#include "dbus/DBusConnector.h"
#include "UpdateDlg.h"
#include "src/UpdateInfoDlg.h"
#include "src/DownloadImageDlg.h"
#include "dialogs.h"

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
		ACTION_FILE_QUIT, ACTION_FILE_UPDATE,
		ACTION_EDIT_EDITTITLE, ACTION_EDIT_OPTIONS,
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

		inline int row2id(int given_id) {
			QTableWidgetItem* firstItem = tableWidget.item(given_id, 0);
			return firstItem->data(Qt::DisplayRole).toInt()/*-1*/;
			// ids begin with 1, columns with 0 - but not important here!
		}

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
		void slotRemoveSong();

		void slotStoreFlash();
		void slotSynch();

		void slotFileQuitAction();

		inline void slotFileUpdateAction() {
			UpdateDlg udlg;
			udlg.show();
			udlg.exec();
		}

		inline void slotViewFullscreenAction() {
			if( isFullScreen() ) showNormal(); else showFullScreen();
		}

		inline void slotViewSwitchAlignment() {
			mainSplitter.setOrientation(
					(mainSplitter.orientation() == Qt::Horizontal)
					? Qt::Vertical : Qt::Horizontal
					);
		}

		inline void slotScrollToSong() {
			if(! tableWidget.selectedItems().empty())
			 tableWidget.scrollToItem(*tableWidget.selectedItems().begin());
		}

		inline void slotInfoDownload() {
			//QString currentDir = player.getCurSongItem()->text();
			//currentDir.resize();
			if(player.getCurSongItem() != NULL) {
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

		void slotItemEdit(int row, int column)
		{
#if 0
			Q_UNUSED(column);
			
		
			AddEntryDlg dlg(sqlhelper, true, row);
			dlg.show();
			dlg.exec();
			
			reloadTable();
#endif

			Q_UNUSED(row);
			Q_UNUSED(column);
			QList<QTableWidgetSelectionRange> ranges = tableWidget.selectedRanges();
			if(ranges.empty()) {
				QMessageBox::information(NULL, "Sorry...", "You can not edit 0 songs :) (at least, it would not make much sense)");
				return;
			}

//			QTableWidgetItem* firstItem = tableWidget.item(ranges.front().topRow(), ranges.front().leftColumn());

			QList<int> selectedRows;
			for(QList<QTableWidgetSelectionRange>::const_iterator itr = ranges.begin(); itr!=ranges.end(); itr++)
			{
				for(int i = itr->topRow(); i <= itr->bottomRow(); i++)
				 selectedRows.append(row2id(i));
			}

			AddEntryDlg dlg(sqlhelper, true, &selectedRows);
			dlg.show();
			if( dlg.exec() == QDialog::Accepted )
			 reloadTable();


			/*if(ranges.size() == 1 && ranges.front().rowCount() == 1) {
				AddEntryDlg dlg(sqlhelper, true, row2id(ranges.front().topRow()));
				dlg.show();
				if( dlg.exec() == QDialog::Accepted )
				 reloadTable();
			}
			else {
				QMessageBox::information(NULL, "Sorry...", "Es kann im Moment nur je eine Zeile bearbeitet werden! Bitte nur eine *Zelle* anklicken!");
			}*/
		}
		
		//! (de)activates buttons depending on player_status variable
		void onSetStatus(STATUS_FLAGS new_status);

		void slotToolBoxChanged(int newIndex);

		inline void showPopupMenu(const QPoint& p) {
			popupMenu.exec(p);
		}

		void switch_tray(QSystemTrayIcon::ActivationReason reason);

		void slotSplitterMoved(int pos, int idx) {
			Q_UNUSED(idx);
			imageLabel.setFixedHeight(pos);
			//imageLabel.pixmap()->resize(pos,);
			imageLabel.setPixmap(QPixmap::fromImage(tmpImage.scaledToHeight(pos-150)));
		}

		void slotOpenOptionsDlg() {
			QMessageBox::information(NULL, "Not yet implemented! Edit file per text editor:", globals::settings->fileName().toAscii().data());
		}

	private:

		class SettingsReader
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
				shouldBe("mplayer_name", "mplayer2", first_start);
				shouldBe("music_root", "insert_your_path", first_start);
				shouldBe("number_of_cores", 2, first_start);
				shouldBe("update_interval_days", 1, first_start);
				shouldBe("do_updates", true, first_start);
				shouldBe("target", "pc", first_start);
				shouldBe("equalizer","0:0:0:0:0:0:0:0:0:0", first_start);
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

	private:
		//database
		QSqlDatabase db;
		SqlHelper sqlhelper;
		
		SettingsReader settingsReader;

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
		QTableWidget tableWidget;
		
		// other stuff
		QVector<QAction*> Actions;
		QMenu popupMenu;
		QSystemTrayIcon tray_icon;
		bool visible; // for the tray icon
		bool quitProgram; // only set to true if the program is exited with the menu bar
		time_t time_to_stop;
		DBusConnector dbus_connector;

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
		
		//! reloads the table from the database
		void reloadTable();

	signals:
		

	public:
		/**
		 * Constructor. Builds up the whole Main Window and sub widgets, connects all signals
		 * @param parent parent pointer as it will be passed to QMainWindow. Not used otherwise.
		 */
		MainWindow (const bool mobile, QWidget* parent=NULL);
		~MainWindow();

	protected:
		bool event(QEvent *event);
};

#endif // _MAIN_WINDOW_H_
