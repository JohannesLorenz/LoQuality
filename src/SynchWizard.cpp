/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2018                                               */
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

#include <unistd.h>
#include <QDateTime>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

#include "SqlHelper.h"
#include "SynchWizard.h"

void SynchWizard::retranslateUi()
{
	setWindowTitle("Synch Wizard");
}

void SynchWizard::setupUi()
{
	setPage(PAGE_INTRO, &introPage);
	setPage(PAGE_START, &startPage);
	setPage(PAGE_OPEN, &openPage);
	setPage(PAGE_SELECT, &selectPage);
	setPage(PAGE_TRANSMIT, &transmitPage);
	setPage(PAGE_SCRIPT, &scriptPage);
	setPage(PAGE_SCRIPT_RUN, &scriptRunPage);
	setPage(PAGE_SCRIPT_DUMP, &scriptDumpPage);
}

SynchWizard::SynchWizard(const SqlHelper& _sqlhelper) :
	selectPage(&selectedSongs, _sqlhelper),
	transmitPage(&selectedSongs),
	scriptRunPage(&selectedSongs)
{
	setupUi();
	retranslateUi();
}

void OpenPage::selectDatabase()
{
	lineEdit.setText(QFileDialog::getOpenFileName(NULL, "LoQuality database"));
}

bool SelectPage::getSongList()
{
/*
	also erst starte ich sqlite normal auf meine musicdb
	dann lade ich deine datenbank dazu, also:
	ATTACH DATABASE "musicdb_johannes.sqlite" AS johannesdb;
	und der folgende befehl gibt dir alle ids und md5sums von meiner datenbank, die in deiner datenbank nicht drin sind:
	select id,last_changed as lol from main group by id having count(id) > (select count(*) from johannesdb.main where last_changed = lol);
	*/
	printf("%s\n",( QString("ATTACH DATABASE \"%1\" AS johannesdb;").arg(field("importDbName").toString()).toLatin1().data()));
	QSqlQuery query = sqlhelper.exec( QString("ATTACH DATABASE \"%1\" AS johannesdb;").arg(field("importDbName").toString()) );
	if(!query.isValid()) {
		QMessageBox::information(NULL, "Error loading database", query.lastError().text().toLatin1().data());
		return false;
	}

	//QSqlQuery query2;
	query = sqlhelper.exec("select id, titel, kuenstler, album, pfad, md5sum as lol from johannesdb.main group by id having count(id) > (select count(*) from main where md5sum = lol);");
	if(!query.isValid()) {
		QMessageBox::information(NULL, "Sql Error", query.lastError().text().toLatin1().data());
		return false;
	}
	while (query.next()) { // WARNING: DO !!!NEVER!!! rely on query.size() here!!! (see qt docs...)
		//printf("new song: %s\n",query.value(4).toString().toLatin1().data());
		syncAddManager.appendItem(query.value(2).toString(), query.value(3).toString(), query.value(1).toString(), query.value(4).toString());
	}

	return true;
}

void TransmitPage::runTransmission()
{
	QString command;

	QListIterator<QListWidgetItem*> itr(*selectedSongs);
/*	while (itr.hasNext()) {
		const QListWidgetItem* curItem = itr.next();
		puts("added 1");
		command += curItem->text();
		command.append(' ');
	}

	printf("command: %s\n", command.toLatin1().data());*/

	QString lca = selectedSongs->first()->text();
	while (itr.hasNext()) {
		const QString curString = itr.next()->text();
		int i = 0;
		for(; i < std::min(lca.length(), curString.length()); i++)
		 if(lca[i] != curString[i])
		  break;
		lca.resize(i);
	}


	printf("lca: %s\n", lca.toLatin1().data());

	int i = 0;
	while ((i = lca.indexOf(QDir::separator(), i)) != -1) {
		fileBaseList.addItem(lca.left(i));
		++i;
	}

}

#include <sys/wait.h>

void ScriptRunPage::slotTimerTimeout()
{
	puts("TIMEOUT!");
	if( 0 != waitpid(scps_pid, NULL, WNOHANG) )  { // wait for ffmpeg to finish
		puts("WAIT FINISHED!");

		runNext();

		/*progressDlg->cancel();
		progressDlg=NULL;
		if(insertSql) { // cbAddAfterwards.isChecked()
			sqlhelper.start_insert_sequence();
			sqlhelper.INSERT(curOutName.toLatin1().data());
			sqlhelper.stop_insert_sequence();
		}*/
	}
	puts("TIMEOUT2!");
}

bool ScriptRunPage::forkScp(QString nextFile)
{
	QString srcFile, destFile;
	const int num_chars_from_right = nextFile.length() - field("fileBase").toString().length();

	QString port, rsaKey, nameAtHost;

	if(field("btnFriendList").toBool())
	{
		// TODO (FRIEND): Read these values from DB
	}
	else
	{
		port = field("port").toString();
		rsaKey = field("rsaKey").toString();
		nameAtHost = field("nameAtHost").toString();
	}
	const QString ip = field("ip").toString();

	puts("forkScp");
	scps_pid=fork();
	if(scps_pid < 0) {
		QMessageBox::information(this, "Sorry...", "... fork() ging leider nicht, kann scp nicht starten :(");
		return false;
	}	
	else if(scps_pid == 0) {

		{
		QString appendix = nextFile.right(num_chars_from_right);
		QString destDir = appendix.left(appendix.lastIndexOf('/'));
		destDir.replace("'", "'");
		destDir.replace(" ", " ");
		QString mkdirCommand = QString("mkdir -p \"%6/%7\"").arg(
			globals::MUSIC_ROOT,
			destDir);

		nextFile.replace("'", "'");
		nextFile.replace(" ", " ");
		printf("%s\n",mkdirCommand.toLatin1().data());
		QString scpCommand = QString("scp -P %1 -i %2 %3@%4:\"%5\" \"%6/%7\"")
				.arg(	port,
					rsaKey,
					nameAtHost,
					ip,
					nextFile,globals::MUSIC_ROOT,
					destDir);
		//sleep(1);
//		printf("%s\n",scpCommand.toLatin1().data());
		scpCommand.replace("'", "'");
		scpCommand.replace(" ", " ");
		system(mkdirCommand.toLatin1().data()); // TODO: use execv
//		system(scpCommand.toLatin1().data());
//		progressBar.setValue(progressBar.value()+1);

		// TODO: full path for first argument
		srcFile = QString("%1@%2:\"%3\"").arg(nameAtHost,
				ip,
				nextFile);
		destFile = QString("%6/%7").arg(globals::MUSIC_ROOT,
			destDir);

		}

		printf("scp -P %s -i %s %s %s\n",
			port.toLatin1().data(), rsaKey.toLatin1().data(),
			srcFile.toLatin1().data(), destFile.toLatin1().data());

		execlp("scp", "scp",
			"-P", port.toLatin1().data(),
			"-i", rsaKey.toLatin1().data(),
			srcFile.toLatin1().data(),
			destFile.toLatin1().data(),
			NULL);

		exit(0);
	}
	return true;
}

