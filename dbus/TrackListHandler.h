

#ifndef TRACKLISTHANDLER_H
#define TRACKLISTHANDLER_H

#include <QObject>
#include <QVariantMap>

namespace Mpris1
{
    typedef void TrackList;
    class TrackListHandler : public QObject
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.MediaPlayer.tracklist")

        public:
            TrackListHandler();

            int AddTrack( const QString &, bool );
            void DelTrack( int index );
            int GetCurrentTrack();
            int GetLength();
            QVariantMap GetMetadata( int );
            void SetLoop( bool enable );
            void SetRandom( bool enable );

            // NB: Amarok extensions, not part of the mpris spec
	    void PlayTrack( int index );

        signals:
            void TrackListChange( int );

        private slots:
            void slotTrackListChange();
    };
}

#endif // TRACKLISTHANDLER_H
