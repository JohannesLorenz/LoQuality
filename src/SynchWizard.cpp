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
}

SynchWizard::SynchWizard()
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
	query.exec("select id,last_changed as lol from main group by id having count(id) > (select count(*) from johannesdb.main where last_changed = lol);");

	if(!ok) {
		QMessageBox::information(NULL, "Sql Error", query.lastError().text().toAscii().data());
		return false;
	}

	int rowcount = 0;

	while (query.next()) { // WARNING: DO !!!NEVER!!! rely on query.size() here!!! (see qt docs...)
		puts("NEXT!!!");
		if(rowcount % 250 == 0)
		 newSongs.setRowCount(rowcount+250);

		QString id = query.value(0).toString();
		QDateTime _last_change = QDateTime::fromTime_t(query.value(1).toInt());
		QString last_change = (_last_change.isValid() && _last_change <= QDateTime::currentDateTime())?
			_last_change.toString("yyyy-MM-dd") : "(incorrect)";


		QTableWidgetItem* item_id = new QTableWidgetItem(id);
		QTableWidgetItem* item_last_change = new QTableWidgetItem(last_change);

		item_id->setData(Qt::DisplayRole, id);
		item_last_change->setData(Qt::DisplayRole, last_change);

		newSongs.setItem(rowcount, 0, item_id);
		newSongs.setItem(rowcount, 1, item_last_change);

		++rowcount;
	}


	return true;
}


