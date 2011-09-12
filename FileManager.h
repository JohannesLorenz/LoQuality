/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2011                                               */
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

#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_ _FILE_MANAGER_H_

#include <QDialog>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QCloseEvent>
class QDir;
class QListWidget;

/*#include <QMap>
#include <QToolBar>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QList>*/

class SqlHelper;

class FileManager : public QDialog
{
	Q_OBJECT
	
	private slots:
		//void slotAddFile();
		void selectionChanged();
		void slotBtnDoIt();
		
	private:
		bool anything_changed;
		const SqlHelper& sqlhelper;
		bool changing_selection; // mutex
		QList<QTreeWidgetItem*> lastSelection;
		QVBoxLayout topLayout;
		QTreeWidget fileView;

		QHBoxLayout buttonLayout;
		QPushButton btnDoIt, btnDone;
		//QRadioButton btnAdd, btnErase;
		bool appendToItem(QTreeWidgetItem* parentItem, QDir* currentDir, QListIterator<QString> dbItr);
		void grepNewFiles(const QTreeWidgetItem* parentItem, QDir* currentDir, QListWidget* listView, const QList<QString>* filesInDb);
		void selectAllSubItems( QTreeWidgetItem* curItem, bool select );

	signals:
		

	public:
		/**
			
			*/
		FileManager (const SqlHelper& _sqlhelper);
		void retranslateUi();
		inline bool isAnythingChanged() {
			return anything_changed;
		}
};

#endif // _FILE_MANAGER_H_
