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
