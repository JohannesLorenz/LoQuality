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

#ifndef SONGTABLEWIDGET_H
#define SONGTABLEWIDGET_H

#include <QTableWidget>
class SqlHelper;

class SongTableWidget : public QTableWidget
{
	Q_OBJECT

	SqlHelper& sqlhelper;
	inline int row2id(int given_id) {
		QTableWidgetItem* firstItem = item(given_id, 0);
		return firstItem->data(Qt::DisplayRole).toInt()/*-1*/;
		// ids begin with 1, columns with 0 - but not important here!
	}



public:
	//! reloads the table from the database
	void reloadTable();

	explicit SongTableWidget(SqlHelper& _sqlhelper, QWidget *parent = 0);

signals:

public slots:

	bool slotItemEdit(int row, int column);
	void slotRemoveSong();
	inline void slotScrollToSong() {
		if(! selectedItems().empty())
		 scrollToItem(*selectedItems().begin());
	}


};

#endif // SONGTABLEWIDGET_H
