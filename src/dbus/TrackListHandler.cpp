/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2012                                               */
/* Johannes Lorenz, Philipp Lorenz                                       */
/* https://github.com/JohannesLorenz/LoQuality                           */
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

#include "TrackListHandler.h"

#include "Mpris1TrackListAdaptor.h"

namespace Mpris1
{

    TrackListHandler::TrackListHandler()
	//: QObject( kapp )
    {
        new Mpris1TrackListAdaptor(this);
        QDBusConnection::sessionBus().registerObject( "/TrackList", this );
       // connect( The::playlist()->qaim(), SIGNAL( rowsInserted( const QModelIndex&, int, int ) ), this, SLOT( slotTrackListChange() ) );
       // connect( The::playlist()->qaim(), SIGNAL( rowsRemoved( const QModelIndex&, int, int ) ), this, SLOT( slotTrackListChange() ) );
    }

    int TrackListHandler::AddTrack( const QString& url, bool playImmediately )
    {
	Q_UNUSED(url);
	Q_UNUSED(playImmediately);
	    puts("TODO");
	    return -1;
     /*   Meta::TrackPtr track = CollectionManager::instance()->trackForUrl( url );
        if( track )
        {
            if( playImmediately )
                The::playlistController()->insertOptioned( track, Playlist::DirectPlay );
            else
                The::playlistController()->insertOptioned( track, Playlist::Append );
            return 0;
        }
        else
	    return -1;*/
    }

    void TrackListHandler::DelTrack( int index )
    {
	    Q_UNUSED(index);
	    puts("TODO");
	/*if( index < GetLength() )
	    The::playlistController()->removeRow( index );*/
    }

    int TrackListHandler::GetCurrentTrack()
    {
	    puts("TODO");
	return 1;
	//return The::playlist()->activeRow();
    }

    int TrackListHandler::GetLength()
    {
	    puts("TODO");
	//return The::playlist()->qaim()->rowCount();
	return 100;
    }

    QVariantMap TrackListHandler::GetMetadata( int position )
    {
	    Q_UNUSED(position);
	    puts("TODO");
	//return Meta::Field::mprisMapFromTrack( The::playlist()->trackAt( position ) );
	return QVariantMap();
    }

    void TrackListHandler::SetLoop( bool enable )
    {
	    Q_UNUSED(enable);
	    puts("TODO");
       /* if( enable )
        {
            AmarokConfig::setTrackProgression( AmarokConfig::EnumTrackProgression::RepeatPlaylist );
            The::playlistActions()->playlistModeChanged();
        }
        else if( AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RepeatPlaylist )
        {
             AmarokConfig::setTrackProgression( AmarokConfig::EnumTrackProgression::Normal );
            The::playlistActions()->playlistModeChanged();
	}*/
    }

    void TrackListHandler::SetRandom( bool enable )
    {
	    Q_UNUSED(enable);
	    puts("TODO");
       /* if( enable )
        {
            AmarokConfig::setTrackProgression( AmarokConfig::EnumTrackProgression::RandomTrack );
            The::playlistActions()->playlistModeChanged();
        }
        else if( AmarokConfig::trackProgression() == AmarokConfig::EnumTrackProgression::RandomTrack )
        {
             AmarokConfig::setTrackProgression( AmarokConfig::EnumTrackProgression::Normal );
            The::playlistActions()->playlistModeChanged();
	}*/
    }

    void TrackListHandler::PlayTrack( int index )
    {
	    Q_UNUSED(index);
	    puts("TODO");
       // The::playlistActions()->play( index );
    }

    void TrackListHandler::slotTrackListChange()
    {
	    puts("TODO");
	//emit TrackListChange( The::playlist()->qaim()->rowCount() );
    }
}


