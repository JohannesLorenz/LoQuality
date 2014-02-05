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

#include <QDir>
#include <QDateTime>
#include <QListWidgetItem>
#include <QLabel>

#include <QFileDialog>
#include <QSqlQuery>

#include "globals.h"
#include "FileManager.h"
#include "SqlHelper.h"

void WelcomePage::browse()
{
	QString directory = QFileDialog::getExistingDirectory(this,
		tr("Select Music Root"), QDir::currentPath());

	if (!directory.isEmpty()) {
		custom_path.setText(directory);
	}
}

/*
 * item is useless <=> all subdirs useless and no useful files
 * item is new <=> one subdir is new or one file is new
 *
 * subdir is useful <=> it exists (would have been removed otherwise)
 * subdir is new <=> it exists and is marked as new
 * file is useful <=> correct file ending
 * file is new <=> useful + time
 */
bool FileManager::suffix_is_music(const QString& suffix)
{
	// TODO: in these 2 functions, use QStringList::contains, it is faster and case sensitive
	// get the QStringlist from a global function, together with: FileManagerAddDlg, line 34
	return (suffix=="mp3" || suffix=="ogg" || suffix=="flac" || suffix=="wav" || suffix=="m4a" || suffix == "wma");
}

bool FileManager::suffix_is_video(const QString &suffix)
{
	exit(99);
	return (suffix=="mp4" || suffix=="wma" || suffix=="flv" || suffix=="avi" || suffix=="mpg" || suffix == "mpeg");
}

bool FileManager::suffix_matches(const QString& suffix)
{
	const bool type_is_music = field("type_is_music").toBool();
	printf("typeis: %d\n",type_is_music);
	return type_is_music ? suffix_is_music(suffix) : suffix_is_video(suffix);
}

void FileManager::sortOutUseless(QTreeWidgetItem *parentItem, QDir *currentDir, QListIterator<QString>& dbItr)
{
	int useful_subdirs = 0, new_subdirs = 0, useful_files = 0, new_files = 0;
	const bool removeTokenDirs = !(field("show_token_dirs").toBool());

	// scan subdirs
	int nchildren = parentItem->childCount();
	for(int i = 0; i < nchildren; i++)
	{
		// directory exists => useful
		QTreeWidgetItem* curChild = parentItem->child(i);

		useful_subdirs++;
		if(!curChild->isDisabled())
		 new_subdirs++;
	}

	// scan contained files
	QList<QFileInfo> files = currentDir->entryInfoList((QDir::Filters)QDir::NoFilter ^ (QDir::Filters)QDir::NoSymLinks, QDir::Name);
	for(QList<QFileInfo>::const_iterator itr = files.begin(); itr != files.end(); itr++)
	if(!itr->isDir())
	{
		printf("File: %s\n",itr->fileName().toAscii().data());

		const QString suffix = itr->suffix();
		if(suffix_matches(suffix))
		{
			useful_files++;
			const QString canPath = itr->canonicalFilePath();

			// skip to next itr item >= this file, or the end
			for(; dbItr.hasNext() && dbItr.peekNext() < canPath; dbItr.next()) {
				printf("peek: %s\n", dbItr.peekNext().toAscii().data());
			}

			if(dbItr.hasNext())
			 printf("a: %s, b:%s\n", dbItr.peekNext().toAscii().data(), canPath.toAscii().data());

			if( ! dbItr.hasNext() || dbItr.peekNext() > canPath ) {
				new_files++;
			}
		}
	}


	printf("Dir %s=> usef files: %d, new files: %d, usef dirs: %d, new dirs: %d\n",
		parentItem->text(0).toAscii().data(),
		useful_files, new_files, useful_subdirs, new_subdirs);

	// mark this directory correctly
	if(new_files || new_subdirs) {
		// contains new files
		parentItem->setForeground(0, QColor("blue"));
	}
	else if(removeTokenDirs || !useful_files) {
		// should be hidden
		parentItem->setDisabled(true);
		parentItem->setHidden(true);
	}
	else {
		// not removed, but also no new files
		parentItem->setDisabled(true);
	}

	const int& displayed_dirs = removeTokenDirs ? new_subdirs : useful_subdirs;
	const int& imaginary_files = removeTokenDirs ? new_files : useful_files;
	if(!imaginary_files && displayed_dirs == 1)
	{
		QTreeWidgetItem* only_displayed = NULL;

		for(int i = 0; i < nchildren && (only_displayed == NULL); i++){
		printf("problem: %s\n",parentItem->child(i)->text(0).toAscii().data());
		 if(!parentItem->child(i)->isHidden())
		  only_displayed = parentItem->child(i);
}
		if(!only_displayed)
		 exit(98); // TODO... use catch/throw or at least assert

	//		parentItem->setText(0, parentItem->text(0) + QDir::separator() + only_displayed->text(0));
	//	only_displayed->setHidden(true);
		only_displayed->setText(0, parentItem->text(0) + QDir::separator() + only_displayed->text(0));

	//	parentItem->removeChild(only_displayed);
	//	parentItem->addChild(only_displayed);
		QTreeWidgetItem* the_parent = parentItem->parent();
		the_parent->removeChild(parentItem);
		the_parent->addChild(only_displayed);
	}
}

void FileManager::appendAllDirectories(QTreeWidgetItem *parentItem, QDir *currentDir)
{
	QTreeWidgetItem* curItem = new QTreeWidgetItem( parentItem, QStringList() << currentDir->dirName() );
	parentItem->addChild( curItem );
	QList<QFileInfo> files = currentDir->entryInfoList((QDir::Filters)QDir::NoFilter ^ (QDir::Filters)QDir::NoSymLinks, QDir::Name);
	for(QList<QFileInfo>::const_iterator itr = files.begin(); itr != files.end(); itr++)
	{
		if(itr->isDir())
		{
			QString fileName = itr->fileName();

			currentDir->cd(fileName);
			appendAllDirectories(curItem, currentDir); // recursion
			currentDir->cdUp();
		}
	}

}

void FileManager::removeUnusedDirs(QTreeWidgetItem *parentItem, QDir *currentDir, QListIterator<QString>& dbItr)
{
	int nchilds = parentItem->childCount();

	// recursion loop
	for(int i = 0; i < nchilds; i++)
	{
		QTreeWidgetItem* curChild = parentItem->child(i);
		printf("before recursion: %s\n", currentDir->canonicalPath().toAscii().data());
		printf("aborting: %s\n", parentItem->text(0).toAscii().data());
		currentDir->cd(curChild->text(0));
		printf("recursion: %s\n", currentDir->canonicalPath().toAscii().data());
		removeUnusedDirs(curChild, currentDir, dbItr); // recursion
		currentDir->cdUp();
	}
	printf("recursion: end: %s\n", currentDir->canonicalPath().toAscii().data());
	Useless(parentItem, currentDir, dbItr);
}



bool FileManager::appendToItem(QTreeWidgetItem* parentItem, QDir* currentDir, QListIterator<QString> &dbItr)
{
	appendAllDirectories(parentItem, currentDir);
	removeUnusedDirs(parentItem, currentDir, dbItr);
	return true; // TODO: useless
}

/*
	BUTTONS SLOTS
*/
/*void MainWindow::slotAddFile() {
	AddEntryDlg dlg;
	dlg.show();
	dlg.exec();
}*/

/*
	MENU BAR SLOTS
*/


FileManager::FileManager (const SqlHelper& _sqlhelper) :
	sqlhelper(_sqlhelper),
	fileAddBase(_sqlhelper, this)
{
}

void FileManager::grepFiles()
{
	QSqlQuery query = sqlhelper.exec("SELECT * FROM main;");
	QList<QString> dbNameList;

	while (query.next()) {
//		puts("...");
		dbNameList.push_back( query.value(13).toString() );
	}
	qSort( dbNameList );
	QListIterator<QString> dbItr(dbNameList);

	const bool loc_is_default = field("loc_is_default").toBool(),
		type_is_music = field("type_is_music").toBool();
	QDir parseDir(
		loc_is_default ?
			(type_is_music ? globals::MUSIC_ROOT : globals::VIDEO_ROOT)
			: field("custom_path").toString()); // see above in this file!

	new QTreeWidgetItem( &fileAddBase.fileView, QStringList() << (type_is_music ? "Music" : "Video") );
	printf("typeis: %d\n",type_is_music);
	appendToItem(fileAddBase.fileView.topLevelItem(0), &parseDir, dbItr);

	fileAddBase.setProceedCallback(this, SLOT(proceed()));
}

#include "FileManagerAddDlg.h"

void FileManager::proceed()
{
	FileManagerAddDlg addDlg(&fileAddBase.fileView, sqlhelper);
	addDlg.show();
	if( !fileAddBase.isAnythingChanged() && addDlg.exec() == QDialog::Accepted ) {
		puts("CHANGED!\n");
		fileAddBase.setAnythingChanged();
	}
}




FileManagerWizard::FileManagerWizard(const SqlHelper& _sqlhelper)
 : filePage(_sqlhelper)
{
	setupUi();
	retranslateUi();
}

void FileManagerWizard::retranslateUi()
{
	setWindowTitle("File Manager");
}

void FileManagerWizard::setupUi()
{
	setPage(PAGE_INTRO, &introPage);
	setPage(PAGE_FILES, &filePage);
}

