#include "SyncAddManager.h"

const char* ARTIST_UNKNOWN = "unknown artist";
const char* ALBUM_UNKNOWN = "unknown album";

SyncAddManager::SyncAddManager(const SqlHelper& _sqlhelper)
	: fileAddBase(_sqlhelper, this)
{

}

bool SyncAddManager::appendItem(const QString &_artist, const QString &_album, const QString &title)
{
	QString artist = (artist=="")?ARTIST_UNKNOWN:_artist;
	QString album = (album=="")?ALBUM_UNKNOWN:_album;
	//		, album;

	fileAddBase.fileView.topLevelItem(0);

	Q_UNUSED(artist);
	Q_UNUSED(album);
	Q_UNUSED(title);
	return true;
}
