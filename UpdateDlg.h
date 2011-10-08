#ifndef UPDATEDLG_H
#define UPDATEDLG_H

#include <QObject>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class UpdateDlg : public QDialog
{
	Q_OBJECT

	QListWidget commitOverview;
	QPushButton yesButton, noButton;

	QVBoxLayout topLayout;
	QHBoxLayout buttonLayout;

	void setupUi();
	void retranslateUi();
	void fetchItems();

private slots:
	void buttonYesPressed();

public:
	UpdateDlg();
};

#endif // UPDATEDLG_H
