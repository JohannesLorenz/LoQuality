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

#include "md5sum.h"
#include "SqlHelper.h"

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
	QSqlQuery query;
	//printf("%s\n",QString("DELETE FROM `musicdb`.`main` WHERE `id`='%1'").arg(id).toAscii().data());
	query.exec( QString("DELETE FROM 'main' WHERE `id`='%1'").arg(id) );
}

void SqlHelper::INSERT(const char* filepath) const
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

	QSqlQuery query;
	QByteArray md5sum;
	calculate_md5sum(filepath, &md5sum);
	QDateTime last_changed = QFileInfo(filename).lastModified();
	printf("str: %s\n",QString("INSERT INTO 'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'others' ,'yours' ,'dateityp' ,'qualitaet' ,'bew_yours' ,'bew_others' ,'pfad', 'last_changed', 'md5sum') "
				   "VALUES ( NULL, %1, %2, %3, '', %4, %5, '0', '0', %6, %7, '0', '0', '%8', '%9', '%10');")
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
				).toAscii().data());

	const bool return_value = query.exec(
	/*QString str =*/	QString("INSERT INTO 'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'others' ,'yours' ,'dateityp' ,'qualitaet' ,'bew_yours' ,'bew_others' ,'pfad', 'last_changed', 'md5sum') "
					  "VALUES ( NULL, %1, %2, %3, '', %4, %5, '0', '0', %6, %7, '0', '0', '%8', '%9', '%10');")
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
			);
	if(!return_value) {
		fputs(query.lastError().text().toAscii().data(),stderr);
	}

}

void SqlHelper::CREATE(void) const
{
	QSqlQuery query;
	query.exec(
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
		"'md5sum' varchar(128)"
		");"
	);
}
