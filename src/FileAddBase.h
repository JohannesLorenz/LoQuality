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
