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
