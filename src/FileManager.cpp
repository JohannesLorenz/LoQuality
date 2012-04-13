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

bool FileManager::appendToItem(QTreeWidgetItem* parentItem, QDir* currentDir, QListIterator<QString> dbItr)
{
	bool somethingNewHere = false;
	QTreeWidgetItem* curItem = new QTreeWidgetItem( parentItem, QStringList() << currentDir->dirName() );
	parentItem->addChild( curItem );
	QList<QFileInfo> files = currentDir->entryInfoList(QDir::NoFilter, QDir::Name);
	for(QList<QFileInfo>::const_iterator itr = files.begin(); itr != files.end(); itr++)
	{
		if(itr->isDir()) {
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
			
			printf("File: %s\n",itr->fileName().toAscii().data());
			
			const QString suffix = itr->suffix();
			if(suffix=="mp3" || suffix=="ogg" || suffix=="flac" || suffix=="wav" || suffix=="m4a" || suffix == "wma")
			{

				const QString absPath = itr->absoluteFilePath();

				// skip to next itr item >= this file, or the end
				for(; dbItr.hasNext() && dbItr.peekNext() < absPath; dbItr.next()) ;

				/*if( dbItr.hasNext() && dbItr.peekNext() == absPath ) {
					puts("->NEW!");
					somethingNewHere = true;
				}*/
				if( ! dbItr.hasNext() || dbItr.peekNext() > absPath ) {
					puts("->NEW!");
					somethingNewHere = true;
				}


#if 0
				if( itr->lastModified() > QDateTime(QDate(2008,5,1)) && !somethingNewHere ) {
					puts("->NEW!");
				 somethingNewHere = true;
				}
#endif
			}
		}
	}
	
	if(somethingNewHere) {
		curItem->setForeground(0, QColor("blue")); // TODO: colors only to decide between (in db) and (not in db)
		return true;
	}
	else {
		printf("Dir %s: disabled!\n",currentDir->dirName().toAscii().data());
		curItem->setDisabled(true);
		return false;
	}
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

	QSqlQuery query("SELECT * FROM main;");
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


