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

#ifndef ROOTHANDLER_H
#define ROOTHANDLER_H

#include <QObject>
#include <QDBusArgument>

namespace Mpris1
{
    struct Version
    {
	quint16 major;
	quint16 minor;
    };

    class RootHandler : public QObject
    {
	Q_OBJECT

	public:
	    RootHandler();
	    QString Identity();
	    void Quit();
	    Version MprisVersion();
	    // NB: Amarok extensions, not part of the mpris spec
	    void ShowOSD() const;
	    void LoadThemeFile( const QString &path ) const;
    };
}

Q_DECLARE_METATYPE(Mpris1::Version)

// Marshall the DBusVersion data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const Mpris1::Version &version);
// Retrieve the DBusVersion data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, Mpris1::Version &version);

#endif // ROOTHANDLER_H
