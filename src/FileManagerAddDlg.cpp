/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2018                                               */
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
#include <QSqlQuery>
#include <QProgressDialog>

#include "FileManagerAddDlg.h"

#include "SqlHelper.h"
#include "globals.h"

void FileManagerAddDlg::grepNewFiles(const QTreeWidgetItem* parentItem, QDir* currentDir, QListWidget* listView, const QList<QString>* filesInDb)
{
	// files in this directory
	if( parentItem->isSelected() ) {
		// TODO: make this a global function?
		QList<QFileInfo> files = currentDir->entryInfoList(QStringList() << "*.mp3" << "*.ogg" << "*.flac" << "*.wav" << "*.m4a" << "*.wma", QDir::Files);
		QListIterator<QFileInfo> i(files);
		while (i.hasNext()) {
			QFileInfo curFileInfo = i.next();
			if(! filesInDb->contains( curFileInfo.canonicalFilePath() ) )
			 listView->addItem( curFileInfo.canonicalFilePath() );
		}
	}

	// recurse
	for(int i=0; i<parentItem->childCount(); i++)
	{
		currentDir->cd(parentItem->child(i)->text(0));
		grepNewFiles(parentItem->child(i), currentDir, listView, filesInDb);
		currentDir->cdUp();
	}
}

void FileManagerAddDlg::retranslateUi(void)
{
	dlgLabel.setText("Bitte Dateien de-selektieren, die nicht hinzugefügt werden sollen, dann bestätigen.");
	okBtn.setText("Ok");
	abortBtn.setText("Abbrechen");
}

void FileManagerAddDlg::setupUi(void)
{
	resize(792, 420);

	buttonLayout.addWidget(&abortBtn);
	buttonLayout.addWidget(&okBtn);
	dlgLayout.addWidget(&dlgLabel);
	dlgLayout.addWidget(&listWidget);
	dlgLayout.addLayout(&buttonLayout);

	listWidget.setSelectionMode(QAbstractItemView::MultiSelection);

	QObject::connect(&okBtn, SIGNAL(clicked()), this, SLOT(slotBtnOk()));
	QObject::connect(&abortBtn, SIGNAL(clicked()), this, SLOT(reject()));

	const QList<QTreeWidgetItem*> items = fileViewRef->selectedItems();

	QDir parseDir(globals::MUSIC_ROOT);

	/*QListIterator<QTreeWidgetItem*> i(items);
	while (i.hasNext()) {
		QTreeWidgetItem* QString& curItem =
		grepNewFiles(i.next(), &parseDir);
	}*/

	QSqlQuery query = sqlhelper.exec("SELECT * FROM main;");
	QList<QString> dbNameList;
	while (query.next()) {
		dbNameList.push_back( query.value(13).toString() );
	}
	std::sort(dbNameList.begin(), dbNameList.end());
	
	grepNewFiles(fileViewRef->topLevelItem(0)->child(0), &parseDir, &listWidget, &dbNameList);
	listWidget.selectAll();
}

void FileManagerAddDlg::slotBtnOk()
{
	const QList<QListWidgetItem *> itemsToAdd = listWidget.selectedItems();
	QProgressDialog progressDlg("Reading file tags...", "Abbrechen", 0, listWidget.selectedItems().size(), this);
	progressDlg.setWindowModality(Qt::WindowModal);

	sqlhelper.start_insert_sequence();
	for(int cnt = 0; cnt != itemsToAdd.size(); ++cnt)
	{
#if 0
		//QString insert_cmd("INSERT INTO `musicdb`.`main` (`id` ,`titel` ,`kuenstler` ,`album` ,`tag` ,`genre` ,`jahr` ,`philipp` ,`johannes` ,`dateityp` ,`qualitaet` ,`bew_joh` ,`bew_phil` ,`pfad`) VALUES (NULL , '");
		QString insert_cmd("NULL , '");
		insert_cmd.append("', '");
		insert_cmd.append("', '");
		insert_cmd.append("', '");
		insert_cmd.append("', '");
		insert_cmd.append("', '");
		insert_cmd.append("', '");

		insert_cmd.append("1");
		insert_cmd.append("', '");
		insert_cmd.append("1");
		
		insert_cmd.append("', '");
		insert_cmd.append("', '");
		insert_cmd.append("', '");
		insert_cmd.append("', '");
		insert_cmd.append("', '");

		QString _filename = itemsToAdd[cnt]->text();
		_filename.replace('\'', "\\'");

		insert_cmd.append(_filename);
		insert_cmd.append("'");
		
		printf("Insert command: %s\n", insert_cmd.toLatin1().data());
		
		sqlhelper.INSERT(insert_cmd);
#else
		progressDlg.setValue(cnt);
		progressDlg.show();
		if (progressDlg.wasCanceled())
		 break;

		sqlhelper.INSERT(itemsToAdd[cnt]->text().toLatin1().data());
#endif
	}
	sqlhelper.stop_insert_sequence();

	if(itemsToAdd.size() > 0)
	 accept();
}
