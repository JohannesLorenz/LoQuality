/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2012                                               */
/* Johannes Lorenz, Philipp Lorenz                                       */
/* https://github.com/DrSegfault/LoQuality                               */
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

#ifndef MD5SUM_H
#define MD5SUM_H

#include <cstdlib>
#include <QFile>
#include <QByteArray>
#include <QCryptographicHash>

/**
	Returns md5sum of a file. Note: to get the sum as the md5sum command prints it, use QByteArray.toHex();
*/
inline void calculate_md5sum(const char* filename, QByteArray* result) {

	QFile file(filename);
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray fileData = file.readAll();
		*result = QCryptographicHash::hash(fileData,QCryptographicHash::Md5);
	} else {
		printf("Sorry, could not open `%s'\n", filename);
		exit(1); // TODO
	}

}

#endif // MD5SUM_H
