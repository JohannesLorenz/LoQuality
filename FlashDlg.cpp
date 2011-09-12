/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2011                                               */
/* Johannes Lorenz, Philipp Lorenz                                       */
/* https://github.com/DrSegfault/LoQuality                               */
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

void FlashDlg::retranslateUi()
{
	cbAddAfterwards.setText("Gleich einfuegen");
	storeButton.setText("Abspeichern");
	reloadButton.setText("Neu laden");
	doneButton.setText("Fertig!");
}

void FlashDlg::addToFileList(const QString& basedir)
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

void FlashDlg::reload() // TODO: we could do that more efficient :P
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

void FlashDlg::slotTimerTimeout()
{
	puts("TIMEOUT!");
	if( 0 != waitpid(ffmpegs_pid, NULL, WNOHANG) )  { // wait for ffmpeg to finish
		puts("WAIT FINISHED!");
		convertTimer.stop();
		progressDlg->cancel();
		progressDlg=NULL;
		if(cbAddAfterwards.isChecked()) {
			sqlhelper.start_insert_sequence();
			sqlhelper.INSERT(curOutName.toAscii().data());
			sqlhelper.stop_insert_sequence();
		}
	}
}

void FlashDlg::setupUi()
{
	resize(600, 425);

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
	buttonLayout.addWidget(&doneButton);

	topLayout.addLayout(&buttonLayout);

	convertTimer.setInterval(1000);
	QObject::connect(&convertTimer, SIGNAL(timeout()), this, SLOT(slotTimerTimeout()));

	connect(&fileList,SIGNAL(itemSelectionChanged ()),
		this, SLOT(selectionChanged()));
	connect(&storeButton,SIGNAL(clicked ()),
		this, SLOT(buttonStorePressed()));
	connect(&doneButton,SIGNAL(clicked ()),
		this, SLOT(close()));
	connect(&reloadButton,SIGNAL(clicked ()), this, SLOT(reload()));

	retranslateUi();

	QMessageBox::warning(this, "Warning!!!", "The auther does not encourage you to:\n\n"
		"<b>use LoQuality for <u>illegal</u> downloads!!!</b>", "Yes", "Yes");
}

void FlashDlg::buttonStorePressed()
{
	QList<QListWidgetItem*> items = fileList.selectedItems();
	for(QList<QListWidgetItem*>::const_iterator itr = items.begin();
		itr != items.end(); itr++)
	{
		// ask for uptput file name
		do {
			curOutName = QFileDialog::getSaveFileName(this, QString("Bitte Speicherort fuer OGG ueberlegen... (%1)").arg((*itr)->text()), globals::MUSIC_ROOT, "*.ogg");
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

		// fork ffmpeg
		ffmpegs_pid=fork();
		if(ffmpegs_pid < 0) {
			QMessageBox::information(NULL, "Sorry...", "... fork() ging leider nicht, kann ffmpeg nicht starten :(");
			return;
		}
		else if(ffmpegs_pid == 0) {
			// ffmpeg -i flashfile -vn -acodec libmp3lame -y -ar 44100 -ab 128000 outfilename
			//execlp("ffmpeg", "ffmpeg", "-i", strchr((*itr)->text().toAscii().data(), ':') + 2, "-vn", "-acodec", "libmp3lame", "-y", "-ar", "44100", "-ab", "128000", curOutName.toAscii().data(), NULL);

			// note: you might want to specify number of threads with "-threads n"
			// ffmpeg -i flashfile -vn -y -ar 44100 -aq 4 -acodec libvorbis -threads THREADNUM outfile
			const QString CPU_THREADS = globals::settings->value("number_of_cores",2).toString();
			const QString ffmpeg_fullpath = globals::settings->value("ffmpeg_fullpath").toString();
			//printf("%s -i %s ... -threads %s %s",,,CPU_THREADS, curOutName.toAscii().data())
			execlp(ffmpeg_fullpath.toAscii().data(), "ffmpeg", "-i", strchr((*itr)->text().toAscii().data(), ':') + 2, "-vn", "-y", "-ar", "44100", "-aq", "4", "-acodec", "libvorbis", "-threads", CPU_THREADS.toAscii().data(), curOutName.toAscii().data(), NULL);

			exit(0);
		}

		if(progressDlg == NULL)
		{
			progressDlg = new QProgressDialog("Konvertiere Daten mit ffmpeg...", "Abbrechen", 0, 0, this);
			progressDlg->setWindowModality(Qt::WindowModal);
			progressDlg->setValue(0);
			progressDlg->show();
			convertTimer.start();
		}

		/*int i=0;
		//int stat_val;
		while( 0 == waitpid(ffmpegs_pid, NULL, WNOHANG) )  { // wait for ffmpeg to finish
			progressDlg.repaint();
			sleep(1);
		}*/


		//waitpid(ffmpegs_pid, NULL, WCONTINUED);

		//waitpid(ffmpegs_pid, NULL, WCONTINUED);

		(*itr)->setSelected(false);
		(*itr)->setFlags((*itr)->flags() ^ Qt::ItemIsEnabled );
	}

}

void FlashDlg::selectionChanged (  )
{
	storeButton.setEnabled(fileList.selectedItems().size() > 0);
}
