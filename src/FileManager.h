/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2012                                               */
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

#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_ _FILE_MANAGER_H_

#include <QDialog>
class QFileInfo;

#include <QButtonGroup>
#include <QCheckBox>
#include <QLineEdit>
#include <QWizardPage>

#include "SqlHelper.h"
#include "FileAddBase.h"

class WelcomePage : public QWizardPage
{
	Q_OBJECT

	QVBoxLayout vbox;

	// TODO: Group Box
	QButtonGroup type_choice;
	QRadioButton btn_music, btn_video;

	// TODO: Group Box
	QButtonGroup loc_choice;
	QRadioButton btn_default, btn_custom;

	// TODO: Hbox
	QLineEdit custom_path;
	QPushButton btn_browse;

	QCheckBox showDirsWithTokenFiles;

	// TODO: needs retranslateUi and setupUi
private slots:
	void set_custom_path(int bool_value)
	{
	//	custom_path.setReadOnly(!bool_value);
		custom_path.setEnabled(bool_value);
		btn_browse.setEnabled(bool_value);
	}

	void browse();

public:
	WelcomePage() :
		vbox(this)
	{
		setTitle("Welcome");
		setSubTitle("Please choose of the options below.");

		btn_music.setText("Add Music");
		btn_video.setText("Add Video");
		type_choice.addButton(&btn_music, 0);
		type_choice.addButton(&btn_video, 1);
		btn_music.setChecked(true);

		btn_default.setText("Media Collection");
		btn_custom.setText("Custom Path");
		loc_choice.addButton(&btn_default, 0);
		loc_choice.addButton(&btn_custom, 1);
		btn_default.setChecked(true);

		set_custom_path(0);
		btn_browse.setText("Browse"); // TODO: needs a folder icon
		showDirsWithTokenFiles.setText("Show Directories where all media is already added.");

		vbox.addWidget(&btn_music);
		vbox.addWidget(&btn_video);
		vbox.addWidget(&btn_default);
		vbox.addWidget(&btn_custom);
		vbox.addWidget(&custom_path);
		vbox.addWidget(&btn_browse);
		vbox.addWidget(&showDirsWithTokenFiles);

		connect(&loc_choice, SIGNAL(buttonClicked(int)), this, SLOT(set_custom_path(int)));
		connect(&btn_browse, SIGNAL(clicked()), this, SLOT(browse()));

		registerField("type_is_music", &btn_music);
		registerField("loc_is_default", &btn_default);
		registerField("custom_path", &custom_path);
		registerField("show_token_dirs", &showDirsWithTokenFiles);
	}
};

class FileManager : public QWizardPage
{
	Q_OBJECT
	private:
		SqlHelper sqlhelper;
		FileAddBase fileAddBase;

		bool suffix_is_music(const QString &suffix);
		bool suffix_is_video(const QString &suffix);
		bool suffix_matches(const QString &suffix);

		void sortOutUseless(QTreeWidgetItem *parentItem, QDir *currentDir, QListIterator<QString> &dbItr);
		void appendAllDirectories(QTreeWidgetItem* parentItem, QDir* currentDir); // TODO: make currentDir const and copy each time ?
		void removeUnusedDirs(QTreeWidgetItem* parentItem, QDir* currentDir, QListIterator<QString> &dbItr);
		bool appendToItem(QTreeWidgetItem* parentItem, QDir* currentDir, QListIterator<QString>& dbItr);
		void grepFiles();
	private slots:
		 //! Callback if the user wants to proceed after the selection
		void proceed();
	public:
		inline bool isAnythingChanged() const { return fileAddBase.isAnythingChanged(); }
		FileManager (const SqlHelper& _sqlhelper);
		void initializePage() { grepFiles(); }
};

class FileManagerWizard : public QWizard
{
	Q_OBJECT;

	enum PAGE_ID {
		PAGE_INTRO,
		PAGE_FILES
	};
private:
	WelcomePage introPage;
	FileManager filePage;

	void retranslateUi();
	void setupUi();
public:
	FileManagerWizard(const SqlHelper& _sqlhelper);
	bool isAnythingChanged() const { return filePage.isAnythingChanged(); }
};

#endif // _FILE_MANAGER_H_
