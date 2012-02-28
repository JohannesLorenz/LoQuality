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

class FileAddBase : public QVBoxLayout
{
	Q_OBJECT
private slots:
	//void slotAddFile();
	void selectionChanged();
	void slotBtnDoIt();
private:
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
	FileAddBase(const SqlHelper& _sqlhelper, QWidget* parent);
};

#endif // FILEADDBASE_H
