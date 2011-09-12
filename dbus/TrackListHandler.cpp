

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
	    puts("TODO");
	//return Meta::Field::mprisMapFromTrack( The::playlist()->trackAt( position ) );
	return QVariantMap();
    }

    void TrackListHandler::SetLoop( bool enable )
    {
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
	    puts("TODO");
       // The::playlistActions()->play( index );
    }

    void TrackListHandler::slotTrackListChange()
    {
	    puts("TODO");
	//emit TrackListChange( The::playlist()->qaim()->rowCount() );
    }
}


