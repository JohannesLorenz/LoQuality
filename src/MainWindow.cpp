/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2012                                               */
/* Johannes Lorenz, Philipp Lorenz                                       */
/* https://github.com/JohannesLorenz/LoQuality                           */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#include <sys/wait.h>
#include <sys/stat.h>

#include <QLocale>
#include <QMessageBox>
#include <QBitArray>
#include <QDateTime>

#include "MainWindowIcons.h"
// TODO: get new icons from http://websvn.kde.org/trunk/kdesupport/oxygen-icons/

#include "MainWindow.h"
#include "FlashDlg.h"
#include "SynchWizard.h"
#include "FileManager.h"

void MainWindow::retranslateUi()
{
	topMenus[MENU_FILE].setTitle(tr("Datei"));
	topMenus[MENU_EDIT].setTitle(tr("Bearbeiten"));
	topMenus[MENU_VIEW].setTitle(tr("Ansicht"));
	topMenus[MENU_HELP].setTitle(tr("Hilfe"));

	Actions[ACTION_FILE_OPEN]->setText(tr("Open Database in new Window"));
	Actions[ACTION_FILE_UPDATE]->setText(tr("Update"));
	Actions[ACTION_FILE_CLOSE]->setText(tr("Close"));
	Actions[ACTION_FILE_QUIT]->setText(tr("Quit"));
	Actions[ACTION_VIEW_SCROLL_TO_SONG]->setText(tr("Scroll zum aktuellen Song"));
	Actions[ACTION_VIEW_FULLSCREEN]->setText(tr("Vollbild"));
	Actions[ACTION_VIEW_SWITCH_ALIGN]->setText(tr("Ausrichtung ändern"));
	Actions[ACTION_EDIT_EDITTITLE]->setText(tr("Titel-Info bearbeiten..."));
	Actions[ACTION_EDIT_DOWNLOAD]->setText(tr("Download missing tracks..."));
	Actions[ACTION_EDIT_OPTIONS]->setText(tr("Einstellungen"));
	Actions[ACTION_HELP_ABOUT]->setText(tr("Über LoQuality"));
	Actions[ACTION_HELP_ABOUTQT]->setText(tr("Über Qt"));
	
	
	buttons1[BTN1_PLAY].setToolTip("Spielen!");
	buttons1[BTN1_PAUSE].setToolTip("Pause");
	buttons1[BTN1_STOP].setToolTip("Anhalten");
	buttons1[BTN1_RANDOM].setToolTip("Zufall");
	buttons1[BTN1_BWD].setToolTip("Zurueck");
	buttons1[BTN1_FWD].setToolTip("Vorwaerts");
	buttons1[BTN1_REPEAT].setToolTip("Wiederholen");

	buttons2[BTN2_SPECIAL_FILTER].setText("Special Filter");
	buttons2[BTN2_SYNCH_USB].setText("Synch USB");
	buttons2[BTN2_SYNCH_MP3].setText("Synch MP3");
	buttons2[BTN2_ADD].setText("Hinzufuegen");
	buttons2[BTN2_REMOVE].setText("Loeschen");
	buttons2[BTN2_FLASH].setText("Flash Download");

	labelYearFilter.setText("Jahr");
	labelRatingFilter.setText("Rating");
	playUnratedFilter.setText("spiele unbewertete");

	infoActionDownload.setText("Load current album's cover.");
	//x1Filter.setText("Person 1");
	//x2Filter.setText("Person 2");
}
	
QAction* MainWindow::initAction (enum MENU menuNo, enum ACTION actionNo, const char* slotName,
					const QKeySequence& shortKeySequence, const char* const xpmStr[])
{
	QAction* new_action = new QAction(this);
	
	if(slotName != NULL) {
		QObject::connect(new_action, SIGNAL(triggered()), this, slotName);
	}
	if(!shortKeySequence.isEmpty()) {
		new_action->setShortcut(shortKeySequence);
	}
	if(xpmStr != NULL) {
		QPixmap* pixmap = new QPixmap(xpmStr);
		QIcon* icon = new QIcon(*pixmap);
		new_action->setIcon(*icon);
	}
	
	if(menuNo != MENU_SIZE)
		topMenus[menuNo].addAction(new_action);
	
	Actions[actionNo] = new_action;
	return new_action;
}

void MainWindow::initButton1 (enum BTN1 btn1No, const char* slotName, const char* const xpmStr[])
{
	QPushButton* interestingButton = buttons1 + btn1No;
	if(slotName != NULL)
	 QObject::connect(interestingButton, SIGNAL(clicked()), this, slotName);
	if(xpmStr != NULL)
	 interestingButton->setIcon( QIcon(QPixmap(xpmStr)) );
}

void MainWindow::initButton2 (enum BTN2 btn2No, const char* slotName /*, const char* const xpmStr[]*/)
{
	QPushButton* interestingButton = buttons2 + btn2No;
	if(slotName != NULL)
	 QObject::connect(interestingButton, SIGNAL(clicked()), this, slotName);
//	if(xpmStr != NULL)
//	 interestingButton->setIcon( QIcon(QPixmap(xpmStr)) );
}

void MainWindow::onSetStatus(STATUS_FLAGS new_status)
{
	printf("New Status: %d\n", (int)new_status);
	switch(new_status)
	{
		case PlayerEngine::STATUS_PLAYING:
			buttons1[BTN1_PLAY].setDisabled(true);
			buttons1[BTN1_STOP].setEnabled(true);
			buttons1[BTN1_PAUSE].setEnabled(true);
			break;
		case PlayerEngine::STATUS_SONGOVER:
			buttons1[BTN1_STOP].setDisabled(true);
			buttons1[BTN1_PLAY].setEnabled(true);
			buttons1[BTN1_PAUSE].setDisabled(true);
			slotForward(); // load next song (but do not play it, maybe)
			break;
		case PlayerEngine::STATUS_SONGLOADED:
			/*if( stopButtonMenu.actions().at(1)->isChecked() )
			{
				if(time_to_stop != 0 && time_to_stop <= time(NULL))
				 { time_to_stop = 0; }
				else
				 time_to_stop =
			}*/
			printf("time to stop: %d\n", (int)time_to_stop);
			if(time_to_stop != 0 && time_to_stop <= time(NULL))
			{
				player.slotStop();
			}
			else
			{
				if( stopButtonMenu.actions().at(1)->isChecked() )
				 time_to_stop = time(NULL);

				if(toolBox->currentIndex() == TOOLBOX_METAINFO) {
					slotToolBoxChanged(TOOLBOX_METAINFO);
				}
				player.slotStartPlayback();
			}
			break; // this break is important against concurrency...
		case PlayerEngine::STATUS_STOPPED:
			if(time_to_stop != 0 && time_to_stop <= time(NULL))
			 time_to_stop = 0; // do nothing, but allow next song to be played
			buttons1[BTN1_STOP].setDisabled(true);
		case PlayerEngine::STATUS_PAUSED:
			buttons1[BTN1_PLAY].setEnabled(true);
			buttons1[BTN1_PAUSE].setDisabled(true);
	}
}

/*
	BUTTONS SLOTS
*/
void MainWindow::slotPlay(int row, int column)
{
	Q_UNUSED(row);
	Q_UNUSED(column);


/*	if(player_status == STATUS_PAUSED)
	 pass_remote_command("pause\n"); // unpause song
	else if( tableWidget.selectedItems().empty() ) {
		QMessageBox::information(NULL, "Kann nicht abspielen...", "Bitte erst einen Song auswaehlen.");
		return;
	}
	else {
		int selectedRow = tableWidget.selectedItems().front()->row();
		pass_remote_command( QString("loadfile %1\n").arg(tableWidget.item(selectedRow, 13)->text()).toAscii().data() );
	}
	setStatus(STATUS_PLAYING);*/

	/*if( tableWidget.selectedItems().empty() ) {
		player.slotPlay( NULL );
	}
	else {
		int selectedRow = tableWidget.selectedItems().front()->row();
		player.slotPlay( tableWidget.item(selectedRow, 13)->text().toAscii().data() );
	}*/

	//QList<QTableWidgetItem*> items = tableWidget.selectedItems();
	//player.slotPlay(buttons1[BTN1_RANDOM].isChecked(), items.isEmpty()?NULL:items.front());

	//player.slotPlay(buttons1[BTN1_RANDOM].isChecked(), (row==-1)?NULL:tableWidget.item(row,13));
	player.slotPlay(); // that simple :P

//	player.slotChangeVolume(volumeSlider.sliderPosition()); // set volume again - only to be sure
}

void MainWindow::slotPause() {
	player.slotPause();
}

void MainWindow::slotStop() {
	player.slotStop();
}

void MainWindow::slotForward(int row, int column)
{
	Q_UNUSED(column);
	//QList<QTableWidgetItem*> items = tableWidget.selectedItems();
	//player.slotForward(buttons1[BTN1_RANDOM].isChecked(), NULL);
	const bool doubleClicked = (row!=-1);
	player.slotForward(buttons1[BTN1_RANDOM].isChecked(), doubleClicked?tableWidget.item(row,13):NULL, doubleClicked);
}

void MainWindow::slotBackward() {
	player.slotBackward();
}

void MainWindow::slotAddFileChooser()
{
	FileManagerWizard fileManager(sqlhelper);
	fileManager.show();
	fileManager.exec();
	if(fileManager.isAnythingChanged())
	 tableWidget.reloadTable();
}

void MainWindow::slotAddFile()
{
	AddEntryDlg dlg(sqlhelper);
	dlg.show();
	if( dlg.exec() == QDialog::Accepted )
	 tableWidget.reloadTable();
}

void MainWindow::slotStoreFlash()
{
	FlashDlg fdlg(sqlhelper);
	fdlg.show();
	if( fdlg.exec() == QDialog::Accepted )
	 tableWidget.reloadTable();
}

void MainWindow::slotSynch()
{
	SynchWizard synch_wizard(sqlhelper);
	synch_wizard.show();
	if( synch_wizard.exec() == QDialog::Accepted )
	 tableWidget.reloadTable();
}

/*
	MENU BAR SLOTS
*/
void MainWindow::slotFileQuitAction () {
	quitProgram = true;
	mwContainer->closeAllWindows();
}

void MainWindow::slotFileCloseAction () {
	quitProgram = true;
	close();
}

void MainWindow::slotHelpAboutAction ()
{
	QMessageBox::about ( NULL, "About - LoQuality",
				"<h1>LoQuality</h1>"
				"<i>(c) 2010-2012</i><br/>"
				"by Philipp Lorenz, Johannes Lorenz<br/><br/>"
				"<a href=\"https://github.com/JohannesLorenz/LoQuality\">https://github.com/JohannesLorenz/LoQuality</a>");
				
}

void MainWindow::slotHelpAboutQtAction () {
	QMessageBox::aboutQt ( NULL, tr("About - Qt") );
}

void MainWindow::slotFilterChanged ( const QString & text )
{
	QList<QTableWidgetItem*> visItems = tableWidget.findItems(text, Qt::MatchContains);
	QBitArray hiddenArray(tableWidget.rowCount(), true);
	int filterCount = 0;
	for(QList<QTableWidgetItem*>::const_iterator listitr = visItems.begin(); listitr != visItems.end(); listitr++) {
		hiddenArray[(*listitr)->row()] = false;
	}

	for(int i = 0; i < tableWidget.rowCount(); i++) {
		tableWidget.setRowHidden(i, hiddenArray[i]);
		filterCount += (int) !(hiddenArray[i]);
	}
	player.setFilterCount(filterCount);
	slotScrollToSong();
}

void MainWindow::slotChangeVolume (int newValue) {
	player.slotChangeVolume(newValue);
}

#if 0
void MainWindow::slotSelectionChanged(int a, int b)
{
	Q_UNUSED(b);
//	if(lastMouseBtn==Qt::LeftButton)
	 selectRow(a);
}
#endif

bool loadImageSafely(QImage* img, const char* path)
{
	return (img->load(path, "PNG")) ||
		(img->load(path, "JPG")) ||
		(img->load(path, "JPEG")) ||
		(img->load(path));
}

void MainWindow::slotToolBoxChanged(int newIndex)
{
	if(newIndex == TOOLBOX_METAINFO)
	{
		if(player.getCurSongItem())
		{
			QDir currentDir(QFileInfo(player.getCurSongItem()->text()).dir());
			QStringList imgs = currentDir.entryList(QStringList() << "*.jpg" << "*.jpeg" << "*.png" << "*.xpm" << "*.gif", QDir::Files);
			printf("Scanning Dir: %s, imgs: %d\n",currentDir.absolutePath().toAscii().data(), imgs.size() );
			if(imgs.empty())
			{
				loadImageSafely(&tmpImage, "media/graphics/lq.png");
				imageLabel.setPixmap(QPixmap::fromImage(tmpImage.scaledToHeight(300)));
				imageLabel.resize(tmpImage.size());
				//imageLabel.clear();
			}
			else
			{
				int imgNumber = (int) (imgs.size()*1.0*rand()/(RAND_MAX+1.0));

				QString chosen_image = currentDir.absoluteFilePath(imgs.at(imgNumber));
				printf("... chosen image: %s\n",chosen_image.toAscii().data());
				loadImageSafely(&tmpImage,chosen_image.toAscii().data());
				printf("size: %d\n",tmpImage.height());
				imageLabel.setPixmap(QPixmap::fromImage(tmpImage.scaledToHeight(300)));
				//imageLabel.resize(tmpImage.height()+1, tmpImage.width()+1);
				printf("size: %d\n",tmpImage.height());
				/*imageLabel.adjustSize();
				imageLabel.show();
				toolBox->update();
				informationBox.update();*/

			}

			mainSplitter.setSizes(mainSplitter.sizes()); // update splitter
		}
		else
		 imageLabel.clear();
		//toolBox->currentWidget()->adjustSize();
		//toolBox->currentWidget()->resize(500, 500);

		//		toolBox->currentWidget()->adjustSize();
		toolBox->currentWidget()->show();

		toolBox->update();

//		toolBox->resize(500, 500);
		// TODO
		//toolBox->adjustSize();

	}
}

void MainWindow::layoutWidgets(bool mobile)
{
	Q_UNUSED(mobile);

	/*
		MENU BAR
		*/
	if(!mobile)
	 resize(792, 768);

	setMenuBar(&menubar);
	for(unsigned int i=0; i<MENU_SIZE; i++) {
		menubar.addAction(topMenus[i].menuAction());
	}

	/*
		TOOL BAR
		*/
	toolBar.setIconSize(QSize(24, 24));
	addToolBar(Qt::TopToolBarArea, &toolBar);

	/*
		CENTRAL WIDGET
		*/
	setCentralWidget(&centralWidget);

	if(mobile)
	{
		/*
			TAB 1 : music buttons
		*/
		stopButtonMenu.addAction( "Sofort!", this, SLOT(slotStop()) /*, const QKeySequence & shortcut = 0 TODO*/);
		stopButtonMenu.addAction( "Nach diesem Song.", this, SLOT(slotStopAfter()));
		stopButtonMenu.addAction( "Nach ... Minuten" /*TODO*/ );
		stopButtonMenu.actions().at(1)->setCheckable(true);
		buttons1[BTN1_STOP].setMenu(&stopButtonMenu);
		buttons1[BTN1_RANDOM].setCheckable(true);
		buttons1[BTN1_REPEAT].setCheckable(true);

		for(unsigned int i = 0; i < BTN1_SIZE; i++)
		 hbox_buttons1.addWidget(& buttons1[i]);

		mobileButtonsVBox.addLayout(&hbox_buttons1);
		mobileButtonsVBox.addWidget(&progressBar);

		mobileTab1.setLayout(&mobileButtonsVBox);
		mobileTab.addTab(&mobileTab1, "Play");

		//mobileTab.addTab(&progressBar, "Play");

		/*
			TAB 2 : graphics
		*/
		mobileTab.addTab(&imageLabel, "Image");

		/*
			TAB 3 : table
		*/
		mobileTableVBox.addWidget(&filter);
		mobileTableVBox.addWidget(&tableWidget);
		mobileTab3.setLayout(&mobileTableVBox);
		mobileTab.addTab(&mobileTab3, "Songs");

		/*
			TAB 4: special buttons
		*/
		mobileTab4.setLayout(&mobileSpecialHBox);
		mobileTab.addTab(&mobileTab4, "Special");

		mobileTab.setCurrentIndex(2);

		verticalLayout.addWidget(&mobileTab);
	}
	else
	{
		hbox_buttons1.addWidget(& filter);


		//connect(buttons1 + BTN1_STOP, SIGNAL(clicked()), this, SLOT(slotStop()));
		stopButtonMenu.addAction( "Sofort!", this, SLOT(slotStop()) /*, const QKeySequence & shortcut = 0 TODO*/);
		stopButtonMenu.addAction( "Nach diesem Song.", this, SLOT(slotStopAfter()));
		stopButtonMenu.addAction( "Nach ... Minuten" /*TODO*/ );
		stopButtonMenu.actions().at(1)->setCheckable(true);
		buttons1[BTN1_STOP].setMenu(&stopButtonMenu);
		buttons1[BTN1_RANDOM].setCheckable(true);
		buttons1[BTN1_REPEAT].setCheckable(true);

		for(unsigned int i = 0; i < BTN1_SIZE; i++)
		 hbox_buttons1.addWidget(& buttons1[i]);
		hbox_buttons1.addWidget(&progressBar);
		hbox_buttons1.addWidget( &volumeSlider );
		verticalLayout.addLayout(&hbox_buttons1);

		insertButtonMenu.addAction( "File Chooser", this, SLOT(slotAddFileChooser()) /*, const QKeySequence & shortcut = 0 TODO*/);
		insertButtonMenu.addAction( "Per Dialog", this, SLOT(slotAddFile()), QKeySequence(Qt::CTRL + Qt::Key_A));
		buttons2[BTN2_ADD].setMenu(&insertButtonMenu);
		hbox_buttons2.addWidget(& filter);
		for(unsigned int i = 0; i < BTN2_SIZE; i++)
		 hbox_buttons2.addWidget(& buttons2[i]);
		verticalLayout.addLayout(&hbox_buttons2);

		playUnratedFilter.setChecked(true);

		specialFilters.addWidget(&labelYearFilter);
		specialFilters.addWidget(&minYearFilter);
		specialFilters.addWidget(&maxYearFilter);
		specialFilters.addWidget(&labelRatingFilter);
		specialFilters.addWidget(&minRatingFilter);
		specialFilters.addWidget(&maxRatingFilter);
		specialFilters.addWidget(&playUnratedFilter);
		//specialFilters.addWidget(&x1Filter);
		//specialFilters.addWidget(&x2Filter);

	//	specialFiltersContainer.setLayout(&specialFilters);

		QWidget* newWidget = new QWidget(&mainSplitter);
		newWidget->setLayout(&specialFilters);



	//	toolBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	//	toolBox->adjustSize();
		toolBox->addItem(newWidget, "SpecialFilters"); // TODO: Icons :)

		informationBox.addWidget(&imageLabel, 1);
		//infoOptionsMenu.append();

		//optionsButton.setText("");
		optionsButton.setMenu(&infoOptionsMenu);
		infoOptionsMenu.addAction(&infoActionDownload);
		informationBox.addWidget(&optionsButton);

		newWidget = new QWidget(toolBox);
		newWidget->setLayout(&informationBox);
		toolBox->addItem(newWidget, "Information");

		toolBox->setCurrentIndex(1);

		verticalLayout.addWidget(&mainSplitter, 1);
		mainSplitter.addWidget(toolBox);
		mainSplitter.addWidget(&tableWidget);
		//mainSplitter.setStretchFactor(0, 1);
		//mainSplitter.setStretchFactor(1, 0);
	}

	/*
		STATUS BAR
		*/
	setStatusBar(&statusBar);

}

inline void removeAllChildrenFrom(QLayout* obj)
{
	const QObjectList& children = obj->children();
	while(! children.empty())
	 obj->removeWidget((QWidget*) children.front());
}

inline void removeAllLayoutsFrom(QLayout* obj)
{
	const unsigned int num_layouts = obj->count();
	for (unsigned int i = 0; i<num_layouts; i++)
	 obj->removeItem(obj->itemAt(i));
}

void MainWindow::freeLayout()
{

	/*
		MENU BAR
		*/
	exit(99);
	menubar.clear(); // delete all actions


/*
  TODO: ab hier...
  */
	removeAllChildrenFrom(&hbox_buttons1);
	removeAllChildrenFrom(&hbox_buttons2);

	stopButtonMenu.clear();

	removeAllLayoutsFrom(&verticalLayout);

	removeAllChildrenFrom(&specialFilters);

	// TODO: ab hier
	QWidget* newWidget = new QWidget(&mainSplitter);
	newWidget->setLayout(&specialFilters);
	toolBox = new QToolBox(&mainSplitter);
	toolBox->addItem(newWidget, "SpecialFilters"); // TODO: Icons :)

	removeAllChildrenFrom(&informationBox);
	infoOptionsMenu.clear();

	newWidget = new QWidget(toolBox);
	newWidget->setLayout(&informationBox);
	toolBox->addItem(newWidget, "Information");

	mainSplitter.addWidget(toolBox);
	mainSplitter.addWidget(&tableWidget);
}

MainWindow::MainWindow (MainWindowContainer* _mwContainer, const QString& dbname, const bool mobile, QWidget* parent)
	:
	QMainWindow(parent),

	// non gui
	mwContainer(_mwContainer),
	sqlhelper(dbname),
	player(&tableWidget),

	// menu bar
	menubar(this),

	// tool bar
	toolBar(this),

	// central widget
//	buttons1(
//		      	(QIcon(QPixmap(media_playback_start_xpm)), "", this) ),
	centralWidget(this),
	verticalLayout(&centralWidget),
	mobileTab(NULL),

	mainSplitter(Qt::Horizontal, (mobile)?(NULL):&centralWidget),

	toolBox(new QToolBox(&mainSplitter)),
	infoActionDownload(this),

	//table widget
	tableWidget(sqlhelper, (mobile)?(QWidget*)(NULL):(QWidget*)(&mainSplitter)),

	// others
	popupMenu(&tableWidget),
	time_to_stop(0),

	// status bar
	statusBar(this)

{
	{
		QString windowTitle = dbname;
		windowTitle.replace('_', ' ');
		bool lastWasSpace = true;
		for(int i = 0; i<windowTitle.size(); i++)
		{
			if( lastWasSpace )
			 windowTitle[i] = windowTitle[i].toUpper();
			lastWasSpace = (windowTitle[i] == ' ');
		}
		setWindowTitle(windowTitle);
	}
	layoutWidgets(mobile);
	/*
		MENU BAR
		*/
	Actions.resize(ACTION_SIZE);

	initAction(MENU_FILE, ACTION_FILE_OPEN, SLOT(slotOpenNewMainWindow()), QKeySequence(Qt::CTRL + Qt::Key_O));
	initAction(MENU_FILE, ACTION_FILE_UPDATE, SLOT(slotFileUpdateAction()), QKeySequence(Qt::CTRL + Qt::Key_U));
	topMenus[MENU_FILE].addSeparator();
	initAction(MENU_FILE, ACTION_FILE_CLOSE, SLOT(slotFileCloseAction()), QKeySequence(Qt::CTRL + Qt::Key_C));
	initAction(MENU_FILE, ACTION_FILE_QUIT, SLOT(slotFileQuitAction()), QKeySequence(Qt::CTRL + Qt::Key_Q), application_exit_xpm);

	initAction(MENU_EDIT, ACTION_EDIT_EDITTITLE, SLOT(slotItemEdit()), QKeySequence(Qt::CTRL + Qt::Key_E));
	initAction(MENU_EDIT, ACTION_EDIT_DOWNLOAD, SLOT(slotEditDownload()), QKeySequence(Qt::CTRL + Qt::Key_D));
	topMenus[MENU_EDIT].addSeparator();
	initAction(MENU_EDIT, ACTION_EDIT_OPTIONS, SLOT(slotOpenOptionsDlg()), QKeySequence(Qt::CTRL + Qt::Key_B)); // note: is B good?

	initAction(MENU_VIEW, ACTION_VIEW_SCROLL_TO_SONG, SLOT(slotScrollToSong()), QKeySequence("^"));
	topMenus[MENU_VIEW].addSeparator();
	initAction(MENU_VIEW, ACTION_VIEW_FULLSCREEN, SLOT(slotViewFullscreenAction()), QKeySequence(Qt::CTRL + Qt::Key_F), view_fullscreen_xpm);
	initAction(MENU_VIEW, ACTION_VIEW_SWITCH_ALIGN, SLOT(slotViewSwitchAlignment()), QKeySequence(Qt::CTRL + Qt::Key_Tab));

	initAction(MENU_HELP, ACTION_HELP_ABOUT, SLOT(slotHelpAboutAction()));
	initAction(MENU_HELP, ACTION_HELP_ABOUTQT, SLOT(slotHelpAboutQtAction()));

	/*
		TOOL BAR
		*/
	toolBar.setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
	
	/*
		CENTRAL WIDGET
		*/
	initButton1(BTN1_PLAY, SLOT(slotPlay()), media_playback_start_xpm);
	initButton1(BTN1_PAUSE, SLOT(slotPause()), media_playback_pause_xpm);
	initButton1(BTN1_STOP, SLOT(slotStop()), media_playback_stop_xpm);
	initButton1(BTN1_RANDOM, NULL, media_playlist_shuffle_xpm);
	initButton1(BTN1_BWD, SLOT(slotBackward()), media_skip_backward_xpm);
	initButton1(BTN1_FWD, SLOT(slotForward()), media_skip_forward_xpm);
	initButton1(BTN1_REPEAT, NULL, media_playlist_repeat_xpm);

	initButton2(BTN2_ADD, SLOT(slotAddFile()));
	initButton2(BTN2_REMOVE, SLOT(slotRemoveSong()));
	initButton2(BTN2_FLASH, SLOT(slotStoreFlash()));
	initButton2(BTN2_SYNCH_MP3, SLOT(slotSynch()));
	initButton2(BTN2_SYNCH_USB, SLOT(slotSynch()));
	
	minYearFilter.setMinimum(1970); minYearFilter.setMaximum(2020); minYearFilter.setValue(1970);
	maxYearFilter.setMinimum(1970); maxYearFilter.setMaximum(2020); maxYearFilter.setValue(QDate::currentDate().year());

	minRatingFilter.setMinimum(1); minRatingFilter.setMaximum(10); minRatingFilter.setValue(1);
	maxRatingFilter.setMinimum(1); maxRatingFilter.setMaximum(10); maxRatingFilter.setValue(10);
	playUnratedFilter.setChecked(true);

	/*
		TABLE WIDGET
		*/
	tableWidget.reloadTable();
	player.setFilterCount(tableWidget.rowCount());

	/*
		CONNECT SIGNALS TO SLOTS
		*/
	//connect(&tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(slotItemEdit(int, int)));

	connect(&tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(slotForward(int, int)));
	connect(&tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showPopupMenu(const QPoint&)));
	connect(&filter, SIGNAL(textChanged(const QString &)), this, SLOT(slotFilterChanged(const QString &)));
	connect(&volumeSlider, SIGNAL(valueChanged(int)), &player, SLOT(slotChangeVolume(int)));

	connect(&infoActionDownload, SIGNAL(triggered()), this, SLOT(slotInfoDownload()));

	connect(&player, SIGNAL(signalStatusChanged(STATUS_FLAGS)), this, SLOT(onSetStatus(STATUS_FLAGS)));
	connect(&player, SIGNAL(signalUpdatePlaytime(int)), &progressBar, SLOT(setValue(int)));
	connect(toolBox, SIGNAL(currentChanged(int)), this, SLOT(slotToolBoxChanged(int)));
	connect(&mainSplitter, SIGNAL(splitterMoved(int,int)), this, SLOT(slotSplitterMoved(int,int)));


	/*
		OTHER
		*/
	retranslateUi();

	// TODO: load this value from file:
	volumeSlider.setSliderPosition(0);
	onSetStatus(PlayerEngine::STATUS_STOPPED);
	progressBar.setValue(100);

	UpdateDlg::autoCheckForUpdates(); // do NOT update settings->last_start before this line!
	globals::settings->setValue("last_start", QDateTime::currentDateTime());

	quitProgram = false;
}

MainWindow::~MainWindow()
{
}

bool MainWindow::event(QEvent *event)
{
#if 0
	if (event->type() == QEvent::Close && ! quitProgram)
	{
		event->ignore();
		visible = false;
		setVisible(false);
		return true;
	}

	return QMainWindow::event(event);
#endif
	/*
		if it is a close-event, but no quit button was pressed,
		we hide in the tray icon iff we are the last window
	*/
	if (event->type() == QEvent::Close && ! quitProgram
		&& !mwContainer->removeFrom(this))
	{
		event->ignore();
	//	visible = false;
		setVisible(false);
		return true;
	}

	return QMainWindow::event(event);
}
