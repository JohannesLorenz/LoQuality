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

#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_ _FILE_MANAGER_H_

#include <QDialog>
class QFileInfo;

/*#include <QMap>
#include <QToolBar>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QList>*/

#include "SqlHelper.h"
#include "FileAddBase.h"

class FileManager : public QDialog
{
	Q_OBJECT
	private:
		SqlHelper sqlhelper;
		FileAddBase fileAddBase;

	//	bool appendSingle(QTreeWidgetItem *curItem, QDir *currentDir, QList<QFileInfo>::const_iterator& itr, QListIterator<QString> &dbItr);

		void sortOutUseless(QTreeWidgetItem *parentItem, QDir *currentDir, QListIterator<QString> dbItr, bool removeTokenDirs);
		void appendAllDirectories(QTreeWidgetItem* parentItem, QDir* currentDir); // TODO: make currentDir const and copy each time (
		void removeUnusedDirs(QTreeWidgetItem* parentItem, QDir* currentDir, QListIterator<QString> dbItr, bool removeTokenDirs);
		bool appendToItem(QTreeWidgetItem* parentItem, QDir* currentDir, QListIterator<QString> dbItr, bool removeTokenDirs);
	private slots:
		void proceed();
	public:
		inline bool isAnythingChanged() const { return fileAddBase.isAnythingChanged(); }
		FileManager (const SqlHelper& _sqlhelper);
};

#endif // _FILE_MANAGER_H_
