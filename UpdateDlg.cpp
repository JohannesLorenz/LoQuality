#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <QMessageBox>

#include "UpdateDlg.h"
#include "globals.h"

bool question(const char* title, const char* text)
{
	QMessageBox dlg;

	dlg.setIcon(QMessageBox::Question);

	dlg.setWindowTitle(title);
	dlg.setText(QString::fromLocal8Bit(text));
	dlg.addButton("Yes", QMessageBox::YesRole); // returns 0
	dlg.addButton("No", QMessageBox::AcceptRole); //returns 1

	int ret_value=dlg.exec();

	return (ret_value==0)?true:false;
}

bool UpdateDlg::readOutputToItems(int output)
{
	QString cur_line;
	char data[2];
	data[1] = 0;

	while(read(output, data, 1)>0) {
		if(data[0] == '\n') {
			commitOverview.addItem(cur_line);
			cur_line.clear();
		}
		else {
			cur_line.append(data[0]);
		}
	}
	if(!cur_line.isEmpty()) // trailing \n was missing...
	{
		commitOverview.addItem(cur_line);
		cur_line.clear();
	}
	return (commitOverview.count() != 0);
}

void UpdateDlg::fetchItems()
{
	// TODO: do not allow if options file does not say it's from git!
	system("git fetch origin");
	//	system("git --no-pager log --pretty=oneline FETCH_HEAD...HEAD > updates.txt");


	int pipefd[2];
	int output;
	if (pipe(pipefd) == -1) {
		QMessageBox::information(NULL, "Sorry...", "... pipe() ging nicht, kein git!");
		return;
	}
	fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

	// fork MPlayer
	pid_t gits_pid=fork();
	if(gits_pid < 0) {
		QMessageBox::information(NULL, "Sorry...", "... fork() ging nicht, kein git!");
		return;
	}
	else if(gits_pid == 0) {

		::close(pipefd[0]); /* Close unused read end */
		dup2(pipefd[1], STDOUT_FILENO);

		execlp("git", "git", "--no-pager", "log", "--pretty=oneline", "FETCH_HEAD...HEAD", NULL);

		::close(pipefd[1]); /* Reader will see EOF */
		exit(0);
	}

	::close(pipefd[1]); /* Close unused write end */
	output = pipefd[0];

	waitpid(gits_pid, NULL, WCONTINUED);

	const bool updatesFound = readOutputToItems(output);

	::close(output);

	if(!updatesFound) {
		QMessageBox::information(NULL, "No updates found", "LoQuality is up to date :) "
			"Thank you for keeping an eye on it, still!");
		close();
	}
}

void UpdateDlg::buttonYesPressed() {
	QMessageBox::warning(this, "Starting update",
		"I will start download the updates now. Please "
		"do not do anything meanwhile - wait until I'll come up with "
		"further instructions...");

	system("git pull");

	QMessageBox::information(this, "Further instructions",
	 "Please close LQ now, then type \"./configure\", then \"make\". Good luck :)");
}

void UpdateDlg::retranslateUi()
{
	yesButton.setText("Go ahead!");
	noButton.setText("No updates, please!");
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

	retranslateUi();
}

UpdateDlg::UpdateDlg() : topLayout(this)//, progressDlg(NULL)
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
		fetchItems();
	else
		QMessageBox::information(this, "Sorry...",
			"... this on works for git!");

}
