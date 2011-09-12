/**
 * @file FileManager.h
 * @author Johannes Lorenz, Philipp Lorenz
 * @brief declaration of class FileManager
 */

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
