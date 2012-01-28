#ifndef MD5SUM_H
#define MD5SUM_H

#include <cstdlib>
#include <QFile>
#include <QByteArray>
#include <QCryptographicHash>

/**
	Returns md5sum of a file. Note: to get the sum as the md5sum command prints it, use QByteArray.toHex();
*/
inline void calculate_md5sum(const QString& filename, QByteArray* result) {

	QFile file(filename);
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray fileData = file.readAll();
		*result = QCryptographicHash::hash(fileData,QCryptographicHash::Md5);
	} else
	 exit(1); // TODO

}

#endif // MD5SUM_H
