#ifndef SYNCHWIZARD_H
#define SYNCHWIZARD_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QWizard>
#include <QWizardPage>
#include <QButtonGroup>
#include <QRadioButton>
#include <QFileDialog>
#include <QCheckBox>

#include <QTableWidget> // TODO

#include "SyncAddManager.h"
class SqlHelper;

enum PAGE_ID {
	PAGE_INTRO,
	PAGE_OPEN,
	PAGE_SELECT,
	PAGE_COUNT
};

class IntroPage : public QWizardPage
{
	QLabel infoLabel;
public:
	IntroPage() : infoLabel(this) {
		setTitle("Introduction");
		infoLabel.setWordWrap(true);
		infoLabel.setText("This wizard will help synchronize your files with "
				"another computer or USB stick.<br/><br/>"
				"Please note that sharing files is only allowed if the"
				"creator of them gave you permission."
				"LoQuality does not encourage illegal file transmissions!");

	}
	inline int nextId() const { return PAGE_OPEN; }
};

class OpenPage : public QWizardPage
{
	Q_OBJECT;

	QHBoxLayout hbox;
	QLineEdit lineEdit;
	QPushButton selectButton;
	QButtonGroup import_or_export;
	QRadioButton btn_import, btn_export;
private slots:
	void selectDatabase();
public:
	OpenPage() {
		setTitle("Open database");
		setSubTitle("Please select the database of the other PC or usb stick.");
		setLayout(&hbox);

		lineEdit.setReadOnly(true);

		hbox.addWidget(&lineEdit);
		hbox.addWidget(&selectButton);

		registerField("importDbName*", &lineEdit);
		connect(&selectButton,SIGNAL(clicked ()), this, SLOT(selectDatabase()));

		/*import_or_export.addButton(&btn_import);
		import_or_export.addButton(&btn_export);
		//cur_vbox->addWidget(&import_or_export);
		cur_vbox->addWidget(&btn_import);
		cur_vbox->addWidget(&btn_export);*/
		/* retranslate:
		btn_import.setText("Import Files");
		btn_export.setText("Export Files");*/
	}
	inline int nextId() const { return PAGE_SELECT; }
};

class SelectPage : public QWizardPage {
	//QTableWidget newSongs;

	SyncAddManager syncAddManager;
	//QTableWidget newSongs;
public:
	bool getSongList();
	SelectPage(const SqlHelper& _sqlhelper)
		: syncAddManager(_sqlhelper)/*, newSongs(this)*/ {
		setTitle("Select files");
		setSubTitle("Please select all songs you would like to im- or export.");
	}
	void initializePage() { getSongList(); }
	inline int nextId() const { return -1; }
};


class SynchWizard : public QWizard
{
	Q_OBJECT;
private:
	IntroPage introPage;
	OpenPage openPage;
	SelectPage selectPage;
	//QString db_name;

	void retranslateUi();
	void setupUi();
public:
	SynchWizard(const SqlHelper& _sqlhelper);
};

#endif // SYNCHWIZARD_H
