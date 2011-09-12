#include "globals.h"

#define LO_RULEZ
#define USE_MPLAYER2

namespace globals
{
	QSettings* settings;
	QString MUSIC_ROOT;
	QString MPLAYER_EXE;
	const char* LQ_VERSION = "0.10";

#if 0
	const char* MUSIC_ROOT =
	#ifdef LAPTOP_RULEZ
		"/home/philipp/Desktop/musik"
	#elif defined ( LO_RULEZ )
		"/home/johannes/music"
	#else
		"/home/philipp/music"
	#endif
	;

	const char* MPLAYER_EXE =
#ifdef USE_MPLAYER2
		"mplayer2";
#else
		"mplayer";
#endif

	const char* LQ_VERSION = "0.10";
#endif
}
