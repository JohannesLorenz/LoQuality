#include "RootHandler.h"

#include "Mpris1RootAdaptor.h"
#include "globals.h"

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
	     puts("REMOVE ME!");
	//The::svgHandler()->setThemeFile( path );
    }

}
