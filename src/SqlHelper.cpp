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

#include <QString>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QSqlError>
#include <QVariant>

#include "md5sum.h"
#include "SqlHelper.h"

#include "taglib/tag.h"
#include "taglib/fileref.h"
#include "taglib/audioproperties.h"

int SqlHelperBase::dbs_open = 0;

SqlHelperBase::SqlHelperBase(const QString& dbname)
{
#ifdef USE_MYSQL
	puts("NOT IMPLEMENTED YET!")
	exit(99);
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
			db.lastError().text().toLatin1().data()
		);
		return;
	}
	dbs_open++;
}

SqlHelper::SqlHelper(const QString& dbname)
	: SqlHelperBase(dbname)
{
#ifndef USE_MYSQL
	// might need to create sqlite table...

	QStringList tables = db.tables();
	printf("tables: %d\n",tables.size());

	if(!table_exists("main"))
	 CREATE_main();
#endif
}

QString SqlHelperBase::corr(const QString& originalString)
{
	QString result = originalString;
	if(!result.isEmpty())
	{
		printf("result: %s\n",result.toLatin1().data());
		// short fix: do not replace first and last 'es!
		bool isInQuotes = (result[0]=='\'') && (result[result.length()-1]=='\'');
		if(isInQuotes){
			result[0] = '"';
			result[result.length()-1] = '"';
		}

		result.replace('\'', "''");

		if(isInQuotes) {
			result[0] = '\'';
			result[result.length()-1] = '\'';
		}
	}
//	else
//	 result = "''";
	return result;
}

void SqlHelper::DELETE(const int id) const
{
	//QSqlQuery query();
	//printf("%s\n",QString("DELETE FROM `musicdb`.`main` WHERE `id`='%1'").arg(id).toLatin1().data());
	db.exec( QString("DELETE FROM 'main' WHERE `id`='%1'").arg(id) );
}

QString get_year(TagLib::Tag* t) {
	unsigned int internal_year = t->year();
	return internal_year ? QString::number(internal_year) : QString();
}

bool SqlHelper::INSERT(const char* filepath, const char* url) const // TODO: NULL allowed?
{
#if 0
	if(mPlayerConnection == NULL)
		return;

	mPlayerConnection->pass_remote_command((QString("loadfile \"%1\"\n").arg(filepath).toLatin1().data()));

	QString filename = filepath; // filename as SQL wants it
	filename.replace('\'', "''");

	QString metaTitle = corr( mPlayerConnection->fetchValue("get_meta_title\n", "ANS_META_TITLE=") );
	printf("metaTitle: %s\n",metaTitle.toLatin1().data());
	if(metaTitle == "''")
	{
		metaTitle = strrchr(filepath, QDir::separator().toLatin1()) + 1;
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
		printf("metaTitle now: %s\n",metaTitle.toLatin1().data());
	}

	QByteArray md5sum;
	calculate_md5sum(filepath, &md5sum);
	QDateTime last_changed = QFileInfo(filepath).lastModified();
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
				).toLatin1().data());

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
						corr(url)
					)
			);
	if(!query.isValid()) {
		fputs(query.lastError().text().toLatin1().data(),stderr);
	}
#else
	TagLib::FileRef fp(filepath);
	if(fp.isNull()) {
		printf("Warning: File not found.");
		return false;
	}
	TagLib::Tag* tag = fp.tag();
	TagLib::AudioProperties* audio_props = fp.audioProperties();

	QString filename = filepath; // filename as SQL wants it
	filename.replace('\'', "''");

	QString metaTitle = corr( TStringToQString(tag->title()) );
	printf("metaTitle: %s\n",metaTitle.toLatin1().data());
	if(metaTitle.isEmpty())
	{
		metaTitle = strrchr(filepath, QDir::separator().toLatin1()) + 1;
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
//		metaTitle.prepend('\'');
//		metaTitle.append('\'');

		metaTitle = corr(metaTitle);
		printf("metaTitle now: %s\n",metaTitle.toLatin1().data());
	}

	QByteArray md5sum;
	calculate_md5sum(filepath, &md5sum);
	QDateTime last_changed = QFileInfo(filepath).lastModified();

	QString audio_codec = filename;
	audio_codec.remove(0, audio_codec.lastIndexOf('.')+1);

	printf("str: %s\n",QString("INSERT INTO 'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'others' ,'yours' ,'dateityp' ,'qualitaet' ,'bew_yours' ,'bew_others' ,'pfad', 'last_changed', 'md5sum', 'url') "
					  "VALUES ( NULL, '%1', '%2', '%3', '', '%4', '%5', '0', '0', '%6', '%7', '0', '0', '%8', '%9', '%10', '%11');")
				 .arg(
					metaTitle,
					corr( TStringToQString(tag->artist()) ),
					corr( TStringToQString(tag->album()) ),

					corr( TStringToQString(tag->genre()) ),
					get_year(tag),
					// TODO: interest

					audio_codec,
					QString::number(audio_props->bitrate()),

					filename,
					md5sum.toHex().data()
				).arg(
					last_changed.toTime_t()
				).arg(
					url
				).toLatin1().data());

	const QSqlQuery query = db.exec(
	/*QString str =*/	QString("INSERT INTO 'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'others' ,'yours' ,'dateityp' ,'qualitaet' ,'bew_yours' ,'bew_others' ,'pfad', 'last_changed', 'md5sum', 'url') "
					  "VALUES ( NULL, '%1', '%2', '%3', '', '%4', '%5', '0', '0', '%6', '%7', '0', '0', '%8', '%9', '%10', '%11');")
					.arg(
						metaTitle,
						corr( TStringToQString(tag->artist()) ),
						corr( TStringToQString(tag->album()) ),

						corr( TStringToQString(tag->genre()) ),
						get_year(tag),
						// TODO: interest

						audio_codec,
						QString::number(audio_props->bitrate()),

						filename)
					.arg(
						last_changed.toTime_t()
						)
					.arg (
						md5sum.toHex().data()
					)
					.arg (
						corr(url)
					)
			);
	if(!query.isValid()) {
		fputs(query.lastError().text().toLatin1().data(),stderr);
	}

	return true;
#endif

}

void SqlHelper::CREATE_main(void) const
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

/*void SqlHelper::CREATE_images(void) const
{
	db.exec("CREATE TABLE images ('url' varchar(255));");
}*/

bool SqlHelperBase::table_exists(const char* table_name) const
{
	bool table_exists = false;
	QSqlQuery query = db.exec("SELECT name FROM  sqlite_master WHERE type='table' ORDER BY name;");
	printf("size: %d\n", query.size());
	//printf("last error: %s\n", query.lastError().text().toLatin1().data());
	while (query.next() && !table_exists) {
		printf("tables: %s\n",query.value(0).toString().toLatin1().data());
		if( query.value(0).toString() == table_name )
			table_exists = true;
	}

	printf("Table exists? %d\n",(int)table_exists);
	return table_exists;
}

void SqlHelperPrivateOptions::INSERT(const char* url, time_t time_opened) const
{


}

