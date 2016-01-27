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

#ifndef UPDATEDLG_H
#define UPDATEDLG_H

#include <QObject>
#include <QTimer>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressDialog>

class UpdateDlg : public QDialog
{
	Q_OBJECT

	enum FINISHED_T {
		FINISHED_NOTHING,
		FINISHED_GIT_FETCH,
		FINISHED_GIT_DIFFLOG
	};
	FINISHED_T status;

	QListWidget commitOverview;
	QPushButton yesButton, noButton;

	QVBoxLayout topLayout;
	QHBoxLayout buttonLayout;

	QProgressDialog* progressDlg;
	QTimer timer;

	pid_t gits_pid;
	int output_fd;

	/**
		Reads (short) revision output from git.
		@param output fd the text comes from
		@return true iff there are new revisions.
	*/
	bool readOutputToItems(int output);

	void setupUi();
	void retranslateUi();
	void fetchItems();
	void fetchHeader();
	int getDiffLogs();

private slots:
	void buttonYesPressed();
	void slotTimerTimeout();

public:
	UpdateDlg();
	static void autoCheckForUpdates();
};

#endif // UPDATEDLG_H
