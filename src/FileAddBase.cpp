#include <cstdio>

#include "FileAddBase.h"
#include "FileManagerAddDlg.h"

void FileAddBase::retranslateUi()
{
	fileView.setHeaderLabel("MusikSammlung");
//	btnAdd.setText("Hinzufügen"); // TODO: add cool + and - icons...
//	btnErase.setText("Herausnehmen");
	btnDoIt.setText("Rein in die Datenbank!");
//	btnDone.setText("Fertig!");
}

void FileAddBase::slotBtnDoIt ()
{
	FileManagerAddDlg addDlg(&fileView, sqlhelper);
	addDlg.show();
	if( !anything_changed && addDlg.exec() == QDialog::Accepted ) {
		puts("CHANGED!\n");
		anything_changed = true;
	}
}

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
	new QTreeWidgetItem( &fileView, QStringList() << "musik" );

	buttonLayout.addWidget(&btnDoIt);
	//buttonLayout.addWidget(&btnDone);
	addLayout(&buttonLayout);

	/*
		CONNECT SIGNALS TO SLOTS
		*/
	connect(&btnDoIt, SIGNAL(clicked()), this, SLOT(slotBtnDoIt()));
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
