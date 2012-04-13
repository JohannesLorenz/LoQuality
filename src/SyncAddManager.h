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
