#ifndef UPDATEINFODLG_H
#define UPDATEINFODLG_H

#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class UpdateInfoDlg : public QObject
{
	Q_OBJECT

	QLabel infoLabel;
	QListWidget errorList;
	QPushButton okButton;

	QVBoxLayout topLayout;
	QHBoxLayout buttonLayout;

	void setupUi();
	void retranslateUi();

public:
	UpdateInfoDlg();
};

#endif // UPDATEINFODLG_H
