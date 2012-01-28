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

#include <QDBusMetaType>
#include <QDBusConnection>

#include "../PlayerEngine.h"
#include "PlayerHandler.h"
#include "Mpris1PlayerAdaptor.h"
#include "../MainWindow.h"

//#include <Phonon/Global>

// Marshall the Status data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const Mpris1::Status &status)
{
	argument.beginStructure();
	argument << status.Play;
	argument << status.Random;
	argument << status.Repeat;
	argument << status.RepeatPlaylist;
	argument.endStructure();
	return argument;
}

// Retrieve the Status data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, Mpris1::Status &status)
{
	argument.beginStructure();
	argument >> status.Play;
	argument >> status.Random;
	argument >> status.Repeat;
	argument >> status.RepeatPlaylist;
	argument.endStructure();
	return argument;
}

namespace Field
{
	//actual string values are not final yet
	static const QString ALBUM          = "album";
	static const QString ARTIST         = "artist";
	static const QString BITRATE        = "audioBitrate";
	static const QString BPM            = "audioBPM";
	static const QString CODEC          = "audioCodec";
	static const QString COMMENT        = "comment";
	static const QString COMPOSER       = "composer";
	static const QString DISCNUMBER     = "discNumber";
	static const QString FILESIZE       = "size";
	static const QString GENRE          = "genre";
	static const QString LENGTH         = "mediaDuration";
	static const QString RATING         = "userRating";
	static const QString SAMPLERATE     = "audioSampleRate";
	static const QString TITLE          = "title";
	static const QString TRACKNUMBER    = "trackNumber";
	static const QString URL            = "url";
	static const QString YEAR           = "contentCreated";
	static const QString ALBUMARTIST    = "albumArtist";
	static const QString ALBUMGAIN      = "albumGain";
	static const QString ALBUMPEAKGAIN  = "albumPeakGain";
	static const QString TRACKGAIN      = "trackGain";
	static const QString TRACKPEAKGAIN  = "trackPeakGain";

	static const QString SCORE          = "autoRating";
	static const QString PLAYCOUNT      = "useCount";
	static const QString FIRST_PLAYED   = "firstUsed";
	static const QString LAST_PLAYED    = "lastUsed";

	static const QString UNIQUEID       = "id";

	// new
	static const QString COMPILATION    = "compilation";
}


namespace Mpris1		
{



	PlayerHandler::PlayerHandler(PlayerEngine* _player) : player(_player)
    {
	qDBusRegisterMetaType<Status>();

	setObjectName("PlayerHandler");

	new Mpris1PlayerAdaptor( this );
	// amarok extensions:
//	new Mpris1AmarokPlayerAdaptor( this );
	QDBusConnection::sessionBus().registerObject("/Player", this);
/*
	connect( The::playlistActions(), SIGNAL(navigatorChanged()),
		 this, SLOT(updateStatus()) );

	EngineController *engine = The::engineController();

	connect( engine, SIGNAL( playbackqDBusRegisterMetaTypeStateChanged() ),
		 this, SLOT( slotStateChanged() ) );
	connect( engine, SIGNAL( trackChanged( Meta::TrackPtr ) ),
		 this, SLOT( slotTrackChanged( Meta::TrackPtr ) ) );*/
    }

	Status PlayerHandler::GetStatus()
	{
		Status status = { 0, 0, 0, 0 };

		//player->
		//status.Play();
		puts("TODO: implement me");
		status.Play = 1; // Paused;
		return status;

/*	Status status = { 0, 0, 0, 0 };

	EngineController *engine = The::engineController();
	if( engine->isPlaying() )
	    status.Play = 0; //Playing
	else if( engine->isPaused() )
	    status.Play = 1; //Paused
	else
	    status.Play = 2; //Stopped

	if ( AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RandomTrack ||
	     AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RandomAlbum )
	    status.Random = 1;
	else
	    status.Random = 0;

	if ( AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RepeatTrack  )
	    status.Repeat = 1;
	else
	    status.Repeat = 0;
	if ( AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RepeatPlaylist ||
	     AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RepeatAlbum ||
	     AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RandomTrack ||
	     AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RandomAlbum )
	    status.RepeatPlaylist = 1;
	else
	    status.RepeatPlaylist = 0; //the music will not end if we play random
	return status;*/
	}

	void PlayerHandler::Pause() {
		player->slotPause();
	}

	void PlayerHandler::Play() {
		puts("TODO: keep random here!");
		player->slotPlay(false, NULL);
	}

	void PlayerHandler::PlayPause() {
		player->slotPause();
	}

	void PlayerHandler::Next() {
		puts("TODO: keep random here!");
		player->slotForward(false, NULL);
	}

	void PlayerHandler::Prev() {
		player->slotBackward();
	}

	void PlayerHandler::Repeat( bool on )
	{
		Q_UNUSED(on);
		puts("TODO: implement me!");
/*
	    debug() << (on ? "Turning repeat on" : "Turning repeat off");
	if( on )
	{
	    //if set on, just switch to repeat track
	    AmarokConfig::setTrackProgression( AmarokConfig::EnumTrackProgression::RepeatTrack );
	    The::playlistActions()->playlistModeChanged();
	}
	else if( AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RepeatTrack ||
		 AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RepeatAlbum ||
		 AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RepeatPlaylist )
	{
	    //if set to off, switch to normal mode if we are currently in one of the repeat modes.
	    AmarokConfig::setTrackProgression( AmarokConfig::EnumTrackProgression::Normal );
	    The::playlistActions()->playlistModeChanged();
	}

	//else just ignore event...*/
	}

    //position is specified in milliseconds
	int PlayerHandler::PositionGet()
	{
		// return position in milliseconds
		puts("TODO");
		return 0;
//	return The::engineController()->trackPositionMs();
	}

	void PlayerHandler::PositionSet( int time )
	{
		Q_UNUSED(time);
		puts("TODO: implement me!");
//	if( time > 0 && !The::engineController()->isStopped() )
//	    The::engineController()->seek( time );
	}

	void PlayerHandler::Stop()
	{
		player->slotStop();
//	The::engineController()->stop();
	}

	void PlayerHandler::StopAfterCurrent()
	{
		puts("TODO");
//	The::playlistActions()->setStopAfterMode( Playlist::StopAfterCurrent );
	}

	int PlayerHandler::VolumeGet()
	{
		puts("TODO!");
		return 50;
		//return The::engineController()->volume();
	}

	void PlayerHandler::VolumeSet( int vol )
	{
		Q_UNUSED(vol);
		puts("TODO!");
	//	The::engineController()->setVolume(vol);
	}

	void PlayerHandler::VolumeUp( int step ) const
	{
		Q_UNUSED(step);
		puts("TODO!");
	//	The::engineController()->increaseVolume( step );
	}

	void PlayerHandler::VolumeDown( int step ) const
	{
		Q_UNUSED(step);
		puts("TODO");
//	The::engineController()->decreaseVolume( step );
	}

	void PlayerHandler::Mute() const
	{
		puts("TODO");
//	The::engineController()->toggleMute();
	}

	void PlayerHandler::ShowOSD() const
	{
		puts("REMOVE THIS?");
//	Amarok::OSD::instance()->forceToggleOSD();
	}

	void PlayerHandler::LoadThemeFile( const QString &path ) const
	{
		Q_UNUSED(path);
		puts("REMOVE TIS?");
//	The::svgHandler()->setThemeFile( path );
	}

	void PlayerHandler::Forward( int time )
	{
		Q_UNUSED(time);
		puts("TODO");
//	if( time > 0 && !The::engineController()->isStopped() )
//	    The::engineController()->seek( The::engineController()->trackPosition() * 1000 + time );
	}

	void PlayerHandler::Backward( int time )
	{
		Q_UNUSED(time);
		puts("TODO");
//	if( time > 0 && The::engineController()->isStopped() )
//	    The::engineController()->seek( The::engineController()->trackPosition() * 1000 - time );
	}

	QVariantMap PlayerHandler::GetMetadata()
	{
//	return GetTrackMetadata( The::engineController()->currentTrack() );

    //note: track does not support first_played yet
		QVariantMap map;

		if( ! player->getCurSongItem() ) {
			puts("Play a song man!");
			return map;
		}

		const QString curSong = player->getCurSongItem()->text();

		/*if( curSong.isEmpty() )
		 map.insert( Field::TITLE, QVariant( QString("NOTHING!!!") ) );
		else
		 map.insert( Field::TITLE, QVariant( curSong ) );



		map.insert( Field::ALBUM, QVariant( QString("Album: TODO") ) );
		map.insert( Field::ARTIST, QVariant( QString("Artist: TODO") ) );
*/
		QTableWidget* _table = player->getCurSongItem()->tableWidget();
		const unsigned int _row = player->getCurSongItem()->row();

		//TODO: move that to the mainwindow or somewhere else!!!
		map.insert( Field::TITLE, _table->item(_row, MainWindow::COL_TITEL)->text() );
		map.insert( Field::ARTIST, _table->item(_row, MainWindow::COL_ARTIST)->text() );
		map.insert( Field::ALBUM, _table->item(_row, MainWindow::COL_ALBUM)->text() );

		printf("Yes, I am currently playing: %s.\n",curSong.toAscii().data());

		return map;

	}

	int PlayerHandler::GetCaps()
	{
		puts("TODO: GetCaps()");
		return 0;
    #if 0
	int caps = NONE;
	Meta::TrackPtr track = The::engineController()->currentTrack();
	caps |= CAN_HAS_TRACKLIST;
	if ( track )
	    caps |= CAN_PROVIDE_METADATA;
	if ( GetStatus().Play == 0 /*playing*/ )
	    caps |= CAN_PAUSE;
	if ( ( GetStatus().Play == 1 /*paused*/ ) || ( GetStatus().Play == 2 /*stoped*/ ) )
	    caps |= CAN_PLAY;
	if ( ( GetStatus().Play == 0 /*playing*/ ) || ( GetStatus().Play == 1 /*paused*/ ) )
	    caps |= CAN_SEEK;
	if ( ( The::playlist()->activeRow() >= 0 ) && ( The::playlist()->activeRow() <= The::playlist()->qaim()->rowCount() ) )
	{
	    caps |= CAN_GO_NEXT;
	    caps |= CAN_GO_PREV;
	}
	return caps;
	#endif
	}

	void PlayerHandler::updateStatus()
	{
/*	Status status = GetStatus();
	emit StatusChange( status );
	emit CapsChange( GetCaps() );*/
	}

    QVariantMap PlayerHandler::GetTrackMetadata( /*Meta::TrackPtr track*/ void* track )
    {
	Q_UNUSED(track);
	//return Meta::Field::mprisMapFromTrack( track );
	    puts("TODO!!!");
	return QVariantMap();
    }

    void PlayerHandler::slotTrackChanged( /*Meta::TrackPtr track*/ void* track )
    {
	Q_UNUSED(track);
	//emit TrackChange( GetTrackMetadata( track ) );
	updateStatus();
	    puts("TODO!!!");

    }

    void PlayerHandler::slotStateChanged()
    {
	    puts("TODO!!!");

	updateStatus();
    }
} // namespace Mpris1


