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

#ifndef FLASH_DLG_H
#define FLASH_DLG_H FLASH_DLG_H

#include <sys/wait.h>
#include <cstdio>

//#include <QDialog>
#include <QLabel>
#include <QButtonGroup>
#include <QRadioButton>
#include <QWizard>

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

class StoreHelper : public QObject {
	Q_OBJECT;
protected:
	const SqlHelper& sqlhelper;
	bool downloadsMade;
	bool insertSql;

	pid_t ffmpegs_pid;
	QString curOutName; //!< empty if no current file is in progress
	QProgressDialog* progressDlg;
	QTimer convertTimer;
public:
	void askForOutputFilename(const QString& infile);
	bool convertToOgg(const char* infile, bool _insertSql);

public slots:
	void slotTimerTimeout();
public:
	inline bool made_downloads() const { return downloadsMade; }
	StoreHelper(const SqlHelper& _sqlhelper)
		: sqlhelper(_sqlhelper), downloadsMade(false) {
		convertTimer.setInterval(1000);
		QObject::connect(&convertTimer, SIGNAL(timeout()), this, SLOT(slotTimerTimeout()));
	}
};

class SelectFlashPage : public QWizardPage {
	Q_OBJECT;
	void setupUi();
	void retranslateUi();

	StoreHelper& storeHelper;

	QListWidget fileList;

	QCheckBox cbAddAfterwards;
	QPushButton storeButton, reloadButton/*, doneButton*/;

	QVBoxLayout topLayout;
	QHBoxLayout buttonLayout;
private slots:
	void selectionChanged();
	void buttonStorePressed();

	//! clears the listview and reloads all the files
	//! also used for initial loading
	void reload();
	void addToFileList(const QString& basedir); // used by reload()

	public:
	inline SelectFlashPage(StoreHelper& _storeHelper) : storeHelper(_storeHelper) {
		setupUi();
	}
	inline int nextId() const { return -1; }
};


class SelectFilesPage : public QWizardPage {
	StoreHelper& storeHelper;
	QLabel testLabel;
public:
	SelectFilesPage(StoreHelper& _storeHelper) : storeHelper(_storeHelper),
	testLabel(this) {
		testLabel.setText("TODO...");
	}
	inline int nextId() const { return -1; }
};

class FlashDlg : public QWizard
{
	Q_OBJECT

	enum PAGE_ID {
		PAGE_INTRO,
		PAGE_DECIDE,
		PAGE_SELECT_FLASH,
		PAGE_SELECT_FILES,
		PAGE_COUNT
	};

	//QVBoxLayout vbox[PAGE_COUNT];

	class IntroPage : public QWizardPage {
		QLabel infoLabel;
	public:
		IntroPage() : infoLabel(this) {
			infoLabel.setWordWrap(true);
			infoLabel.setText("The auther does not encourage you to<br/>"
				"<b>use LoQuality for <u>illegal</u> downloads!!!</b>");
		}
		inline int nextId() const { return PAGE_DECIDE; }
	} introPage;

	class DecidePage : public QWizardPage {
		QVBoxLayout vbox;
		QLabel infoLabel;
		QButtonGroup choice;
		QRadioButton btn_flash, btn_files;
	public:
		DecidePage() {
			infoLabel.setWordWrap(true);
			infoLabel.setText("Please choose one of the options below.");
			btn_flash.setText("download flash from Firefox");
			btn_files.setText("download files from HDD");
			choice.addButton(&btn_flash, 0);
			choice.addButton(&btn_files, 1);
			vbox.addWidget(&btn_flash);
			vbox.addWidget(&btn_files);
			setLayout(&vbox);
		}
		inline int nextId() const {
			return (choice.checkedId() == 1)	? PAGE_SELECT_FILES
								: PAGE_SELECT_FLASH;
		}
	} decidePage;


	private:

		// QLabel lbl_title; ....

		inline void closeEvent(QCloseEvent *event) {
			Q_UNUSED(event);
			if (storeHelper.made_downloads() /*&& cbAddAfterwards.isChecked()*/) accept();
			else reject();
		}

	public:

		void setupUi();
		void retranslateUi();

		StoreHelper storeHelper;
		SelectFlashPage selectFlashPage;
		SelectFilesPage selectFilesPage;

		FlashDlg(const SqlHelper& _sqlhelper)
		 :	storeHelper(_sqlhelper),
			selectFlashPage(storeHelper),
			selectFilesPage(storeHelper)
		{
			setupUi();
		}

};

#endif // FLASH_DLG_H
