#ifndef UPDATEDLG_H
#define UPDATEDLG_H

#include <QObject>
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressDialog>

class UpdateDlg : public QDialog
{
	Q_OBJECT

	QListWidget commitOverview;
	QPushButton yesButton, noButton;

	QVBoxLayout topLayout;
	QHBoxLayout buttonLayout;

//	QProgressDialog* progressDlg;
//	QTimer timer;

	/**
		Reads (short) revision output from git.
		@param output fd the text comes from
		@return true iff there are new revisions.
	*/
	bool readOutputToItems(int output);

	void setupUi();
	void retranslateUi();
	void fetchItems();

private slots:
	void buttonYesPressed();

public:
	UpdateDlg();
};

#endif // UPDATEDLG_H
