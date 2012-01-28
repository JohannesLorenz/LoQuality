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

#include "RootHandler.h"

#include "Mpris1RootAdaptor.h"
#include "../globals.h"

// Marshall the DBusVersion data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const Mpris1::Version &version)
{
    argument.beginStructure();
    argument << version.major << version.minor;
    argument.endStructure();
    return argument;
}

// Retrieve the DBusVersion data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, Mpris1::Version &version)
{
    argument.beginStructure();
    argument >> version.major >> version.minor;
    argument.endStructure();
    return argument;
}


namespace Mpris1
{

    RootHandler::RootHandler()
	//: QObject( kapp )
    {
	qDBusRegisterMetaType<Version>();

	new Mpris1RootAdaptor( this );
	// amarok extensions:
	//new Mpris1AmarokAppAdaptor( this );
	QDBusConnection::sessionBus().registerObject("/", this);
    }

    QString RootHandler::Identity()
    {
	    return QString( "%1 %2" ).arg( "LoQuality", globals::LQ_VERSION );
    }

    void RootHandler::Quit()
    {
	// Same as KStandardAction::Quit
	// kapp->quit();
	    puts("haha, ignored!");
    }

    Version RootHandler::MprisVersion()
    {
	struct Version version;
	version.major = 1;
	version.minor = 0;
	return version;
    }

    void RootHandler::ShowOSD() const
    {
	    puts("REMOVE ME!");
	//Amarok::OSD::instance()->forceToggleOSD();
    }

    void RootHandler::LoadThemeFile( const QString &path ) const
    {
	Q_UNUSED(path);
	puts("REMOVE ME!");
	//The::svgHandler()->setThemeFile( path );
    }

}
