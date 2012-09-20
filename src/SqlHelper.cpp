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

#include <QString>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QSqlError>
#include <QVariant>

#include "md5sum.h"
#include "SqlHelper.h"

int SqlHelper::dbs_open = 0;

SqlHelper::SqlHelper(const QString& dbname)
{
#ifdef USE_MYSQL
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setUserName("music");
	db.setPassword("opensource");
	db.setHostName("localhost");
	db.setDatabaseName("musicdb");
#else
	/*if(!dbs_open)
	 db = QSqlDatabase::addDatabase("QSQLITE");
	else
	 db = QSqlDatabase::addDatabase();*/

	db = QSqlDatabase::addDatabase("QSQLITE", QString::number(dbs_open));


	QString path;
#ifdef Q_OS_LINUX
	// NOTE: We have to store database file into user home folder in Linux
	// path.append(QDir::home().path());
	// path.append(QDir::separator()).append("musicdb");
	// path = QDir::toNativeSeparators(path);
	// db.setDatabaseName(path);
	path.append(dbname);
#else
	// NOTE: File exists in the application private folder, in Symbian Qt implementation
	path.append(dbname);
#endif

	db.setDatabaseName(path);
#endif


	const bool ok = db.open();

	if(ok)
	 printf("CONNECTION TO DATABASE ESTABLISHED!!\n");
	else {
		printf("COULD NOT OPEN OR CREATE DATABASE!\n Error: %s",
			db.lastError().text().toAscii().data()
		);
		return;
	}
	dbs_open++;

#ifndef USE_MYSQL
	// might need to create sqlite table...

	QStringList tables = db.tables();
	printf("tables: %d\n",tables.size());

	CREATE_if_main_missing();
#endif
}

QString SqlHelper::corr(const QString& originalString)
{
	QString result = originalString;
	if(!result.isEmpty())
	{
		// short fix: do not replace first and last 'es!
		result[0] = '"';
		result[result.length()-1] = '"';

		result.replace('\'', "''");

		result[0] = '\'';
		result[result.length()-1] = '\'';
	}
	else
	 result = "''";
	return result;
}

void SqlHelper::DELETE(const int id) const
{
	//QSqlQuery query();
	//printf("%s\n",QString("DELETE FROM `musicdb`.`main` WHERE `id`='%1'").arg(id).toAscii().data());
	db.exec( QString("DELETE FROM 'main' WHERE `id`='%1'").arg(id) );
}

void SqlHelper::INSERT(const char* filepath, const char* url) const // TODO: NULL allowed?
{
	if(mPlayerConnection == NULL)
		return;

	mPlayerConnection->pass_remote_command((QString("loadfile \"%1\"\n").arg(filepath).toAscii().data()));

	QString filename = filepath; // filename as SQL wants it
	filename.replace('\'', "''");

	QString metaTitle = corr( mPlayerConnection->fetchValue("get_meta_title\n", "ANS_META_TITLE=") );
	printf("metaTitle: %s\n",metaTitle.toAscii().data());
	if(metaTitle == "''")
	{
		metaTitle = strrchr(filepath, QDir::separator().toAscii()) + 1;
		metaTitle.resize(metaTitle.lastIndexOf('.')); // get rid of ending

		/*
			handle spaces
		*/
		metaTitle.replace('_', ' ');
		bool lastWasSpace = true;
		for(int i = 0; i<metaTitle.size(); i++)
		{
			if( lastWasSpace )
			 metaTitle[i] = metaTitle[i].toUpper();
			lastWasSpace = (metaTitle[i] == ' ');
		}
		metaTitle.prepend('\'');
		metaTitle.append('\'');

		metaTitle = corr(metaTitle);
		printf("metaTitle now: %s\n",metaTitle.toAscii().data());
	}

	QByteArray md5sum;
	calculate_md5sum(filepath, &md5sum);
	QDateTime last_changed = QFileInfo(filename).lastModified();
	printf("str: %s\n",QString("INSERT INTO 'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'others' ,'yours' ,'dateityp' ,'qualitaet' ,'bew_yours' ,'bew_others' ,'pfad', 'last_changed', 'md5sum', 'url') "
				   "VALUES ( NULL, %1, %2, %3, '', %4, %5, '0', '0', %6, %7, '0', '0', '%8', '%9', '%10', '%11');")
				 .arg(
					 metaTitle,
					 corr( mPlayerConnection->fetchValue("get_meta_artist\n", "ANS_META_ARTIST=") ),
					 corr( mPlayerConnection->fetchValue("get_meta_album\n", "ANS_META_ALBUM=") ),

					 corr( mPlayerConnection->fetchValue("get_meta_genre\n", "ANS_META_GENRE=") ),
					 mPlayerConnection->fetchValue("get_meta_year\n", "ANS_META_YEAR="),
					 // TODO: interest

					 mPlayerConnection->fetchValue("get_audio_codec\n", "ANS_AUDIO_CODEC="),
					 mPlayerConnection->fetchValue("get_audio_bitrate\n", "ANS_AUDIO_BITRATE="),

					 filename,
					 md5sum.toHex().data()
				).arg(
					last_changed.toTime_t()
				).arg(
					url
				).toAscii().data());

	const QSqlQuery query = db.exec(
	/*QString str =*/	QString("INSERT INTO 'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'others' ,'yours' ,'dateityp' ,'qualitaet' ,'bew_yours' ,'bew_others' ,'pfad', 'last_changed', 'md5sum', 'url') "
					  "VALUES ( NULL, %1, %2, %3, '', %4, %5, '0', '0', %6, %7, '0', '0', '%8', '%9', '%10', '%11');")
					.arg(
						metaTitle,
						corr( mPlayerConnection->fetchValue("get_meta_artist\n", "ANS_META_ARTIST=") ),
						corr( mPlayerConnection->fetchValue("get_meta_album\n", "ANS_META_ALBUM=") ),

						corr( mPlayerConnection->fetchValue("get_meta_genre\n", "ANS_META_GENRE=") ),
						mPlayerConnection->fetchValue("get_meta_year\n", "ANS_META_YEAR="),
						// TODO: interest

						mPlayerConnection->fetchValue("get_audio_codec\n", "ANS_AUDIO_CODEC="),
						mPlayerConnection->fetchValue("get_audio_bitrate\n", "ANS_AUDIO_BITRATE="),

						filename )
					.arg(
						last_changed.toTime_t()
						)
					.arg (
						md5sum.toHex().data()
					)
					.arg (
						url
					)
			);
	if(!query.isValid()) {
		fputs(query.lastError().text().toAscii().data(),stderr);
	}

}

void SqlHelper::CREATE(void) const
{
	db.exec(
		"CREATE TABLE main ("
		"'id' INTEGER PRIMARY KEY, "
		"'titel' varchar(128),"
		"'kuenstler' varchar(128),"
		"'album' varchar(64),"
		"'tag' varchar(255),"
		"'genre' varchar(32),"
		"'jahr' smallint(6),"
		"'others' tinyint(4),"
		"'yours' tinyint(4),"
		"'dateityp' varchar(8),"
		"'qualitaet' smallint(6),"
		"'bew_yours' tinyint(4),"
		"'bew_others' tinyint(4),"
		"'pfad' varchar(255),"
		"'last_changed' int,"
		"'md5sum' varchar(128),"
		"'url' varchar(255)"
		");"
	);
}

bool SqlHelper::main_exists(void) const
{
	bool main_exists = false;
	QSqlQuery query = db.exec("SELECT name FROM  sqlite_master WHERE type='table' ORDER BY name;");
	printf("size: %d\n", query.size());
	//printf("last error: %s\n", query.lastError().text().toAscii().data());
	while (query.next() && !main_exists) {
		printf("tables: %s\n",query.value(0).toString().toAscii().data());
		if( query.value(0).toString() == "main" )
			main_exists = true;
	}

	printf("Main exists? %d\n",(int)main_exists);
	return main_exists;
}

