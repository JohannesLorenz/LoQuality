#include "UpdateDlg.h"

UpdateDlg::UpdateDlg()
{
	// TODO: do not allow if options file does not say it's from git!
	system("git fetch origin");
	system("git --no-pager log --pretty=oneline FETCH_HEAD...HEAD > updates.txt");

}
