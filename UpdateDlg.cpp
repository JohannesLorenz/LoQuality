#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <QMessageBox>

#include "UpdateDlg.h"

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

	static char line[1024];
	*line = 0;
	while(read(output, line, 1024)>0) {
		commitOverview.addItem(line);
	}

	::close(output);
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
	fetchItems();
}
