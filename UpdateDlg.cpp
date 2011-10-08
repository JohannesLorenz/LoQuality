#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <QMessageBox>

#include "UpdateDlg.h"

UpdateDlg::UpdateDlg()
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
