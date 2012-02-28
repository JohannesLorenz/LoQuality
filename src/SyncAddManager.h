#ifndef SYNCADDMANAGER_H
#define SYNCADDMANAGER_H

#include <QWidget>

#include "FileAddBase.h"
class SqlHelper;

class SyncAddManager : public QWidget
{
	FileAddBase fileAddBase;
public:
	bool appendItem(const QString& artist, const QString& album, const QString& title);
	SyncAddManager(const SqlHelper& _sqlhelper);
};

#endif // SYNCADDMANAGER_H
