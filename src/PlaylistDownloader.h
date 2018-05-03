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

#ifndef PLAYLISTDOWNLOADER_H
#define PLAYLISTDOWNLOADER_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProgressDialog>
#include <QCheckBox>
class SqlHelper;

#include "flash_tools.h"
class SongTableWidget;

class PlaylistDownloader : public QDialog
{
	Q_OBJECT

//	const SqlHelper& sqlhelper;
	QVBoxLayout boxMain;

	QCheckBox cbEnumerate;
	QCheckBox cbOverwrite;

	QLabel lblFormat;
	QComboBox cbFormat;
	QHBoxLayout boxFormat;

	QLabel lblFolder;
	QLineEdit leFolder;
	QPushButton btnFolder;
	QHBoxLayout boxFolder;

	QPushButton okBtn, abortBtn;
	QHBoxLayout boxButtons;

	YouTubeDlSession youtubedl;
	//FileDownloadSession httpget;
	WgetSession wget;
	MySpaceSession myspace;
	SongTableWidget* table;
	int currentDownloadRow;
	QProgressDialog* progressDlg;
	const SqlHelper& sqlhelper; // TODO: not needed???
	const char* TMP_DIRECTORY;

	bool shallDownloadFile(int row);
	void nextDownload();
	int countDownloads();

	// this might be put elsewhere
	static QString makeFilename(const QString& title);

public:
	void setupUi();
	void retranslateUi();
	explicit PlaylistDownloader(SongTableWidget* _table, const SqlHelper& _sqlhelper, QWidget *parent = 0);

signals:
	
private slots:
	void slotBtnOk();
	void slotTimerTimeout();
	void btnFolderClicked();
};

#endif // PLAYLISTDOWNLOADER_H
