#include <QDateTime>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

#include "SynchWizard.h"

void SynchWizard::retranslateUi()
{
	setWindowTitle("Synch Wizard");
}

void SynchWizard::setupUi()
{
	setPage(PAGE_INTRO, &introPage);
	setPage(PAGE_OPEN, &openPage);
	setPage(PAGE_SELECT, &selectPage);
	setPage(PAGE_TRANSMIT, &transmitPage);
	setPage(PAGE_SCRIPT, &scriptPage);
}

SynchWizard::SynchWizard(const SqlHelper& _sqlhelper) :
	selectPage(&selectedSongs, _sqlhelper),
	transmitPage(&selectedSongs)
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
	bool ok;

	printf("%s\n",( QString("ATTACH DATABASE \"%1\" AS johannesdb;").arg(field("importDbName").toString()).toAscii().data()));
	QSqlQuery query;
	ok = query.exec( QString("ATTACH DATABASE \"%1\" AS johannesdb;").arg(field("importDbName").toString()) );
	if(!ok) {
		QMessageBox::information(NULL, "Error loading database", query.lastError().text().toAscii().data());
		return false;
	}

	//QSqlQuery query2;
	query.exec("select id, titel, kuenstler, album, pfad, md5sum as lol from main group by id having count(id) > (select count(*) from johannesdb.main where md5sum = lol);");

	if(!ok) {
		QMessageBox::information(NULL, "Sql Error", query.lastError().text().toAscii().data());
		return false;
	}

	while (query.next()) { // WARNING: DO !!!NEVER!!! rely on query.size() here!!! (see qt docs...)
		//printf("new song: %s\n",query.value(4).toString().toAscii().data());
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

	printf("command: %s\n", command.toAscii().data());*/

	QString lca = selectedSongs->first()->text();
	while (itr.hasNext()) {
		const QString curString = itr.next()->text();
		int i = 0;
		for(; i < std::min(lca.length(), curString.length()); i++)
		 if(lca[i] != curString[i])
		  break;
		lca.resize(i);
	}


	printf("lca: %s\n", lca.toAscii().data());

	unsigned int i = 0;
	while ((i = lca.indexOf(QDir::separator(), i)) != -1) {
		fileBaseList.addItem(lca.left(i));
		++i;
	}

}


