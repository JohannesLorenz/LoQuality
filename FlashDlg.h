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
		void selectionChanged ();
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

	public:

		void setupUi();
		void retranslateUi();

		FlashDlg(const SqlHelper& _sqlhelper) : topLayout(this), progressDlg(NULL), sqlhelper(_sqlhelper)
		{
			setupUi();
		}

};

#endif // FLASH_DLG_H
