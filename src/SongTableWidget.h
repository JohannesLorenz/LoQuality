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

#ifndef SONGTABLEWIDGET_H
#define SONGTABLEWIDGET_H

#include <QTableWidget>
class SqlHelper;

class SongTableWidget : public QTableWidget
{
	Q_OBJECT

	QPoint dragStartPosition;
	SqlHelper& sqlhelper;

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	bool dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action);
	QMimeData *mimeData(const QList<QTableWidgetItem *> items) const;

public:
	//! reloads the table from the database
	void reloadTable();

	inline int row2id(int given_id) {
		QTableWidgetItem* firstItem = item(given_id, 0);
		return firstItem->data(Qt::DisplayRole).toInt()/*-1*/;
		// ids begin with 1, columns with 0 - but not important here!
	}

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
