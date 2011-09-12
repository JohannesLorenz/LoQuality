#ifndef MPLAYERCONNECTION_H
#define MPLAYERCONNECTION_H

#include <QString>

class MPlayerConnection
{
	private:
		pid_t mplayers_pid;
		const char* REMOTE_PIPE_NAME;
		int mplayer_output;

	public:
		//! passes a remote command to MPlayer
		//! @param command The full remote command. Should end on \\n.
		//! @return false if no connection could be established, otherwise true
		bool pass_remote_command(const char* command) const;
		QString fetchValue(const char* remoteCommand, const char* ansStr, bool msgBoxOnErr = false);

		MPlayerConnection(const char* _REMOTE_PIPE_NAME, bool nulldevice = false);
		~MPlayerConnection();
};

#endif // MPLAYERCONNECTION_H
