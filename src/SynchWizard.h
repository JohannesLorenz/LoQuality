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
#include <QButtonGroup>
#include <QRadioButton>
#include <QGroupBox>
#include <QProgressBar>
#include <QTimer>

#include <QTableWidget> // TODO

#include "SyncAddManager.h"
#include "globals.h"
class SqlHelper;

enum PAGE_ID {
	PAGE_INTRO,
	PAGE_OPEN,
	PAGE_SELECT,
	PAGE_TRANSMIT,
	PAGE_SCRIPT,
	PAGE_SCRIPT_RUN,
	PAGE_SCRIPT_DUMP,
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
	OpenPage()
	{
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

class SelectPage : public QWizardPage
{
	Q_OBJECT

	SyncAddManager syncAddManager;
	QListWidget songList;
	QList<QListWidgetItem*>* selectedSongs;
	QHBoxLayout hbox;

public:
	bool getSongList();
	SelectPage(QList<QListWidgetItem*>* _selectedSongs, const SqlHelper& _sqlhelper) :
		syncAddManager(_sqlhelper, this),
		selectedSongs(_selectedSongs),
		hbox(this)
	{
		setTitle("Select files");
		setSubTitle("Please select all songs you would like to im- or export.");

		hbox.addWidget(&syncAddManager);
		hbox.addWidget(&songList);
		songList.setSelectionMode(QAbstractItemView::MultiSelection);

		registerField("songListWidget*", &songList);
		connect(&syncAddManager, SIGNAL(signalSongsAdded()), this, SLOT(slotSongsAdded()));
	}
	void initializePage() { getSongList(); }
	inline int nextId() const {
		*selectedSongs = songList.selectedItems();
		return PAGE_TRANSMIT;
	}
public slots:
	inline void slotSongsAdded() {
		syncAddManager.fillListWidget(&songList);
		QList<QListWidgetItem*> ptr = songList.selectedItems();
	}
};

class TransmitPage : public QWizardPage
{
	Q_OBJECT
	const QList<QListWidgetItem*>* selectedSongs;
	QListWidget fileBaseList;
	QLabel sampleCopy;
	QVBoxLayout vbox;

	QGroupBox hostGroupBox;
	QVBoxLayout vbox2;
	QLineEdit fileBase;
	QLineEdit nameAtHost, ip, port, rsaKey;

public slots:
	inline void slotSelectionChanged(const QString& newText) {
		fileBase.setText(newText);
		QString firstFile = selectedSongs->front()->text();
		const int num_chars_from_right = firstFile.length() - fileBase.text().length();

		sampleCopy.setText(QString("Example: Copying %1 to %2/%3")
			.arg(firstFile,globals::MUSIC_ROOT,
			firstFile.right(num_chars_from_right)));
	}
public:
	void runTransmission();
	inline TransmitPage(const QList<QListWidgetItem*>* _selectedSongs) :
		selectedSongs(_selectedSongs),
		vbox(this), /*vbox2(&hostGroupBox),*/ hostGroupBox("your login name, ip, port and rsa id file")
	{
		setTitle("Select music Base");
		setSubTitle("Please select from which directory you want to copy the songs from."
			"On proceeding, you will get an overview.");

		fileBaseList.setSelectionMode(QAbstractItemView::SingleSelection);
		fileBase.setReadOnly(true);
		registerField("fileBaseList*", &fileBaseList);
		registerField("fileBase*", &fileBase);

		registerField("nameAtHost", &nameAtHost);
		registerField("ip", &ip);
		registerField("port", &port);
		registerField("rsaKey", &rsaKey);

		vbox.addWidget(&fileBaseList);
		vbox.addWidget(&fileBase);
		vbox.addWidget(&sampleCopy);
		vbox.addWidget(&hostGroupBox);

		hostGroupBox.setLayout(&vbox2);
		vbox2.addWidget(&nameAtHost);
		vbox2.addWidget(&ip);
		vbox2.addWidget(&port);
		vbox2.addWidget(&rsaKey);
		rsaKey.setText("/home/whoever/.ssh/id_rsa");

		// TODO: remove this:


		connect(&fileBaseList, SIGNAL(currentTextChanged(const QString&)),this, SLOT(slotSelectionChanged(const QString&)));
	}

	void initializePage() { runTransmission(); }
	inline int nextId() const { return PAGE_SCRIPT; }
};

class ScriptPage : public QWizardPage
{
	QVBoxLayout vbox;
	QButtonGroup choice;
	QRadioButton btn_run, btn_script;
	QLabel scpLogon;
	//QLabel sampleCopy;
	//const QList<QListWidgetItem*>* selectedSongs;
public:
	inline ScriptPage(/*const QList<QListWidgetItem*>* selectedSongs*/) : vbox(this) {
		setTitle("Create script files");
		setSubTitle("Please check whether everything is correct.");

		btn_run.setText("Run directly, without script (only if public key-based authentication is installed)");
		btn_script.setText("Dump script and run remote");
		choice.addButton(&btn_run, 0);
		choice.addButton(&btn_script, 1);
		btn_run.setChecked(true);

		vbox.addWidget(&scpLogon);
		vbox.addWidget(&btn_run);
		vbox.addWidget(&btn_script);


		//QString firstFile =
		//sampleCopy.setText(QString("Example: Copying %1/%2 to %3/%4").arg(,,globals::MUSIC_ROOT,));
	}

	void initializePage() {
		QString base = field("fileBase").toString();
		printf("base: %s\n", base.toAscii().data());
		scpLogon.setText(QString("SSH user: %1@%2, port %3").arg(field("nameAtHost").toString(),
			field("ip").toString(),field("port").toString())
			);
	}
	inline int nextId() const { return (choice.checkedId() == 0) ? PAGE_SCRIPT_RUN
		: PAGE_SCRIPT_DUMP; }
};

class ScriptRunPage : public QWizardPage
{
	Q_OBJECT
	const QList<QListWidgetItem*>* selectedSongs;
	QHBoxLayout hbox;
	QProgressBar progressBar;
	QPushButton btnStart;

	QTimer scpTimer;
	pid_t scps_pid;

private slots:
	void slotTimerTimeout();

public:
	ScriptRunPage(const QList<QListWidgetItem*>* _selectedSongs) :
		selectedSongs(_selectedSongs),
		hbox(this)
	{
		hbox.addWidget(&progressBar);
		hbox.addWidget(&btnStart);
		btnStart.setText("Start");
		connect(&btnStart, SIGNAL(clicked()), this, SLOT(slotBtnClicked()));

		scpTimer.setInterval(200);
		QObject::connect(&scpTimer, SIGNAL(timeout()), this, SLOT(slotTimerTimeout()));
	}
public slots:

	void slotBtnClicked() {
		//btnStart.set...
		scpTimer.start();
		runNext();
	}

	bool forkScp(QString nextFile);

	void runNext() {
		puts("RUN NEXT");
		static QListIterator<QListWidgetItem*> itr(*selectedSongs);

		printf("itr.hasNext? %d\n",itr.hasNext());
		printf("size: %d\n",selectedSongs->size());
		if(itr.hasNext())
		// run(itr.next()->text());
		{
		/*QListIterator<QListWidgetItem*> itr(*selectedSongs);
		while (itr.hasNext()) {*/

			progressBar.setValue(progressBar.value()+1);
			forkScp(itr.next()->text());

			//nextFile = itr.next()->text();
		}
		else
		 scpTimer.stop();
	}

	void initializePage() {
		//itr.toFront();
		progressBar.setMaximum(selectedSongs->size());
	}

	inline int nextId() const { return -1; }
};

class ScriptDumpPage : public QWizardPage
{
	QLabel infoLabel;
public:
	ScriptDumpPage() {
		infoLabel.setText("Processing Shell-Script... Please wait.");
	}

	void initializePage() {
		infoLabel.setText("Done...");
	}
	inline int nextId() const { return -1; }
};

class SynchWizard : public QWizard
{
	Q_OBJECT;
private:
	QList<QListWidgetItem*> selectedSongs;
	IntroPage introPage;
	OpenPage openPage;
	SelectPage selectPage;
	TransmitPage transmitPage;
	ScriptPage scriptPage;
	ScriptRunPage scriptRunPage;
	ScriptDumpPage scriptDumpPage;

	void retranslateUi();
	void setupUi();
public:
	SynchWizard(const SqlHelper& _sqlhelper);
};

#endif // SYNCHWIZARD_H
