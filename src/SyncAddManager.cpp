#include "SyncAddManager.h"

SyncAddManager::SyncAddManager(const SqlHelper& _sqlhelper)
	: fileAddBase(_sqlhelper, this)
{

}

bool SyncAddManager::appendItem(const QString &artist, const QString &album, const QString &title)
{
	fileAddBase.fileView.topLevelItem(0);
	Q_UNUSED(artist);
	Q_UNUSED(album);
	Q_UNUSED(title);
	return true;
}
