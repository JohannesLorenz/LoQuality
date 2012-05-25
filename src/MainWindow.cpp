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
#include <QDateTime>
#include <QProgressDialog>
#include <QBitArray>
#include <QSqlError> // TODO: remove me later
/*#include <QDebug>

#include <QString>
#include <QSizePolicy>
#include <QFileDialog>
#include <QTextBrowser>
*/

#include "MainWindowIcons.h"
// TODO: get new icons from http://websvn.kde.org/trunk/kdesupport/oxygen-icons/

#include "MainWindow.h"
#include "FlashDlg.h"
#include "SynchWizard.h"
#include "FileManager.h"

#define _MPLAYER_REMOTE_PIPE "/tmp/lq_remote_pipe"

void MainWindow::retranslateUi() {

	topMenus[MENU_FILE].setTitle(tr("Datei"));
	topMenus[MENU_EDIT].setTitle(tr("Bearbeiten"));
	topMenus[MENU_VIEW].setTitle(tr("Ansicht"));
	topMenus[MENU_HELP].setTitle(tr("Hilfe"));

	Actions[ACTION_FILE_UPDATE]->setText(tr("Update"));
	Actions[ACTION_FILE_QUIT]->setText(tr("Beenden"));
	Actions[ACTION_VIEW_SCROLL_TO_SONG]->setText(tr("Scroll zum aktuellen Song"));
	Actions[ACTION_VIEW_FULLSCREEN]->setText(tr("Vollbild"));
	Actions[ACTION_VIEW_SWITCH_ALIGN]->setText(tr("Ausrichtung ändern"));
	Actions[ACTION_EDIT_EDITTITLE]->setText(tr("Titel-Info bearbeiten..."));
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

	infoActionDownload.setText("Falls ein Song laeuft kann man hier ein Cover laden...");
	//x1Filter.setText("Person 1");
	//x2Filter.setText("Person 2");
}
	
QAction* MainWindow::initAction (enum MENU menuNo, enum ACTION actionNo, const char* slotName,
					const QKeySequence& shortKeySequence, const char* const xpmStr[]) {
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

void MainWindow::onSetStatus(STATUS_FLAGS new_status) {
	switch(new_status)
	{
		case PlayerEngine::STATUS_PLAYING:
			buttons1[BTN1_PLAY].setDisabled(true);
			buttons1[BTN1_STOP].setEnabled(true);
			buttons1[BTN1_PAUSE].setEnabled(true);
			if(toolBox->currentIndex() == TOOLBOX_METAINFO) {
				slotToolBoxChanged(TOOLBOX_METAINFO);
			}
			break;
		case PlayerEngine::STATUS_SONGOVER:
			if(time_to_stop == 0 || time_to_stop > time(NULL))
			 slotForward();
			else
			 slotStop();
			break;
		case PlayerEngine::STATUS_STOPPED:
			buttons1[BTN1_STOP].setDisabled(true);
		case PlayerEngine::STATUS_PAUSED:
			buttons1[BTN1_PLAY].setEnabled(true);
			buttons1[BTN1_PAUSE].setDisabled(true);
	}
}

/*
	BUTTONS SLOTS
*/
void MainWindow::slotPlay(int row, int column) {

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

void MainWindow::slotForward(int row, int column) {

	Q_UNUSED(column);
	//QList<QTableWidgetItem*> items = tableWidget.selectedItems();
	//player.slotForward(buttons1[BTN1_RANDOM].isChecked(), NULL);
	const bool doubleClicked = (row!=-1);
	player.slotForward(buttons1[BTN1_RANDOM].isChecked(), doubleClicked?tableWidget.item(row,13):NULL, doubleClicked);
}

void MainWindow::slotBackward() {
	player.slotBackward();
}

void MainWindow::slotAddFile()
{
	FileManager fileManager(sqlhelper);
	fileManager.show();
	fileManager.exec();
	if(fileManager.isAnythingChanged())
	 reloadTable();
#if 0 // TODO: move this to the flashdlg
	AddEntryDlg dlg(sqlhelper);
	dlg.show();
	dlg.exec();
#endif
}

void MainWindow::slotRemoveSong()
{
#if 0 // TODO: re-implement me if QTableWidget or QTreeWidget is done
	if( tableWidget.selectedItems().size() < 1 )
		QMessageBox::information(NULL, "Kein Song ausgewaehlt...", "Du musst erst noch einen Song auswaehlen, um ihn zu loeschen...");
	else if( QMessageBox::Yes == QMessageBox::question(NULL, "Wirklich loeschen?", "Willst Du (TODO) wirklich aus der Wiedergabelist loeschen?",
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No) )
	 tableWidget.removeItemWidget(tableWidget.selectedItems().front());
#endif

	QList<QTableWidgetItem*> selectedItems = tableWidget.selectedItems();
	// count number of selected rows
	unsigned int selectedRows;
	{
		QList<unsigned int> selectedRowList;
		for(QList<QTableWidgetItem*>::const_iterator itr = selectedItems.begin(); itr!=selectedItems.end(); itr++)
		 if(! selectedRowList.contains((*itr)->row()))
		  selectedRowList.append((*itr)->row());
		selectedRows = selectedRowList.size();
	}

	if(selectedRows < 1 ) {
		QMessageBox::information(NULL, "Kein Song ausgewaehlt...", "Du musst erst noch einen Song auswaehlen, um ihn zu loeschen...");
		return;
	}	
	else if(selectedRows == 1) {
		QString songTitle = tableWidget.item( (*selectedItems.begin())->row(), 1)->text();
		if(songTitle.isEmpty())
		 songTitle = "diesen Titel";
		if( QMessageBox::No == QMessageBox::question(NULL, "Wirklich loeschen?", QString("Willst Du <i>%1</i> wirklich aus der Wiedergabeliste loeschen?").arg( songTitle ),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No) )
			return;
	}
	else { // multiples...
		if( QMessageBox::No == QMessageBox::question(NULL, "Wirklich loeschen?", QString("Willst Du diese <b>%1 Songs</b> wirklich aus der Wiedergabeliste loeschen?").arg( selectedRows ),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No) )
			return;
	}

	for(QList<QTableWidgetItem*>::const_iterator itr = selectedItems.begin(); itr != selectedItems.end(); itr++) {
		if( (*itr)->column() == 1) {
			sqlhelper.DELETE( row2id((*itr)->row()) );
			tableWidget.removeRow((*itr)->row());
		}
	}
}

void MainWindow::slotStoreFlash()
{
	FlashDlg fdlg(sqlhelper);
	fdlg.show();
	if( fdlg.exec() == QDialog::Accepted )
	 reloadTable();
}

void MainWindow::slotSynch()
{
	SynchWizard synch_wizard(sqlhelper);
	synch_wizard.show();
	if( synch_wizard.exec() == QDialog::Accepted )
	 reloadTable();
}

/*
	MENU BAR SLOTS
*/
void MainWindow::slotFileQuitAction () {
	quitProgram = true;
	close();
}

void MainWindow::slotHelpAboutAction () {
	QMessageBox::about ( NULL, "About - LoQuality",
				"<h1>LoQuality</h1>"
				"<i>(c) 2010-2012</i><br/>"
				"by Philipp Lorenz, Johannes Lorenz<br/><br/>"
				"<a href=\"https://github.com/JohannesLorenz/LoQuality\">https://github.com/JohannesLorenz/LoQuality</a>");
				
}

void MainWindow::slotHelpAboutQtAction () {
	QMessageBox::aboutQt ( NULL, tr("About - Qt") );
}

void MainWindow::slotFilterChanged ( const QString & text ) {
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

void MainWindow::slotToolBoxChanged(int newIndex)
{
	if(newIndex == TOOLBOX_METAINFO)
	{
		if(player.getCurSongItem())
		{
			QDir currentDir(QFileInfo(player.getCurSongItem()->text()).dir());
			QStringList imgs = currentDir.entryList(QStringList() << "*.jpg" << "*.png" << "*.xpm" << "*.gif", QDir::Files);
			printf("dir: %s, imgs: %d\n",player.getCurSongItem()->text().toAscii().data(), imgs.size() );
			if(imgs.empty())
			{
				tmpImage.load("media/graphics/lq.png");
				imageLabel.setPixmap(QPixmap::fromImage(tmpImage.scaledToHeight(300)));

				//imageLabel.clear();
			}
			else
			{
				int imgNumber = (int) (imgs.size()*1.0*rand()/(RAND_MAX+1.0));
				printf("chosen: %s\n",imgs.at(imgNumber).toAscii().data());
				tmpImage.load(currentDir.absoluteFilePath(imgs.at(imgNumber)));
				imageLabel.setPixmap(QPixmap::fromImage(tmpImage.scaledToHeight(300)));
				/*imageLabel.adjustSize();
				imageLabel.show();
				toolBox->update();
				informationBox.update();*/

			}
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

void MainWindow::reloadTable()
{
	tableWidget.setSortingEnabled(false); // no sorting during insertion - it takes too long

	tableWidget.clear();
	tableWidget.setRowCount(0);

	tableWidget.setColumnCount(15);

	QStringList description;
	
	description.insert(0, "ID");
	description.insert(1, "Titel");
	description.insert(2, "Künstler");
	description.insert(3, "Album");
	description.insert(4, "Tags");
	description.insert(5, "Genre");
	description.insert(6, "Jahr");
	description.insert(7, "Interessiert Dich?");
	description.insert(8, "Interessiert ...?");
	description.insert(9, "Typ");
	description.insert(10, "Qualität");
	description.insert(11, "Deine Bewertung");
	description.insert(12, "Bewertung von ...");
	description.insert(13, "Pfad");
	description.insert(14, "Last Changes");
	
	tableWidget.setHorizontalHeaderLabels(description);
	
	QSqlQuery query;
	query.exec("SELECT * FROM main;");
		printf("size: %d\n", query.size());
		printf("last error: %s\n", query.lastError().text().toAscii().data());

//	tableWidget.setRowCount(query.size());

	QProgressDialog progressDlg("Lade Musik-Datenbank...", "Abbrechen", 0, query.size(), this);
	progressDlg.setWindowModality(Qt::WindowModal);

	unsigned int rowcount=0;

	while (query.next()) { // WARNING: DO !!!NEVER!!! rely on query.size() here!!! (see qt docs...)

		progressDlg.setValue(rowcount);
		progressDlg.show();
		if (progressDlg.wasCanceled())
		 break;

		if(rowcount % 250 == 0)
		 tableWidget.setRowCount(rowcount+250);
	
		QString id = query.value(0).toString();
		QString title = query.value(1).toString();
		QString artist = query.value(2).toString();
		QString album = query.value(3).toString();
		QString tags = query.value(4).toString();
		QString genre = query.value(5).toString();
		QString year = query.value(6).toString();
		QString int_you = query.value(7).toString(); // int = interest
		QString int_other = query.value(8).toString();
		QString type = query.value(9).toString();
		QString quality = query.value(10).toString();
		QString vote_you = query.value(11).toString();
		QString vote_other = query.value(12).toString();
		QString path = query.value(13).toString();

		QDateTime _last_change = QDateTime::fromTime_t(query.value(14).toInt());
		QString last_change = (_last_change.isValid() && _last_change <= QDateTime::currentDateTime())?
			_last_change.toString("yyyy-MM-dd") : "(incorrect)";
		
		//QString &artistRef = artistAlbumList[artist];
		//if(! artistRef.contains(artist) )
		// artistRef.push_back(album);

		QTableWidgetItem* item_id = new QTableWidgetItem(id);
		QTableWidgetItem* item_title = new QTableWidgetItem(title);
		QTableWidgetItem* item_artist = new QTableWidgetItem(artist);
		QTableWidgetItem* item_album = new QTableWidgetItem(album);
		QTableWidgetItem* item_tags = new QTableWidgetItem(tags);
		QTableWidgetItem* item_genre = new QTableWidgetItem(genre);
		QTableWidgetItem* item_year = new QTableWidgetItem(year);
		QTableWidgetItem* item_int_other = new QTableWidgetItem(int_other);
		QTableWidgetItem* item_int_you = new QTableWidgetItem(int_you);
		QTableWidgetItem* item_type = new QTableWidgetItem(type);
		QTableWidgetItem* item_quality = new QTableWidgetItem(quality);
		QTableWidgetItem* item_vote_you = new QTableWidgetItem(vote_you);
		QTableWidgetItem* item_vote_other = new QTableWidgetItem(vote_other);
		QTableWidgetItem* item_path = new QTableWidgetItem(path);
		QTableWidgetItem* item_last_change = new QTableWidgetItem(last_change);
		
		item_id->setData(Qt::DisplayRole, id);
		item_title->setData(Qt::DisplayRole, title);
		item_artist->setData(Qt::DisplayRole, artist);
		item_album->setData(Qt::DisplayRole, album);
		item_tags->setData(Qt::DisplayRole, tags);
		item_genre->setData(Qt::DisplayRole, genre);
		item_year->setData(Qt::DisplayRole, year);
		item_int_other->setData(Qt::DisplayRole, int_other);
		item_int_you->setData(Qt::DisplayRole, int_you);
		item_type->setData(Qt::DisplayRole, type);
		item_quality->setData(Qt::DisplayRole, quality);
		item_vote_you->setData(Qt::DisplayRole, vote_you);
		item_vote_other->setData(Qt::DisplayRole, vote_other);
		item_path->setData(Qt::DisplayRole, path);
		item_last_change->setData(Qt::DisplayRole, last_change);
		
		tableWidget.setItem(rowcount, 0, item_id);
		tableWidget.setItem(rowcount, 1, item_title);
		tableWidget.setItem(rowcount, 2, item_artist);
		tableWidget.setItem(rowcount, 3, item_album);
		tableWidget.setItem(rowcount, 4, item_tags);
		tableWidget.setItem(rowcount, 5, item_genre);
		tableWidget.setItem(rowcount, 6, item_year);
		tableWidget.setItem(rowcount, 7, item_int_other);
		tableWidget.setItem(rowcount, 8, item_int_you);
		tableWidget.setItem(rowcount, 9, item_type);
		tableWidget.setItem(rowcount, 10, item_quality);
		tableWidget.setItem(rowcount, 11, item_vote_you);
		tableWidget.setItem(rowcount, 12, item_vote_other);
		tableWidget.setItem(rowcount, 13, item_path);
		tableWidget.setItem(rowcount, 14, item_last_change);
		
		++rowcount;
	}
	tableWidget.setRowCount(rowcount);

	tableWidget.hideColumn(0);
	tableWidget.hideColumn(7);
	tableWidget.hideColumn(8);
	tableWidget.hideColumn(12);
	tableWidget.hideColumn(13);
	
	tableWidget.resizeColumnsToContents ();
	tableWidget.resizeRowsToContents ();

	tableWidget.setSortingEnabled(true);
	tableWidget.sortByColumn(14, Qt::AscendingOrder);
}

void MainWindow::switch_tray(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
		case QSystemTrayIcon::Trigger:
		case QSystemTrayIcon::DoubleClick:
		{
			visible ^= 1;
			setVisible(visible);
			break;
		}
		case QSystemTrayIcon::MiddleClick:
		{
			QMessageBox::information(NULL, "Oh dude", "*head ---> table* :P wrong key ^^");
			break;
		}
		default:
		{
/*			int limit = link_list.size();

			for(int count=0; count < limit; ++count)
			{
				if(count > 9)
				 break;

				cout << "Setting IP no. " << count << " as '" << link_list[count].c_str() << "'" << endl;

				launchlinks[count]->setText(link_list[count].c_str());

				cout << "SET!" << endl;

				popup.addAction(launchlinks[count]);

				cout << "ADDED!" << endl;
			}

			popup.addSeparator();
			popup.addAction(&act_exit);

			popup.exec(QCursor::pos());

			break;*/
		}
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

		informationBox.addWidget(&imageLabel);
		//infoOptionsMenu.append();

		optionsButton.setText("...");
		optionsButton.setMenu(&infoOptionsMenu);
		infoOptionsMenu.addAction(&infoActionDownload);
		informationBox.addWidget(&optionsButton);

		newWidget = new QWidget(toolBox);
		newWidget->setLayout(&informationBox);
		toolBox->addItem(newWidget, "Information");

		toolBox->setCurrentIndex(1);

		verticalLayout.addWidget(&mainSplitter);
		mainSplitter.addWidget(toolBox);
		mainSplitter.addWidget(&tableWidget);
	}

	/*
		STATUS BAR
		*/
	setStatusBar(&statusBar);

	/*
		OTHER
		*/
	QIcon* tray_icon_icon = new QIcon("media/graphics/lq.png");
	tray_icon.setIcon(*tray_icon_icon);
	tray_icon.setVisible(true);
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


	/*
		OTHER
		*/
	//QIcon* tray_icon_icon = new QIcon("media/graphics/lq.png");
}

MainWindow::MainWindow (const bool mobile, QWidget* parent)
	:
	QMainWindow(parent),

	// settings - the first thing to do!
	settingsReader(),

	// non gui
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

	mainSplitter(Qt::Vertical, (mobile)?(NULL):&centralWidget),

	toolBox(new QToolBox(&mainSplitter)),
	infoActionDownload(this),

	//table widget
	tableWidget((mobile)?(QWidget*)(NULL):(QWidget*)(&mainSplitter)),

	// others
	popupMenu(&tableWidget),
	visible(true),
	time_to_stop(0),
	dbus_connector(&player),

	// status bar
	statusBar(this)

{
	layoutWidgets(mobile);
	/*
		MENU BAR
		*/
	Actions.resize(ACTION_SIZE);

	initAction(MENU_FILE, ACTION_FILE_UPDATE, SLOT(slotFileUpdateAction()), QKeySequence(Qt::CTRL + Qt::Key_U));
	initAction(MENU_FILE, ACTION_FILE_QUIT, SLOT(slotFileQuitAction()), QKeySequence(Qt::CTRL + Qt::Key_Q), application_exit_xpm);

	initAction(MENU_EDIT, ACTION_EDIT_EDITTITLE, SLOT(slotItemEdit()), QKeySequence(Qt::CTRL + Qt::Key_E));
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
		TABLE WIDGET (WILL ALSO INITIALIZE DATABASE)
		*/

//#define USE_MYSQL
#ifdef USE_MYSQL
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setUserName("music");
	db.setPassword("opensource");
	db.setHostName("localhost");
	db.setDatabaseName("musicdb");
#else
	db = QSqlDatabase::addDatabase("QSQLITE");
	QString path;
	#ifdef Q_OS_LINUX
		// NOTE: We have to store database file into user home folder in Linux
//		path.append(QDir::home().path());
//		path.append(QDir::separator()).append("musicdb");
//		path = QDir::toNativeSeparators(path);
//		db.setDatabaseName(path);
		path.append("musicdb");
	#else
		// NOTE: File exists in the application private folder, in Symbian Qt implementation
		path.append("musicdb");
	#endif

	db.setDatabaseName(path);
#endif


	const bool ok = db.open();
	
	if(ok)
	 printf("CONNECTION TO DATABASE ESTABLISHED!!\n");
	else {
		printf("COULD NOT OPEN OR CREATE DATABASE!\n");
		return;
	}
	

#ifndef USE_MYSQL
	// might need to create sqlite table...

	QStringList tables = db.tables();
	printf("tables: %d\n",tables.size());


	bool main_exists = false;

	QSqlQuery query;

	query.exec("SELECT name FROM  sqlite_master WHERE type='table' ORDER BY name;");
	printf("size: %d\n", query.size());
	//printf("last error: %s\n", query.lastError().text().toAscii().data());
	while (query.next() && !main_exists) {
		printf("tables: %s\n",query.value(0).toString().toAscii().data());
		if( query.value(0).toString() == "main" )
			main_exists = true;
	}

	if(!main_exists)
	 sqlhelper.CREATE();

	printf("Main exists? %d\n",(int)main_exists);
#endif

	tableWidget.setSortingEnabled(true);
	reloadTable();
	player.setFilterCount(tableWidget.rowCount());

	/*
		CONNECT SIGNALS TO SLOTS
		*/
	tableWidget.sortByColumn(14, Qt::AscendingOrder);
	tableWidget.setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableWidget.setContextMenuPolicy(Qt::CustomContextMenu);
	tableWidget.setSelectionBehavior(QAbstractItemView::SelectRows);
	//connect(&tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(slotItemEdit(int, int)));

	connect(&tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(slotForward(int, int)));
	connect(&tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showPopupMenu(const QPoint&)));
	connect(&filter, SIGNAL(textChanged(const QString &)), this, SLOT(slotFilterChanged(const QString &)));
	connect(&volumeSlider, SIGNAL(valueChanged(int)), &player, SLOT(slotChangeVolume(int)));

	connect(&infoActionDownload, SIGNAL(triggered()), this, SLOT(slotInfoDownload()));

	connect(&player, SIGNAL(signalStatusChanged(STATUS_FLAGS)), this, SLOT(onSetStatus(STATUS_FLAGS)));
	connect(&player, SIGNAL(signalUpdatePlaytime(int)), &progressBar, SLOT(setValue(int)));
	connect(toolBox, SIGNAL(currentChanged(int)), this, SLOT(slotToolBoxChanged(int)));
	connect(&tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(switch_tray(QSystemTrayIcon::ActivationReason)));
	connect(&mainSplitter, SIGNAL(splitterMoved(int,int)), this, SLOT(slotSplitterMoved(int,int)));



	/*
		OTHER
		*/
	retranslateUi();

	// TODO: load this value from file:
	volumeSlider.setSliderPosition(0);
	onSetStatus(PlayerEngine::STATUS_STOPPED);
	progressBar.setValue(100);

	dbus_connector.start();

	UpdateDlg::autoCheckForUpdates(); // do NOT update settings->last_start before this line!
	globals::settings->setValue("last_start", QDateTime::currentDateTime());

	quitProgram = false;
}

MainWindow::~MainWindow()
{
	//close the database
	db.close();
}

bool MainWindow::event(QEvent *event)
{
	if (event->type() == QEvent::Close && ! quitProgram)
	{
		event->ignore();
		visible = false;
		setVisible(false);
		return true;
	}

	return QMainWindow::event(event);
}
