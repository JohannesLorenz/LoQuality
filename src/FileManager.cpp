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

/*#include <QDebug>

#include <QString>
#include <QSizePolicy>
#include <QFileDialog>
#include <QTextBrowser>
*/

#include <QSqlQuery>

#include "globals.h"
#include "FileManager.h"
#include "SqlHelper.h"

#if 0
bool FileManager::appendSingle(QTreeWidgetItem* curItem, QDir* currentDir, QList<QFileInfo>::const_iterator& itr, QListIterator<QString>& dbItr)
{
	bool somethingNewHere = false;
	if(itr->isDir()) {
		printf("Dir: %s\n",itr->fileName().toAscii().data());

		QString fileName = itr->fileName();
		if( fileName != "." && fileName != ".." )
		{
			printf("cd %s\n",itr->fileName().toAscii().data());
			currentDir->cd(fileName);
			if( appendToItem(curItem, currentDir, dbItr) ) // <-- RECURSION HERE !
			 somethingNewHere = true;
			currentDir->cdUp();
		}
	}
	else {
		/*
		 * Note: this is not effective
		 * Reason: Useless dirs are not, but dirs with "useless" subdirs are still added.
		 * For the future, remove this, and search the tree upwards
		 *  in order to remove useless items
		 */

		printf("File: %s\n",itr->fileName().toAscii().data());

		const QString suffix = itr->suffix();
		if(suffix=="mp3" || suffix=="ogg" || suffix=="flac" || suffix=="wav" || suffix=="m4a" || suffix == "wma")
		{

			const QString absPath = itr->absolutePath();

			// skip to next itr item >= this file, or the end
			for(; dbItr.hasNext() && dbItr.peekNext() < absPath; dbItr.next()) ;

			if( ! dbItr.hasNext() || dbItr.peekNext() > absPath ) {
				puts("->NEW!");
				somethingNewHere = true;
			}
		}
	}
	return somethingNewHere;
}
#endif

/*
 * item is useless <=> all subdirs useless and no useful files
 * item is new <=> one subdir is new or one file is new
 *
 * subdir is useful <=> it exists (would have been removed otherwise)
 * subdir is new <=> it exists and is marked as new
 * file is useful <=> correct file ending
 * file is new <=> useful + time
 */
void FileManager::sortOutUseless(QTreeWidgetItem *parentItem, QDir *currentDir, QListIterator<QString>& dbItr, bool removeTokenDirs)
{
	int useful_subdirs = 0, new_subdirs = 0, useful_files = 0, new_files = 0;

/*	for(int i = 0; i < nchilds; i++)
	{
		QTreeWidgetItem* curChild = parentItem->child(i);*/
		QTreeWidgetItem* curChild = parentItem;

		int nchildren = parentItem->childCount();
		printf("children: %d\n", nchildren);
		for(int i = 0; i < nchildren; i++)
		{
			// directory exists => useful
			useful_subdirs++;
		//	if(curChild->data(0, Qt::UserRole).toBool() == true) // marked as new
		//	 new_subdirs++;
			if(!curChild->isDisabled())
			 new_subdirs++;
		}
#if 0
		// directory exists => useful
		useful_subdirs++;
	//	if(curChild->data(0, Qt::UserRole).toBool() == true) // marked as new
	//	 new_subdirs++;
		if(!curChild->isDisabled())
		 new_subdirs++;
#endif
		// scan directory for files
//		currentDir.cd(curChild->text());

//		printf("current Dir: %s, current child: %s\n", currentDir->canonicalPath().toAscii().data(), curChild->text(0).toAscii().data());
//		currentDir->cd(curChild->text(0));
		printf("current Dir now: %s\n", currentDir->canonicalPath().toAscii().data());
		QList<QFileInfo> files = currentDir->entryInfoList((QDir::Filters)QDir::NoFilter ^ (QDir::Filters)QDir::NoSymLinks, QDir::Name);
		for(QList<QFileInfo>::const_iterator itr = files.begin(); itr != files.end(); itr++)
		if(!itr->isDir())
		{
			printf("File: %s\n",itr->fileName().toAscii().data());

			const QString suffix = itr->suffix();
			if(suffix=="mp3" || suffix=="ogg" || suffix=="flac" || suffix=="wav" || suffix=="m4a" || suffix == "wma")
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

//		currentDir->cdUp();

//	}

	printf("Dir %s=> usef files: %d, new files: %d, usef dirs: %d, new dirs: %d\n",
		parentItem->text(0).toAscii().data(),
		useful_files, new_files, useful_subdirs, new_subdirs);

	if(new_files || new_subdirs) {
		parentItem->setForeground(0, QColor("blue")); // TODO: colors only to decide between (in db) and (not in db)
	//	parentItem->setData(0, Qt::UserRole, QVariant(true)); // marked as new
	}
	else if(removeTokenDirs || !useful_files) {
	//	parentItem->treeWidget()->removeItemWidget(parentItem);
//		parentItem->parent()->removeChild(parentItem); // this looks crazy...
		printf("deleting: %s...\n", parentItem->text(0).toAscii().data());
		// destructor will remove child automatically
		// (src: Qt Docs, stackoverflow)
	//	delete parentItem;
		parentItem->setHidden(true);
		printf("deleting done.\n");
	}
	else {
		// not removed, but also no new files
		printf("Dir %s: disabled!\n",currentDir->dirName().toAscii().data());
		parentItem->setDisabled(true);
	//	parentItem->setData(1, Qt::UserRole, QVariant(true)); // marked as new
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
		//	if( fileName != "." && fileName != ".." )
			{
				printf("cd %s\n",itr->fileName().toAscii().data());
				currentDir->cd(fileName);
				appendAllDirectories(curItem, currentDir); // recursion
				//if( appendAllDirectories(curItem, currentDir, dbItr) ) // <-- RECURSION HERE !
				// somethingNewHere = true;
				currentDir->cdUp();
			}
		}

	}

}

void FileManager::removeUnusedDirs(QTreeWidgetItem *parentItem, QDir *currentDir, QListIterator<QString>& dbItr, bool removeTokenDirs)
{
	int nchilds = parentItem->childCount();
	bool somethingImportant = false;
	printf("parent: %s\n", parentItem->text(0).toAscii().data());
	// first traverse the children
	for(int i = 0; i < nchilds; i++)
	{
		QTreeWidgetItem* curChild = parentItem->child(i);
		printf("parent: %s, i: %d\n",parentItem->text(0).toAscii().data(),i);

		currentDir->cd(curChild->text(0));

		removeUnusedDirs(curChild, currentDir, dbItr, removeTokenDirs); // recursion
		printf("child: %s\n", curChild->text(0).toAscii().data());
		sortOutUseless(curChild, currentDir, dbItr, removeTokenDirs);

		currentDir->cdUp();

		/*if(itemIsUseless(curChild, removeTokenDirs))
		{
			parentItem->removeChild(curChild); // TODO: call delete?
			delete curChild;
		}*/
		//removeUnusedDirs(curChild); // recursion
	}

}



bool FileManager::appendToItem(QTreeWidgetItem* parentItem, QDir* currentDir, QListIterator<QString> &dbItr, bool removeTokenDirs = true)
{
	appendAllDirectories(parentItem, currentDir);
	removeUnusedDirs(parentItem, currentDir, dbItr, removeTokenDirs);

#if 0
	bool somethingNewHere = false;
	QTreeWidgetItem* curItem = new QTreeWidgetItem( parentItem, QStringList() << currentDir->dirName() );
	parentItem->addChild( curItem );
	QList<QFileInfo> files = currentDir->entryInfoList((QDir::Filters)QDir::NoFilter ^ (QDir::Filters)QDir::NoSymLinks, QDir::Name);

/*	int nSubDirs = 0;
	for(QList<QFileInfo>::const_iterator itr = files.begin(); itr != files.end() && (nSubDirs < 2); itr++)
	 if(itr->isDir())
	  nSubDirs++;*/

	/*switch(nSubDirs)
	{
		case 0: break; // nothing to add here
		case 1: // edit current item

			break;
		case 0:
		case 2: // make sub items
			for(QList<QFileInfo>::const_iterator itr = files.begin(); itr != files.end(); itr++)
			{
				bool newItems = appendSingle(curItem, currentDir, itr, dbItr);
				if(!somethingNewHere && newItems)
				 somethingNewHere = true;
			}
			break;
	}*/
	
	if(somethingNewHere) {
		curItem->setForeground(0, QColor("blue")); // TODO: colors only to decide between (in db) and (not in db)
		return true;
	}
	else {
		printf("Dir %s: disabled!\n",currentDir->dirName().toAscii().data());
		curItem->setDisabled(true);
		return false;
	}
#endif
	return true;
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

	QSqlQuery query = sqlhelper.exec("SELECT * FROM main;");
	QList<QString> dbNameList;

	while (query.next()) {
//		puts("...");
		dbNameList.push_back( query.value(13).toString() );
	}
	qSort( dbNameList );
	QListIterator<QString> dbItr(dbNameList);

	QDir parseDir(globals::MUSIC_ROOT); // see above in this file!

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


