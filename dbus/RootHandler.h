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
