#include "SyncAddManager.h"

SyncAddManager::SyncAddManager(const SqlHelper& _sqlhelper)
	: fileAddBase(_sqlhelper, this)
{

}

bool SyncAddManager::appendItem(const QString &artist, const QString &album, const QString &title)
{
	fileAddBase.fileView.topLevelItem(0);
}
