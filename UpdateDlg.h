#ifndef UPDATEDLG_H
#define UPDATEDLG_H

#include <QObject>
#include <QDialog>
#include <QListWidget>

class UpdateDlg : public QDialog
{
	Q_OBJECT

	QListWidget commitOverview;

	void setupUi();
	void retranslateUi();

public:
	UpdateDlg();
};

#endif // UPDATEDLG_H
