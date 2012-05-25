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

#ifndef FILEADDBASE_H
#define FILEADDBASE_H

#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QCloseEvent>

class QDir;
class QListWidget;

class SqlHelper;

/*class FileProceedDlg : public QDialog
{
	Q_OBJECT

	private:
		const QTreeWidget* fileViewRef;

		QLabel dlgLabel;
		QVBoxLayout dlgLayout;

		QListWidget listWidget;
		QPushButton okBtn, abortBtn;
		QHBoxLayout buttonLayout;

	private slots:
		inline void slotBtnOk() { callbackBtnOk(); }

	public:
		virtual void callbackBtnOk() = 0;

		FileProceedDlg(const QTreeWidget* _fileViewRef, const SqlHelper& _sqlhelper) : sqlhelper(_sqlhelper), fileViewRef(_fileViewRef), dlgLayout(this) {
			setupUi();
			retranslateUi();
		}

		void setupUi();
		void retranslateUi();
};*/

class FileAddBase : public QVBoxLayout
{
	Q_OBJECT
private slots:
	//void slotAddFile();
	void selectionChanged();
//	void slotBtnDoIt();
private:
//	virtual void callbackProceed() = 0;
	const SqlHelper& sqlhelper;
	bool anything_changed;
	bool changing_selection; // mutex
	QList<QTreeWidgetItem*> lastSelection;
public:
	QTreeWidget fileView;
private:
	QHBoxLayout buttonLayout;
	QPushButton btnDoIt/*, btnDone*/;

	void retranslateUi();
	void setupUi();

	//void grepNewFiles(const QTreeWidgetItem* parentItem, QDir* currentDir, QListWidget* listView, const QList<QString>* filesInDb);
	void selectAllSubItems( QTreeWidgetItem* curItem, bool select );
public:
	inline bool isAnythingChanged() const { return anything_changed; }
	inline void setAnythingChanged() { anything_changed = true; }
	FileAddBase(const SqlHelper& _sqlhelper, QWidget* parent);
	inline void setProceedCallback(const QObject * receiver, const char * method) {
		connect(&btnDoIt, SIGNAL(clicked()), receiver, method);
	}
};

#endif // FILEADDBASE_H
