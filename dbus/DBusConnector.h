/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2011                                               */
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

#ifndef DBUSCONNECTOR_H
#define DBUSCONNECTOR_H

#include <QDBusConnection>
#include <QDBusConnectionInterface>

#include "RootHandler.h"
#include "TrackListHandler.h"
#include "PlayerHandler.h"

class PlayerEngine;

class DBusConnector {
	Mpris1::RootHandler root_handler;
	Mpris1::TrackListHandler tracklist_handler;
	Mpris1::PlayerHandler player_handler;
	//PlayerEngine* player;
public:
	DBusConnector(PlayerEngine* _player) : player_handler(_player){
	}

	void start() {
		QDBusConnection::sessionBus().registerService("org.mpris.LoQuality");
		QDBusConnection::sessionBus().registerService("org.kde.amarok");
		QDBusConnection::sessionBus().registerService("org.mpris.amarok");
	}

	void stop() {
		// work around for KUniqueApplication being not completely implemented on windows
		QDBusConnectionInterface* dbusService = NULL;
		if (QDBusConnection::sessionBus().isConnected() && (dbusService = QDBusConnection::sessionBus().interface()))
		{
			dbusService->unregisterService("org.mpris.LoQuality");
			dbusService->unregisterService("org.kde.amarok");
			dbusService->unregisterService("org.mpris.amarok");
		}
	}

	~DBusConnector() {
		stop();
	}

};

#endif // DBUSCONNECTOR_H
