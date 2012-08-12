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

#include <QSqlError>
#include <QProgressDialog>
#include <QDateTime>
#include <QMessageBox>

#include "SqlHelper.h"
#include "AddEntries.h"

#include "SongTableWidget.h"

SongTableWidget::SongTableWidget(SqlHelper& _sqlhelper, QWidget *parent) :
	QTableWidget(parent),
	sqlhelper(_sqlhelper)
{

}

bool SongTableWidget::slotItemEdit(int row, int column)
{
#if 0
	Q_UNUSED(column);


	AddEntryDlg dlg(sqlhelper, true, row);
	dlg.show();
	dlg.exec();

	reloadTable();
#endif

	Q_UNUSED(row);
	Q_UNUSED(column);
	QList<QTableWidgetSelectionRange> ranges = selectedRanges();
	if(ranges.empty()) {
		QMessageBox::information(NULL, "Sorry...", "You can not edit 0 songs :) (at least, it would not make much sense)");
		return false;
	}

//			QTableWidgetItem* firstItem = tableWidget.item(ranges.front().topRow(), ranges.front().leftColumn());

	QList<int> selectedRows;
	for(QList<QTableWidgetSelectionRange>::const_iterator itr = ranges.begin(); itr!=ranges.end(); itr++)
	{
		for(int i = itr->topRow(); i <= itr->bottomRow(); i++)
		 selectedRows.append(row2id(i));
	}

	AddEntryDlg dlg(sqlhelper, true, &selectedRows);
	dlg.show();
	if(dlg.exec() == QDialog::Accepted)
	 reloadTable();

	return true;

	/*if(ranges.size() == 1 && ranges.front().rowCount() == 1) {
		AddEntryDlg dlg(sqlhelper, true, row2id(ranges.front().topRow()));
		dlg.show();
		if( dlg.exec() == QDialog::Accepted )
		 reloadTable();
	}
	else {
		QMessageBox::information(NULL, "Sorry...", "Es kann im Moment nur je eine Zeile bearbeitet werden! Bitte nur eine *Zelle* anklicken!");
	}*/
}

void SongTableWidget::slotRemoveSong()
{
#if 0 // TODO: re-implement me if QTableWidget or QTreeWidget is done
	if( tableWidget.selectedItems().size() < 1 )
		QMessageBox::information(NULL, "Kein Song ausgewaehlt...", "Du musst erst noch einen Song auswaehlen, um ihn zu loeschen...");
	else if( QMessageBox::Yes == QMessageBox::question(NULL, "Wirklich loeschen?", "Willst Du (TODO) wirklich aus der Wiedergabelist loeschen?",
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No) )
	 tableWidget.removeItemWidget(tableWidget.selectedItems().front());
#endif

	QList<QTableWidgetItem*> _selectedItems = selectedItems();
	// count number of selected rows
	unsigned int selectedRows;
	{
		QList<unsigned int> selectedRowList;
		for(QList<QTableWidgetItem*>::const_iterator itr = _selectedItems.begin(); itr!=_selectedItems.end(); itr++)
		 if(! selectedRowList.contains((*itr)->row()))
		  selectedRowList.append((*itr)->row());
		selectedRows = selectedRowList.size();
	}

	if(selectedRows < 1 ) {
		QMessageBox::information(NULL, "Kein Song ausgewaehlt...", "Du musst erst noch einen Song auswaehlen, um ihn zu loeschen...");
		return;
	}
	else if(selectedRows == 1) {
		QString songTitle = item( (*_selectedItems.begin())->row(), 1)->text();
		if(songTitle.isEmpty())
		 songTitle = "diesen Titel";
		if( QMessageBox::No == QMessageBox::question(NULL, "Wirklich loeschen?", QString("Willst Du <i>%1</i> wirklich aus der Wiedergabeliste loeschen?").arg( songTitle ),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No) )
			return;
	}
	else { // multiples...
		if( QMessageBox::No == QMessageBox::question(NULL, "Wirklich loeschen?", QString("Willst Du diese <b>%1 Songs</b> wirklich aus der Wiedergabeliste loeschen?").arg( selectedRows ),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No) )
			return;
	}

	for (QList<QTableWidgetItem*>::const_iterator itr = _selectedItems.begin(); itr != _selectedItems.end() && selectedRows > 0; itr++)
	{
		if( (*itr)->column() == 1)
		{
			sqlhelper.DELETE( row2id((*itr)->row()) );
			removeRow((*itr)->row());
			--selectedRows;
		}
	}
}

void SongTableWidget::reloadTable()
{
	setSortingEnabled(false); // no sorting during insertion - it takes too long

	clear();
	setRowCount(0);
	setColumnCount(15);

	QStringList description;

	description.insert(0, "ID");
	description.insert(1, "Titel");
	description.insert(2, "Künstler");
	description.insert(3, "Album");
	description.insert(4, "Tags");
	description.insert(5, "Genre");
	description.insert(6, "Jahr");
	description.insert(7, "Interessiert Dich?");
	description.insert(8, "Interessiert ...?");
	description.insert(9, "Typ");
	description.insert(10, "Qualität");
	description.insert(11, "Deine Bewertung");
	description.insert(12, "Bewertung von ...");
	description.insert(13, "Pfad");
	description.insert(14, "Last Changes");

	setHorizontalHeaderLabels(description);

	QSqlQuery query;
	query.exec("SELECT * FROM main;");
		printf("size: %d\n", query.size());
		printf("last error: %s\n", query.lastError().text().toAscii().data());

//	setRowCount(query.size());

	QProgressDialog progressDlg("Lade Musik-Datenbank...", "Abbrechen", 0, query.size(), this);
	progressDlg.setWindowModality(Qt::WindowModal);

	unsigned int rowcount=0;

	while (query.next()) { // WARNING: DO !!!NEVER!!! rely on query.size() here!!! (see qt docs...)

		progressDlg.setValue(rowcount);
		progressDlg.show();
		if (progressDlg.wasCanceled())
		 break;

		if(rowcount % 250 == 0)
		 setRowCount(rowcount+250);

		QString id = query.value(0).toString();
		QString title = query.value(1).toString();
		QString artist = query.value(2).toString();
		QString album = query.value(3).toString();
		QString tags = query.value(4).toString();
		QString genre = query.value(5).toString();
		QString year = query.value(6).toString();
		QString int_you = query.value(7).toString(); // int = interest
		QString int_other = query.value(8).toString();
		QString type = query.value(9).toString();
		QString quality = query.value(10).toString();
		QString vote_you = query.value(11).toString();
		QString vote_other = query.value(12).toString();
		QString path = query.value(13).toString();

		QDateTime _last_change = QDateTime::fromTime_t(query.value(14).toInt());
		QString last_change = (_last_change.isValid() && _last_change <= QDateTime::currentDateTime())?
			_last_change.toString("yyyy-MM-dd") : "(incorrect)";

		//QString &artistRef = artistAlbumList[artist];
		//if(! artistRef.contains(artist) )
		// artistRef.push_back(album);

		QTableWidgetItem* item_id = new QTableWidgetItem(id);
		QTableWidgetItem* item_title = new QTableWidgetItem(title);
		QTableWidgetItem* item_artist = new QTableWidgetItem(artist);
		QTableWidgetItem* item_album = new QTableWidgetItem(album);
		QTableWidgetItem* item_tags = new QTableWidgetItem(tags);
		QTableWidgetItem* item_genre = new QTableWidgetItem(genre);
		QTableWidgetItem* item_year = new QTableWidgetItem(year);
		QTableWidgetItem* item_int_other = new QTableWidgetItem(int_other);
		QTableWidgetItem* item_int_you = new QTableWidgetItem(int_you);
		QTableWidgetItem* item_type = new QTableWidgetItem(type);
		QTableWidgetItem* item_quality = new QTableWidgetItem(quality);
		QTableWidgetItem* item_vote_you = new QTableWidgetItem(vote_you);
		QTableWidgetItem* item_vote_other = new QTableWidgetItem(vote_other);
		QTableWidgetItem* item_path = new QTableWidgetItem(path);
		QTableWidgetItem* item_last_change = new QTableWidgetItem(last_change);

		item_id->setData(Qt::DisplayRole, id);
		item_title->setData(Qt::DisplayRole, title);
		item_artist->setData(Qt::DisplayRole, artist);
		item_album->setData(Qt::DisplayRole, album);
		item_tags->setData(Qt::DisplayRole, tags);
		item_genre->setData(Qt::DisplayRole, genre);
		item_year->setData(Qt::DisplayRole, year);
		item_int_other->setData(Qt::DisplayRole, int_other);
		item_int_you->setData(Qt::DisplayRole, int_you);
		item_type->setData(Qt::DisplayRole, type);
		item_quality->setData(Qt::DisplayRole, quality);
		item_vote_you->setData(Qt::DisplayRole, vote_you);
		item_vote_other->setData(Qt::DisplayRole, vote_other);
		item_path->setData(Qt::DisplayRole, path);
		item_last_change->setData(Qt::DisplayRole, last_change);

		setItem(rowcount, 0, item_id);
		setItem(rowcount, 1, item_title);
		setItem(rowcount, 2, item_artist);
		setItem(rowcount, 3, item_album);
		setItem(rowcount, 4, item_tags);
		setItem(rowcount, 5, item_genre);
		setItem(rowcount, 6, item_year);
		setItem(rowcount, 7, item_int_other);
		setItem(rowcount, 8, item_int_you);
		setItem(rowcount, 9, item_type);
		setItem(rowcount, 10, item_quality);
		setItem(rowcount, 11, item_vote_you);
		setItem(rowcount, 12, item_vote_other);
		setItem(rowcount, 13, item_path);
		setItem(rowcount, 14, item_last_change);

		++rowcount;
	}
	setRowCount(rowcount);

	hideColumn(0);
	hideColumn(7);
	hideColumn(8);
	hideColumn(12);
	hideColumn(13);

	resizeColumnsToContents ();
	resizeRowsToContents ();

	setSortingEnabled(true);
	sortByColumn(14, Qt::AscendingOrder);
}


