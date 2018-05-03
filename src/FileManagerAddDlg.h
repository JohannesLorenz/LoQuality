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

#ifndef FILEMANAGERADDDLG_H
#define FILEMANAGERADDDLG_H

#include <QDialog>
#include <QTreeWidget>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>

class QDir;
class SqlHelper;

class FileManagerAddDlg : public QDialog
{
	Q_OBJECT

	private:
		const SqlHelper& sqlhelper;

		const QTreeWidget* fileViewRef;

		QLabel dlgLabel;
		QVBoxLayout dlgLayout;

		QListWidget listWidget;
		QPushButton okBtn, abortBtn;
		QHBoxLayout buttonLayout;

		void grepNewFiles(const QTreeWidgetItem* parentItem, QDir* currentDir, QListWidget* listView, const QList<QString>* filesInDb);

	private slots:
		void slotBtnOk();

	public:
		FileManagerAddDlg(const QTreeWidget* _fileViewRef, const SqlHelper& _sqlhelper) : sqlhelper(_sqlhelper), fileViewRef(_fileViewRef), dlgLayout(this) {
			setupUi();
			retranslateUi();
		}

		void setupUi();
		void retranslateUi();
};

#endif // FILEMANAGERADDDLG_H
