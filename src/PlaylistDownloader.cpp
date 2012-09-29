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

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QUrl>

#include "globals.h"
#include "flash_tools.h"
#include "SongTableWidget.h"
#include "PlaylistDownloader.h"
#include "SqlHelper.h"

PlaylistDownloader::PlaylistDownloader(SongTableWidget *_table, const SqlHelper& _sqlhelper, QWidget *parent) :
	QDialog(parent),
	boxMain(this),
	cbEnumerate(),
	table(_table),
	progressDlg(NULL),
	sqlhelper(_sqlhelper),
	TMP_DIRECTORY("lq-youtube-dl")
{
	setupUi();
	retranslateUi();
}

void PlaylistDownloader::retranslateUi(void)
{
	cbEnumerate.setText("Enumerate Filenames");
	cbOverwrite.setText("Overwrite Files");
	lblFormat.setText("File Format");
	lblFolder.setText("Destination Folder");
	btnFolder.setText("Choose");
	okBtn.setText("Download");
	abortBtn.setText("Done");
}

void PlaylistDownloader::setupUi(void)
{
	cbFormat.addItem("best");
	cbFormat.addItem("aac");
	cbFormat.addItem("vorbis");
	cbFormat.addItem("mp3");
	cbFormat.addItem("m4a");
	cbFormat.addItem("wav");

	cbEnumerate.setChecked(true);
	cbOverwrite.setChecked(true);

	boxFormat.addWidget(&lblFormat);
	boxFormat.addWidget(&cbFormat);

	boxFolder.addWidget(&lblFolder);
	boxFolder.addWidget(&leFolder);
	boxFolder.addWidget(&btnFolder);

	boxButtons.addWidget(&abortBtn);
	boxButtons.addWidget(&okBtn);

	boxMain.addWidget(&cbEnumerate);
	boxMain.addWidget(&cbOverwrite);
	boxMain.addLayout(&boxFormat);
	boxMain.addLayout(&boxFolder);
	boxMain.addLayout(&boxButtons);

	QObject::connect(&btnFolder, SIGNAL(clicked()), this, SLOT(btnFolderClicked()));
	QObject::connect(&okBtn, SIGNAL(clicked()), this, SLOT(slotBtnOk()));
	QObject::connect(&abortBtn, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(&youtubedl, SIGNAL(finished()), this, SLOT(slotTimerTimeout()));
	QObject::connect(&wget, SIGNAL(finished()), this, SLOT(slotTimerTimeout()));
	//QObject::connect(&httpget, SIGNAL(done(bool)), this, SLOT(slotTimerTimeout()));
}

bool PlaylistDownloader::shallDownloadFile(int row) {
	//printf("File %s exists? %d\n",table->item(row, )->text().toAscii().data(), QFile::exists(table->item(row, )->text()));
	return( !QFile::exists(table->item(row, 13)->text()) &&
	 !table->item(row, 15)->text().isEmpty() );
}

int PlaylistDownloader::countDownloads()
{
	int missing_downloads = 0;
	for(int row = 0; row < table->rowCount(); row++)
	 if( shallDownloadFile(row) )
	{
		missing_downloads++;
	}
	return missing_downloads;
}

void PlaylistDownloader::slotBtnOk()
{
	{
		QDir dir(leFolder.text());
		if(!dir.exists() || leFolder.text().isEmpty()) {
			QMessageBox::information(this, "Directory missing", "Refusing to start, because no directory given.");
			return;
		}
	}

	int numDownloads = countDownloads();
	QMessageBox::information(this, "Starting Download", "Number of tracks to download: " + QString::number(numDownloads));


	progressDlg = new QProgressDialog("Downloading all Tracks...", "Stop after current track",
		0, numDownloads, this);
	progressDlg->show();
	currentDownloadRow = -1;

	QDir tmp("/tmp");

	if(tmp.exists(TMP_DIRECTORY))
	{
		tmp.cd(TMP_DIRECTORY);
		QStringList tmpFiles = tmp.entryList();
		QStringListIterator itr(tmpFiles);
		while (itr.hasNext())
		{
			tmp.remove(itr.next());
		}
	}
	else
	 tmp.mkdir(TMP_DIRECTORY);

	nextDownload();
	//...
//	{
		//convertion.download(...);

//	}
}

/*void PlaylistDownloader::updateTableWidget(int row, const QString& path)
{
	QString newPath = table->item(row, 13)->text();
	MPlayerConnection* mpl = sqlhelper.getMPlayerConnection();

	QString insert_cmd("UPDATE 'main' SET 'dateityp' = '");
	insert_cmd.append(mpl->fetchValue("get_audio_codec\n", "ANS_AUDIO_CODEC=");
	insert_cmd.append("', 'qualitaet' = '");
	insert_cmd.append(mpl->fetchValue("get_audio_bitrate\n", "ANS_AUDIO_BITRATE=");
	insert_cmd.append("', 'pfad' = '");
	insert_cmd.append(newPath);
	insert_cmd.append("' WHERE 'main'.'id' =");
	insert_cmd.append(QString::number(row));

	printf("Query command: %s\n", insert_cmd.toAscii().data());

	sqlhelper.exec(insert_cmd);
}*/

// TODO: move to MainWindow
/*void PlaylistDownloader::updateSqlTable(int row)
{
	QString newPath = table->item(row, 13)->text();
	MPlayerConnection* mpl = sqlhelper.getMPlayerConnection();

	QString insert_cmd("UPDATE 'main' SET 'dateityp' = '");
	insert_cmd.append(mpl->fetchValue("get_audio_codec\n", "ANS_AUDIO_CODEC=");
	insert_cmd.append("', 'qualitaet' = '");
	insert_cmd.append(mpl->fetchValue("get_audio_bitrate\n", "ANS_AUDIO_BITRATE=");
	insert_cmd.append("', 'pfad' = '");
	insert_cmd.append(newPath);
	insert_cmd.append("' WHERE 'main'.'id' =");
	insert_cmd.append(QString::number(row));

	printf("Query command: %s\n", insert_cmd.toAscii().data());

	sqlhelper.exec(insert_cmd);
}*/

void PlaylistDownloader::nextDownload()
{
#if 0
	// move latest song from a temporary to right place, update table and sql
	if(currentDownloadRow != -1)
	{
		QDir dir("/tmp");
		dir.cd(TMP_DIRECTORY);
		QString latestFile = dir.entryInfoList().last().absoluteFilePath();
		QString path = leFolder.text();
		path += QDir::separator();
		path += dir.entryInfoList().last().fileName();

		if(! QFile::rename(latestFile, path))
		 QFile::remove(latestFile); // keep /tmp clean!

		table->item(currentDownloadRow, 13)->setText(path);

		QString insert_cmd("UPDATE 'main' SET 'pfad' = '");
		insert_cmd.append(path);
		insert_cmd.append("' WHERE 'main'.'id' =");
		insert_cmd.append(QString::number(table->row2id(currentDownloadRow)));

		printf("Query command: %s\n", insert_cmd.toAscii().data());
		sqlhelper.exec(insert_cmd);
	}

	// get next downloadable file
	currentDownloadRow++;
	for( ; currentDownloadRow < table->rowCount(); currentDownloadRow++)
	 if(shallDownloadFile(currentDownloadRow))
	  break;

	if(currentDownloadRow == table->rowCount()) {
		delete progressDlg;
		QDir("/tmp").rmdir(TMP_DIRECTORY);
		QMessageBox::information(this, "Finished.", "Number of files that could not be downloaded: " + QString::number(countDownloads()));
	}
	else {
		// initiate download
		QString destName = "/tmp/";
		destName += TMP_DIRECTORY;
		destName += QDir::separator();
		destName += table->item(currentDownloadRow, 1)->text().toLower().replace(' ', '_');
		destName += ".%%(ext)";

		convertion.download(table->item(currentDownloadRow, 15)->text().toAscii().data(),
			destName.toAscii().data(),
			cbFormat.currentText().toAscii().data());
	}
#endif
	// move latest song from a temporary to right place, update table and sql
	if(currentDownloadRow != -1)
	{
		QFileInfoList dirList;
		{
			QDir dir("/tmp");
			dir.cd(TMP_DIRECTORY);
			dirList = dir.entryInfoList();
		}

		if(dirList.empty()) // this means the download failed
		{

		}
		else
		{
			QString latestFile = dirList.last().absoluteFilePath();
			QString path = leFolder.text();
			path += QDir::separator();
			char num_prefix[4];
			if(cbEnumerate.isChecked()) {
				snprintf(num_prefix, 4, "%02d_", table->item(currentDownloadRow, 0)->text().toInt());
				path += num_prefix;
			}
			path += dirList.last().fileName(); // keep the file name

			bool can_write_to_file = true;
			if(QFile::exists(path))
			{
				if(cbOverwrite.isChecked())
				 QFile::remove(path);
				else
				 can_write_to_file = false;
			}

			if(can_write_to_file)
			{
				if(! QFile::rename(latestFile, path))
				 QFile::remove(latestFile); // keep /tmp clean!

				table->item(currentDownloadRow, 13)->setText(path);

				QString insert_cmd("UPDATE 'main' SET 'pfad' = '");
				insert_cmd.append(path);
				insert_cmd.append("' WHERE 'main'.'id' =");
				insert_cmd.append(QString::number(table->row2id(currentDownloadRow)));

				printf("Query command: %s\n", insert_cmd.toAscii().data());
				sqlhelper.exec(insert_cmd);
			}
		}
	}

	// get next downloadable file
	currentDownloadRow++;
	for( ; currentDownloadRow < table->rowCount(); currentDownloadRow++)
	 if(shallDownloadFile(currentDownloadRow))
	  break;

	if(currentDownloadRow == table->rowCount()) {
		delete progressDlg;
		QDir("/tmp").rmdir(TMP_DIRECTORY);
		QMessageBox::information(this, "Finished.", "Number of files that could not be downloaded: " + QString::number(countDownloads()));
	}
	else // initiate download
	{
		//QString urlString = table->item(currentDownloadRow, 15)->text();
		QUrl url(table->item(currentDownloadRow, 15)->text());
		QString destName = "/tmp/";
		destName += TMP_DIRECTORY;
		destName += QDir::separator();

		if(url.scheme() == "res")
		{
			destName += url.path().remove(0,url.path().lastIndexOf('/')+1);
			url.setScheme("http");
			wget.download(url.toString().toAscii().data(), destName.toAscii().data());
		}
		else // must be a song
		{
			destName += table->item(currentDownloadRow, 1)->text().toLower().replace(' ', '_').replace(".","");

			if(url.scheme()=="mp3") {
				destName += ".mp3";
				url.setScheme("http");
				wget.download(url.toString().toAscii().data(), destName.toAscii().data());
			}
			else {
				destName += ".%%(ext)";
				youtubedl.download(url.toString().toAscii().data(),
					destName.toAscii().data(),
					cbFormat.currentText().toAscii().data());
			}
		}

	}
}


void PlaylistDownloader::slotTimerTimeout()
{
	progressDlg->setValue(progressDlg->value()+1);
	nextDownload();
}

void PlaylistDownloader::btnFolderClicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, "LoQuality - Open Directory", globals::MUSIC_ROOT);
	leFolder.setText(dir);
}

