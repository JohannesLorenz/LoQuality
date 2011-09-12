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
