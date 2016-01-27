/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2016                                               */
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

#include <cstdio>

#include "FileAddBase.h"

/*void FileProceedDlg::retranslateUi(void)
{
	dlgLabel.setText("Bitte Dateien de-selektieren, die nicht hinzugefügt werden sollen, dann bestätigen.");
	okBtn.setText("Ok");
	abortBtn.setText("Abbrechen");
}

void FileProceedDlg::setupUi(void)
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

	listWidget.selectAll();
}*/


void FileAddBase::retranslateUi()
{
	fileView.setHeaderLabel("Files Collection");
//	btnAdd.setText("Hinzufügen"); // TODO: add cool + and - icons...
//	btnErase.setText("Herausnehmen");
	btnDoIt.setText("Insert into database...");
//	btnDone.setText("Fertig!");
}

/*void FileAddBase::slotBtnDoIt ()
{
	puts("This should never happen.")
	exit(1);
	FileManagerAddDlg addDlg(&fileView, sqlhelper);
	addDlg.show();
	if( !anything_changed && addDlg.exec() == QDialog::Accepted ) {
		puts("CHANGED!\n");
		anything_changed = true;
	}
}*/

void FileAddBase::selectAllSubItems( QTreeWidgetItem* curItem, bool select )
{
	//selectionModel()
	curItem->setSelected(select);
	for(int i=0; i<curItem->childCount(); i++)
	{
		if( ! curItem->child(i)->isSelected() )
		 selectAllSubItems(curItem->child(i), select);
	}
}

void FileAddBase::selectionChanged (  )
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

void FileAddBase::setupUi()
{
	/*
		CENTRAL WIDGET
		*/
	fileView.setSelectionMode(QAbstractItemView::MultiSelection);
	addWidget(&fileView);
//	new QTreeWidgetItem( &fileView, QStringList() << "Music" );

	buttonLayout.addWidget(&btnDoIt);
	//buttonLayout.addWidget(&btnDone);
	addLayout(&buttonLayout);

	/*
		CONNECT SIGNALS TO SLOTS
		*/
//	connect(&btnDoIt, SIGNAL(clicked()), this, SLOT(slotBtnDoIt()));
	//connect(&btnDone, SIGNAL(clicked()), this, SLOT(close()));
	connect(&fileView, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

	/*
		OTHER
		*/
	retranslateUi();
}

FileAddBase::FileAddBase(const SqlHelper& _sqlhelper, QWidget *parent) :
	QVBoxLayout(parent),
	sqlhelper(_sqlhelper),
	anything_changed(false),
	changing_selection(false),

	// file view
	//fileView(this)
	fileView(parent)
{
	setupUi();
	retranslateUi();
}
