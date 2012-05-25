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

#ifndef SYNCADDMANAGER_H
#define SYNCADDMANAGER_H

#include <QWidget>
#include <QListWidget>
#include <QDialog>
#include <QLabel>

#include "FileAddBase.h"
class SqlHelper;

class SyncAddManagerOverview : public QDialog
{
	Q_OBJECT
private:
	const QTreeWidget* fileViewRef;

	QLabel dlgLabel;
	QVBoxLayout dlgLayout;

	QListWidget listWidget;
	QPushButton okBtn, abortBtn;
	QHBoxLayout buttonLayout;

	void setupUi();
	void retranslateUi();

	void grepFiles();

public:
	SyncAddManagerOverview(const QTreeWidget* _fileViewRef) : fileViewRef(_fileViewRef), dlgLayout(this) {
		setupUi();
		retranslateUi();
	}
};

class SyncAddManager : public QWidget
{
	Q_OBJECT
	FileAddBase fileAddBase;
private slots:
	void proceed();
public:
	bool appendItem(const QString& artist, const QString& album, const QString& title,
		const QString& filepath);
	SyncAddManager(const SqlHelper& _sqlhelper, QWidget* _parent = NULL);
	void fillListWidget(QListWidget* _listWidget);
signals:
	void signalSongsAdded();
};

#endif // SYNCADDMANAGER_H
