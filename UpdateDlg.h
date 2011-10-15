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
