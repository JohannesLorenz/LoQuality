/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2011                                               */
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

#ifndef FLASH_DLG_H
#define FLASH_DLG_H FLASH_DLG_H

#include <sys/wait.h>
#include <cstdio>

#include <QDialog>
#include <QListWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressDialog>
#include <QTimer>

//#include <QLabel>
//#include <QLineEdit>
//#include <QPushButton>
//#include <QFileDialog>

class SqlHelper;

class FlashDlg : public QDialog
{
	Q_OBJECT

	private slots:
		void selectionChanged();
		void buttonStorePressed();

		//! clears the listview and reloads all the files
		//! also used for initial loading
		void reload();
		void addToFileList(const QString& basedir); // used by reload()

		void slotTimerTimeout();

	private:
		pid_t ffmpegs_pid;
		QString curOutName; //!< empty if no current file is in progress
		QListWidget fileList;

		QCheckBox cbAddAfterwards;
		QPushButton storeButton, reloadButton, doneButton;

		QVBoxLayout topLayout;
		QHBoxLayout buttonLayout;

		QProgressDialog* progressDlg;
		QTimer convertTimer;

		// QLabel lbl_title; ....

		const SqlHelper& sqlhelper;
		bool downloadsMade;

		inline void closeEvent(QCloseEvent *event) {
			Q_UNUSED(event);
			if (downloadsMade && cbAddAfterwards.isChecked()) accept();
			else reject();
		}


	public:

		void setupUi();
		void retranslateUi();

		FlashDlg(const SqlHelper& _sqlhelper)
		 : topLayout(this), progressDlg(NULL), sqlhelper(_sqlhelper),
			downloadsMade(false)
		{
			setupUi();
		}

};

#endif // FLASH_DLG_H
