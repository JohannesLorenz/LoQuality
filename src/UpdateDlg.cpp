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

#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <QDateTime>
#include <QMessageBox>

#include "UpdateDlg.h"
#include "globals.h"
#include "dialogs.h"

bool UpdateDlg::readOutputToItems(int output)
{
	QString cur_line;
	char data[2];
	data[1] = 0;
	bool space_read = false;
	while(read(output, data, 1)>0) {
		if(data[0] == '\n') {
			commitOverview.addItem(cur_line);
			cur_line.clear();
			space_read = false;
		}
		else {
			if(space_read)
			 cur_line.append(data[0]);
			if(data[0] == ' ')
			 space_read = true;
		}
	}
	if(!cur_line.isEmpty()) // trailing \n was missing...
	{
		commitOverview.addItem(cur_line);
		cur_line.clear();
	}
	return (commitOverview.count() != 0);
}

void UpdateDlg::fetchHeader() {

	gits_pid=fork();
	if(gits_pid < 0) {
		QMessageBox::information(NULL, "Sorry...", "... fork() ging nicht, kein git!");
		return;
	}
	else if(gits_pid == 0) {
		execlp("git", "git", "fetch", "origin", NULL);
		exit(0);
	}
	else
	{
		status = FINISHED_GIT_FETCH;
		progressDlg = new QProgressDialog("Downloading new revisions", "Abort", 0, 0, this);
		progressDlg->setWindowModality(Qt::WindowModal);
		progressDlg->setValue(0);
		progressDlg->show();
		timer.start();
	}

	// TODO: do not allow if options file does not say it's from git!
//	system("git fetch origin");
}

int UpdateDlg::getDiffLogs() {
	int pipefd[2];
	int output;
	if (pipe(pipefd) == -1) {
		QMessageBox::information(NULL, "Sorry...", "... pipe() ging nicht, kein git!");
		return -1;
	}
	fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

	pid_t gits_pid=fork();
	if(gits_pid < 0) {
		QMessageBox::information(NULL, "Sorry...", "... fork() ging nicht, kein git!");
		return -1;
	}
	else if(gits_pid == 0) {

		::close(pipefd[0]); /* Close unused read end */
		dup2(pipefd[1], STDOUT_FILENO);

		execlp("git", "git", "--no-pager", "log", "--pretty=oneline", "FETCH_HEAD...HEAD", NULL);

		::close(pipefd[1]); /* Reader will see EOF */
		exit(0);
	}
	else {
		::close(pipefd[1]); /* Close unused write end */
		output = pipefd[0];

		status = FINISHED_GIT_DIFFLOG;
		progressDlg = new QProgressDialog("Comparing revisions...", "Abort", 0, 0, this);
		progressDlg->setWindowModality(Qt::WindowModal);
		progressDlg->setValue(0);
		progressDlg->show();
		timer.start();
	}

	return output;
}


void UpdateDlg::fetchItems()
{
	fetchHeader();
}

void UpdateDlg::buttonYesPressed() {
	QMessageBox::warning(this, "Starting update",
		"I will start downloading the updates now. Please "
		"do not do anything meanwhile - wait until I'll come up with "
		"further instructions...");

	system("git pull");

	QMessageBox::information(this, "Further instructions",
		"Please close LQ as soon as possible."
		"I will compile LQ afterwards and then restart it. This may take a minute. Good luck :)");

	globals::settings->setValue("update_applied", true);

	pid_t sh_pid=fork();
	if(sh_pid < 0) {
		QMessageBox::information(NULL, "Sorry...", "... fork() ging nicht, kein update!");
		return;
	}
	else if(sh_pid == 0) {
		char ppid_str[16];
		snprintf(ppid_str, 16, "%d", getppid());
		execlp("xterm", "xterm", "-title", "LoQuality updater","-e",
			"./run.sh", globals::settings->value("number_of_cores",2).toString().toAscii().data(),
			ppid_str, NULL);
		exit(0);
	}
}

void UpdateDlg::retranslateUi()
{
	yesButton.setText("Go ahead!");
	noButton.setText("Close this window");
}

void UpdateDlg::setupUi() {

//	resize(600, 425);

	/*
		Setup UI...
	*/

//	fileList.setSelectionMode(QAbstractItemView::MultiSelection);
	topLayout.addWidget(&commitOverview);

	buttonLayout.addWidget(&yesButton);
	buttonLayout.addWidget(&noButton);
	topLayout.addLayout(&buttonLayout);

	connect(&yesButton,SIGNAL(clicked ()),
		this, SLOT(buttonYesPressed()));
	connect(&noButton,SIGNAL(clicked ()),
		this, SLOT(close()));

	timer.setInterval(1000);
	QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(slotTimerTimeout()));

	retranslateUi();
}

void UpdateDlg::slotTimerTimeout()
{
	if( 0 != waitpid(gits_pid, NULL, WNOHANG) ) { // wait for ffmpeg to finish
		timer.stop();
		progressDlg->cancel();
		progressDlg=NULL;
		switch(status)
		{
			case FINISHED_NOTHING:
				QMessageBox::warning(NULL, "program error", "this shall never happen");
				exit(1);
			case FINISHED_GIT_FETCH:
				output_fd = getDiffLogs();
				break;
			case FINISHED_GIT_DIFFLOG:
			{
				const bool updatesFound = readOutputToItems(output_fd);
				::close(output_fd);
				if(!updatesFound) {
					QMessageBox::information(NULL, "No updates found", "LoQuality is up to date :) "
						"Thank you for keeping an eye on it, still!");
					yesButton.setDisabled(true);
					close();
				}
				status = FINISHED_NOTHING;
			}

		}
	}
}

UpdateDlg::UpdateDlg() :
	status(FINISHED_NOTHING),
	topLayout(this),
	progressDlg(NULL)
{
	setupUi();

	bool usedGit;
	if(globals::settings->value("update_type", "").toString() == "git")
	 usedGit = true;
	else
	{
		usedGit = question("Are you using git?",
			"Did you start LQ right from the git repository?");
		globals::settings->setValue("update_type",
			(usedGit)?"git":"unknown");
	}

	if( usedGit )
		fetchHeader();
	else
		QMessageBox::information(this, "Sorry...",
			"... this only works for git!");

}

void UpdateDlg::autoCheckForUpdates() {
	QDateTime time_now = QDateTime::currentDateTime();
	QDateTime last_start =
	globals::settings->value("last_start", QDateTime(QDate(0,0,0))).toDateTime();
	bool do_updates = globals::settings->value("do_updates", true).toBool();
	int update_interval =
		globals::settings->value("update_interval_days", 1).toInt();

	if(do_updates && last_start.daysTo(time_now) >= update_interval) {
		UpdateDlg u;
		u.show();
		u.exec();
	}

}

