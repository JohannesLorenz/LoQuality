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

#include <QDir>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>

#include "globals.h"
#include "FlashDlg.h"
#include "SqlHelper.h"

#define FIREFOX_NEW

void SelectFlashPage::retranslateUi()
{
	setTitle("Flash videos from Firefox");
	cbAddAfterwards.setText("Insert into my sql database");
	storeButton.setText("Save");
	reloadButton.setText("Reload");
//	doneButton.setText("Fertig!");
}

void SelectFlashPage::addToFileList(const QString& basedir)
{
#ifdef FIREFOX_NEW
	QDir tmpDir(basedir, /*"Flash*",*/"", QDir::Time, QDir::Files);
	QFileInfoList files = tmpDir.entryInfoList();
	QDateTime time_now = QDateTime::currentDateTime ();
	int row = 0;
	for(QList<QFileInfo>::const_iterator itr = files.begin(); itr != files.end(); itr++, row++)
	{
		if(! itr->canonicalFilePath().contains("Flash"))
		 continue;
		QDateTime lastChanged = itr->lastModified();
		if( lastChanged.daysTo(time_now) == 0 )
			if( lastChanged.secsTo(time_now) <= 3600 ) { // TODO: make 3600 flexible
				int minutesLeft = (int) lastChanged.secsTo(time_now) / 60;
				if( minutesLeft < 1 ) {
					fileList.addItem(
					QString("Still loading: %1")
					.arg(itr->absoluteFilePath())
					);
					puts("a");
					fileList.item(row)->setFlags(fileList.item(row)->flags() ^ Qt::ItemIsEnabled);
					puts("c");
				}
				else
					fileList.addItem(
					QString("%1 Minutes ago: %2")
					.arg(minutesLeft)
					.arg(itr->absoluteFilePath())
					);
			}
	}
#endif
}

void SelectFlashPage::reload() // TODO: we could do that more efficient :P
{
	fileList.clear();
#ifndef FIREFOX_NEW
	QDir tmpDir("/tmp", "Flash*", QDir::Time, QDir::Files);
	QFileInfoList files = tmpDir.entryInfoList();
	QDateTime time_now = QDateTime::currentDateTime ();
	int row = 0;
	for(QList<QFileInfo>::const_iterator itr = files.begin(); itr != files.end(); itr++, row++)
	{
		QDateTime lastChanged = itr->lastModified();
		if( lastChanged.daysTo(time_now) == 0 )
			if( lastChanged.secsTo(time_now) <= 3600 ) {// TODO: make 3600 flexible
				int minutesLeft = (int) lastChanged.secsTo(time_now) / 60;
				if( minutesLeft < 1 ) {
					fileList.addItem(
					QString("Still loading: %1")
					.arg(itr->absoluteFilePath())
					);
					fileList.item(row)->setFlags(fileList.item(row)->flags() ^ Qt::ItemIsEnabled);
				}
				else
					fileList.addItem(
					QString("%1 Minutes ago: %2")
					.arg(minutesLeft)
					.arg(itr->absoluteFilePath())
					);
			}
	}
#else
	QDir dir("/proc/");
	QFileInfoList list = dir.entryInfoList();
	for (int iList=0;iList<list.count();iList++)
	{
		QFileInfo info = list[iList];
		QString sFilePath = info.filePath();
		if (info.isDir())
		{
			if (info.fileName()!=".." && info.fileName()!=".")
			{
				QFile fp(sFilePath+"/cmdline");
				if( fp.open(QIODevice::ReadOnly))
				{
					QByteArray line = fp.readLine();
					fp.close();
					if(line.contains("flashplayer"))
					 addToFileList(sFilePath+"/fd");
				}
			}
		}
	}

#endif
}

void StoreHelper::slotTimerTimeout()
{
	puts("TIMEOUT!");
	if( 0 != waitpid(ffmpegs_pid, NULL, WNOHANG) )  { // wait for ffmpeg to finish
		puts("WAIT FINISHED!");
		convertTimer.stop();
		progressDlg->cancel();
		progressDlg=NULL;
		if(insertSql) { // cbAddAfterwards.isChecked()
			sqlhelper.start_insert_sequence();
			sqlhelper.INSERT(curOutName.toAscii().data());
			sqlhelper.stop_insert_sequence();
		}
	}
}

void FlashDlg::setupUi()
{
	setPage(PAGE_INTRO, &introPage);
	setPage(PAGE_DECIDE, &decidePage);
	setPage(PAGE_SELECT_FLASH, &selectFlashPage);
	setPage(PAGE_SELECT_FILES, &selectFilesPage);
}

void SelectFlashPage::buttonStorePressed()
{
	QList<QListWidgetItem*> items = fileList.selectedItems();
	for(QList<QListWidgetItem*>::const_iterator itr = items.begin();
		itr != items.end(); itr++)
	{
		storeHelper.askForOutputFilename((*itr)->text());
		storeHelper.convertToOgg(this, strchr((*itr)->text().toAscii().data(), ':') + 2,
			cbAddAfterwards.isChecked());

		(*itr)->setSelected(false);
		(*itr)->setFlags((*itr)->flags() ^ Qt::ItemIsEnabled );
	}
}

void SelectFlashPage::selectionChanged (  ) {
	storeButton.setEnabled(fileList.selectedItems().size() > 0);
}

void SelectFlashPage::setupUi()
{
//	resize(600, 425);
	setLayout(&topLayout);

	/*
		Calculate File List...
	*/
	reload();

	/*
		Setup UI...
	*/
	fileList.setSelectionMode(QAbstractItemView::MultiSelection);
	topLayout.addWidget(&fileList);

	cbAddAfterwards.setChecked(true);
	storeButton.setEnabled(false);
	buttonLayout.addWidget(&cbAddAfterwards);
	buttonLayout.addWidget(&storeButton);
	buttonLayout.addWidget(&reloadButton);
	//buttonLayout.addWidget(&doneButton);

	topLayout.addLayout(&buttonLayout);

	connect(&fileList,SIGNAL(itemSelectionChanged ()),
		this, SLOT(selectionChanged()));
	connect(&storeButton,SIGNAL(clicked ()),
		this, SLOT(buttonStorePressed()));
	/*connect(&doneButton,SIGNAL(clicked ()),
		this, SLOT(close()));*/
	connect(&reloadButton,SIGNAL(clicked ()), this, SLOT(reload()));

	retranslateUi();
}

void StoreHelper::askForOutputFilename(const QString& infile)
{
	// ask for output file name
	do {
		curOutName = QFileDialog::getSaveFileName(NULL, QString("Bitte Speicherort fuer OGG ueberlegen... (%1)").arg(infile), globals::MUSIC_ROOT, "*.ogg");
		if(curOutName.isEmpty())
		 break;
		if( 0 != strncmp(curOutName.toAscii().data(),globals::MUSIC_ROOT.toAscii().data(), globals::MUSIC_ROOT.length())) {
			QMessageBox::information(NULL, "Sorry...",QString("Dateien sollen nur in Verzeichnissen ab %1 gespeichert werden!").arg(globals::MUSIC_ROOT));
			continue;
		}
		break;
	} while(true);

	if(curOutName.isEmpty())
	 return;
	if(!curOutName.endsWith(".ogg"))
		curOutName += ".ogg";
}

bool StoreHelper::convertToOgg(QWidget* parent, const char *infile, bool _insertSql)
{
	insertSql = _insertSql;

	// fork ffmpeg
	ffmpegs_pid=fork();
	if(ffmpegs_pid < 0) {
		QMessageBox::information(parent, "Sorry...", "... fork() ging leider nicht, kann ffmpeg nicht starten :(");
		return false;
	}
	else if(ffmpegs_pid == 0) {
		// ffmpeg -i flashfile -vn -acodec libmp3lame -y -ar 44100 -ab 128000 outfilename
		//execlp("ffmpeg", "ffmpeg", "-i", strchr((*itr)->text().toAscii().data(), ':') + 2, "-vn", "-acodec", "libmp3lame", "-y", "-ar", "44100", "-ab", "128000", curOutName.toAscii().data(), NULL);

		/*
			We need at most audio quality 6 = approx. 128 kbps. See:
			http://en.wikipedia.org/wiki/YouTube#Quality_and_codecs
			http://en.wikipedia.org/wiki/Vorbis
		*/
		// note: you might want to specify number of threads with "-threads n"
		// ffmpeg -i flashfile -vn -y -ar 44100 -aq 6 -acodec libvorbis -threads THREADNUM outfile
		const QString CPU_THREADS = globals::settings->value("number_of_cores",2).toString();
		const QString ffmpeg_fullpath = globals::settings->value("ffmpeg_fullpath").toString();
		//printf("%s -i %s ... -threads %s %s",,,CPU_THREADS, curOutName.toAscii().data())
		execlp(ffmpeg_fullpath.toAscii().data(), "ffmpeg", "-i", infile, "-vn", "-y", "-ar", "44100", "-aq", "6", "-acodec", "libvorbis", "-threads", CPU_THREADS.toAscii().data(), curOutName.toAscii().data(), NULL);

		exit(0);
	}

	if(progressDlg == NULL)
	{
		progressDlg = new QProgressDialog("Konvertiere Daten mit ffmpeg...", "Abbrechen", 0, 0, parent);
		progressDlg->setWindowModality(Qt::WindowModal);
		progressDlg->setValue(0);
		progressDlg->show();
		convertTimer.start();
	}

	downloadsMade = true;
	return true;
}

void SelectFilesPage::setupUi()
{
	setLayout(&topLayout);

	fileName.setReadOnly(true);
	cbAddAfterwards.setChecked(true);
	storeButton.setDisabled(true);

	topLayout.addWidget(&fileName);
	topLayout.addWidget(&selectButton);
	topLayout.addWidget(&storeButton);
	topLayout.addWidget(&cbAddAfterwards);

	connect(&selectButton,SIGNAL(clicked ()), this, SLOT(selectFiles()));
	connect(&storeButton,SIGNAL(clicked ()), this, SLOT(buttonStorePressed()));
}

void SelectFilesPage::buttonStorePressed()
{
	storeHelper.askForOutputFilename(fileName.text());
	storeHelper.convertToOgg(this, fileName.text().toAscii().data(), cbAddAfterwards.isChecked());
}

void SelectFilesPage::selectFiles()
{
	fileName.setText(QFileDialog::getOpenFileName(NULL, "HDD video in flash or mp4 format"));
	storeButton.setDisabled(fileName.text().isEmpty());
}

void SelectFilesPage::retranslateUi()
{
	setTitle("Flash Files from HDD");
	setSubTitle("Please Choose a song and then convert it.");
	cbAddAfterwards.setText("Insert into my sql database");
	selectButton.setText("Select Input file (flv or mp4)");
	storeButton.setText("Convert File");
}
