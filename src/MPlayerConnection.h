/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2011                                               */
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
