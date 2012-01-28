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
#include "FileManagerAddDlg.h"
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

void FileManager::retranslateUi()
{
	fileView.setHeaderLabel("MusikSammlung");
//	btnAdd.setText("Hinzufügen"); // TODO: add cool + and - icons...
//	btnErase.setText("Herausnehmen");
	btnDoIt.setText("Rein in die Datenbank!");
	btnDone.setText("Fertig!");
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


FileManager::FileManager (const SqlHelper& _sqlhelper)
	:

	anything_changed(false),
	sqlhelper(_sqlhelper),
	changing_selection(false),

	// file view
	topLayout(this),
	fileView(this)

	// others
	
{

	QSqlQuery query("SELECT * FROM main;");
	QList<QString> dbNameList;

	while (query.next()) {
//		puts("...");
		dbNameList.push_back( query.value(13).toString() );
	}
	qSort( dbNameList );
	QListIterator<QString> dbItr(dbNameList);

	/*
		MENU BAR
		*/
//	resize(792, 768);
	
	/*
		CENTRAL WIDGET
		*/
	fileView.setSelectionMode(QAbstractItemView::MultiSelection);
	topLayout.addWidget(&fileView);
	
//	fileView.resize(400, 320);
	
	QDir parseDir(globals::MUSIC_ROOT); // see above in this file!
	QTreeWidgetItem* curItem = new QTreeWidgetItem( &fileView, QStringList() << "musik" );
	appendToItem(curItem, &parseDir, dbItr);


//	btnAdd.setChecked(true);
//	buttonLayout.addWidget(&btnAdd);
//	buttonLayout.addWidget(&btnErase);
	buttonLayout.addWidget(&btnDoIt);
	buttonLayout.addWidget(&btnDone);
	topLayout.addLayout(&buttonLayout);

	/*
		CONNECT SIGNALS TO SLOTS
		*/
	connect(&btnDoIt, SIGNAL(clicked()), this, SLOT(slotBtnDoIt()));
	connect(&btnDone, SIGNAL(clicked()), this, SLOT(close()));
	connect(&fileView, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

	/*
		OTHER
		*/

	retranslateUi();

}

void FileManager::slotBtnDoIt ()
{
	FileManagerAddDlg addDlg(&fileView, sqlhelper);
	addDlg.show();
	if( !anything_changed && addDlg.exec() == QDialog::Accepted ) {
		puts("CHANGED!\n");
		anything_changed = true;
	}
}

void FileManager::selectAllSubItems( QTreeWidgetItem* curItem, bool select )
{
	//selectionModel()
	curItem->setSelected(select);
	for(int i=0; i<curItem->childCount(); i++)
	{
		if( ! curItem->child(i)->isSelected() )
		 selectAllSubItems(curItem->child(i), select);
	}
}

void FileManager::selectionChanged (  )
{
	if(! changing_selection)
	{
		changing_selection = true;
	/*	// TODO: better algorithm needed!
		QListIterator<QTreeWidgetItem*> i(lastSelection);
		while (i.hasNext()) {
			QTreeWidgetItem* item = i.next();
			if( ! fileView.selectedItems().contains(item) )
			 selectAllSubItems( item, false );
		}*/

		QListIterator<QTreeWidgetItem*> i2(fileView.selectedItems());
		while (i2.hasNext()) {
			selectAllSubItems( i2.next(), true );
		}

		lastSelection = fileView.selectedItems();
		changing_selection = false;
	}

/*	bool enableAdd = false, enableErase = false;
	
	QList<QTreeWidgetItem*> items = fileView.selectedItems();
	for(QList<QTreeWidgetItem*>::const_iterator itr = items.begin();
		itr != items.end(); itr++)
	{
		if( !enableAdd && ! (*itr)->isDisabled() )
		 enableAdd = true;
		if( !enableErase && (*itr)->isDisabled() )
		 enableErase = true;
	}


	btnAdd.setEnabled(enableAdd);
	btnErase.setEnabled(enableAdd); */
}

