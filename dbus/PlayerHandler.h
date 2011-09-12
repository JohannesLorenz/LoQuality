#ifndef PLAYERHANDLER_H
#define PLAYERHANDLER_H

#include <QObject>
#include <QVariantMap>
#include <QDBusArgument>

class PlayerEngine;

namespace Mpris1 {
    class PlayerHandler;
}

namespace Mpris1
{
    struct Status
    {
	int Play; //Playing = 0, Paused = 1, Stopped = 2
	int Random; //Linearly = 0, Randomly = 1
	int Repeat; //Go_To_Next = 0, Repeat_Current = 1
	int RepeatPlaylist; //Stop_When_Finished = 0, Never_Give_Up_Playing = 1
    };
}

Q_DECLARE_METATYPE( Mpris1::Status )

// Marshall the Status data into a D-BUS argument
QDBusArgument &operator << ( QDBusArgument &argument, const Mpris1::Status &status );
// Retrieve the Status data from the D-BUS argument
const QDBusArgument &operator >> ( const QDBusArgument &argument, Mpris1::Status &status );

namespace Mpris1
{
	class PlayerHandler : public QObject
	{
	Q_OBJECT
	private:
		PlayerEngine* player; // here, we can exchange stuff with LoQuality's Player!
	public:
		PlayerHandler(PlayerEngine* _player);

		enum DBusCaps {
		 NONE                  = 0,
		 CAN_GO_NEXT           = 1 << 0,
		 CAN_GO_PREV           = 1 << 1,
		 CAN_PAUSE             = 1 << 2,
		 CAN_PLAY              = 1 << 3,
		 CAN_SEEK              = 1 << 4,
		 CAN_PROVIDE_METADATA  = 1 << 5,
		 CAN_HAS_TRACKLIST     = 1 << 6
		};

	public slots:
		Status GetStatus();

		void Pause();
		void Play();
		void PlayPause();
		void Stop();
		void StopAfterCurrent();
		void Prev();
		void Next();
		void Repeat( bool on );

		int  PositionGet();
		void PositionSet( int time );

		int  VolumeGet();
		void VolumeSet( int vol );

		int GetCaps();
		QVariantMap GetMetadata();

		// NB: Amarok extensions, not part of the mpris spec
		void VolumeUp( int step ) const;
		void VolumeDown( int step ) const;
		void Mute() const;
		void ShowOSD() const;

		void LoadThemeFile( const QString &path ) const;

		void Forward( int time );
		void Backward( int time );

		void updateStatus();

	signals:
		void CapsChange( int );
		void TrackChange( QVariantMap );
		void StatusChange( Mpris1::Status );

	public:
		QVariantMap GetTrackMetadata( void* track );

	private slots:
		void slotTrackChanged( void* track );
		void slotStateChanged();
	};

} // namespace Amarok

#endif // PLAYERHANDLER_H
