#include <QString>
#include <QDir>

#include "SqlHelper.h"

QString SqlHelper::corr(const QString& originalString)
{
	QString result = originalString;
	result[0] = '"';
	result[result.length()-1] = '"';
	result.replace('\'', "\\'");
	result[0] = '\'';
	result[result.length()-1] = '\'';
	return result;
}

void SqlHelper::DELETE(const QString& id) const
{
	QSqlQuery query;
	query.exec( QString("DELETE FROM `musicdb`.`main` WHERE `id`='%1'").arg(id) );
}

/*void SqlHelper::INSERT(QString& argumentList, bool askUser) const
{
	argumentList.prepend("INSERT INTO `musicdb`.`main` (`id` ,`titel` ,`kuenstler` ,`album` ,`tag` ,`genre` ,`jahr` ,`philipp` ,`johannes` ,`dateityp` ,`qualitaet` ,`bew_joh` ,`bew_phil` ,`pfad`) VALUES (");
	argumentList.append(");");

	printf("Query command: %s\n", argumentList.toAscii().data());

	// TODO: Add Query to List...
	QSqlQuery query;
	query.exec(argumentList);
}*/



void SqlHelper::INSERT(const char* filepath) const
{
	if(mPlayerConnection == NULL)
		return;

	mPlayerConnection->pass_remote_command((QString("loadfile \"%1\"\n").arg(filepath).toAscii().data()));


/*	QString title = mPlayerConnection.fetchValue("get_meta_title\n", "ANS_META_TITLE=");
	QString artist = mPlayerConnection.fetchValue("get_meta_artist\n", "ANS_META_ARTIST=");
	QString album = mPlayerConnection.fetchValue("get_meta_album\n", "ANS_META_ALBUM=");
	QString genre = mPlayerConnection.fetchValue("get_meta_genre\n", "ANS_META_GENRE=");
	QString year = mPlayerConnection.fetchValue("get_meta_year\n", "ANS_META_YEAR=");
	QString type = mPlayerConnection.fetchValue("get_audio_codec\n", "ANS_AUDIO_CODEC=");
	QString quality = mPlayerConnection.fetchValue("get_audio_bitrate\n", "ANS_AUDIO_BITRATE=");
	//QString length = mPlayerConnection.fetchValue("get_time_length\n", "ANS_LENGTH=").toFloat();*/

/*	printf("INSERT CMD: %s\n",
	QString("INSERT INTO `musicdb`.`main` (`id` ,`titel` ,`kuenstler` ,`album` ,`tag` ,`genre` ,`jahr` ,`philipp` ,`johannes` ,`dateityp` ,`qualitaet` ,`bew_joh` ,`bew_phil` ,`pfad`) "
			  "VALUES ( NULL, %1, %2, %3, '', %4, %5, '0', '0', %6, %7, '0', '0', '%8')")
			.arg(
				mPlayerConnection.fetchValue("get_meta_title\n", "ANS_META_TITLE="),
				mPlayerConnection.fetchValue("get_meta_artist\n", "ANS_META_ARTIST="),
				mPlayerConnection.fetchValue("get_meta_album\n", "ANS_META_ALBUM="),

				mPlayerConnection.fetchValue("get_meta_genre\n", "ANS_META_GENRE="),
				mPlayerConnection.fetchValue("get_meta_year\n", "ANS_META_YEAR="),
				// TODO: interest

				mPlayerConnection.fetchValue("get_audio_codec\n", "ANS_AUDIO_CODEC="),
				mPlayerConnection.fetchValue("get_audio_bitrate\n", "ANS_AUDIO_BITRATE="),

				filepath
				).toAscii().data() );*/
	QString filename = filepath; // filename as SQL wants it
	filename.replace('\'', "\\'");

	QString metaTitle = corr( mPlayerConnection->fetchValue("get_meta_title\n", "ANS_META_TITLE=") );
	printf("metaTitle: %s\n",metaTitle.toAscii().data());
	if(metaTitle == "''")
	{
		metaTitle = strrchr(filepath, QDir::separator().toAscii()) + 1;
		metaTitle.replace('_', ' ');
		metaTitle.resize(metaTitle.lastIndexOf('.'));
		bool lastWasSpace = true;
		for(int i = 0; i<metaTitle.size(); i++)
		{
			if( lastWasSpace )
			 metaTitle[i] = metaTitle[i].toUpper();
			lastWasSpace = (metaTitle[i] == ' ');
		}
		metaTitle.prepend('\'');
		metaTitle.append('\'');
		printf("metaTitle now: %s\n",metaTitle.toAscii().data());
	}

	QSqlQuery query;
	query.exec(
	/*QString str =*/	QString("INSERT INTO 'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'philipp' ,'johannes' ,'dateityp' ,'qualitaet' ,'bew_joh' ,'bew_phil' ,'pfad') "
					  "VALUES ( NULL, %1, %2, %3, '', %4, %5, '0', '0', %6, %7, '0', '0', '%8');")
					.arg(
						metaTitle,
						corr( mPlayerConnection->fetchValue("get_meta_artist\n", "ANS_META_ARTIST=") ),
						corr( mPlayerConnection->fetchValue("get_meta_album\n", "ANS_META_ALBUM=") ),

						corr( mPlayerConnection->fetchValue("get_meta_genre\n", "ANS_META_GENRE=") ),
						mPlayerConnection->fetchValue("get_meta_year\n", "ANS_META_YEAR="),
						// TODO: interest

						mPlayerConnection->fetchValue("get_audio_codec\n", "ANS_AUDIO_CODEC="),
						mPlayerConnection->fetchValue("get_audio_bitrate\n", "ANS_AUDIO_BITRATE="),

						filename
						)
			);
	printf("str: %s\n",QString("INSERT INTO 'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'philipp' ,'johannes' ,'dateityp' ,'qualitaet' ,'bew_joh' ,'bew_phil' ,'pfad') "
				   "VALUES ( NULL, %1, %2, %3, '', %4, %5, '0', '0', %6, %7, '0', '0', '%8');")
				 .arg(
					 metaTitle,
					 corr( mPlayerConnection->fetchValue("get_meta_artist\n", "ANS_META_ARTIST=") ),
					 corr( mPlayerConnection->fetchValue("get_meta_album\n", "ANS_META_ALBUM=") ),

					 corr( mPlayerConnection->fetchValue("get_meta_genre\n", "ANS_META_GENRE=") ),
					 mPlayerConnection->fetchValue("get_meta_year\n", "ANS_META_YEAR="),
					 // TODO: interest

					 mPlayerConnection->fetchValue("get_audio_codec\n", "ANS_AUDIO_CODEC="),
					 mPlayerConnection->fetchValue("get_audio_bitrate\n", "ANS_AUDIO_BITRATE="),

					 filename
					 ).toAscii().data());

}

