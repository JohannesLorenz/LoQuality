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

#ifndef _SQLHELPER_H_
#define _SQLHELPER_H_ _SQLHELPER_H_

#include <QSqlQuery>
#include <stdio.h> // only for printf

#include "MPlayerConnection.h"
class QString;

#define _MPLAYER_META_PIPE "/tmp/lq_edit_pipe"

class SqlHelper
{
	private:
		mutable MPlayerConnection* mPlayerConnection;

		/**
			replaces all ' to \' in a string which begins and ends on '
			beginning and end are not replaced.
		 */
		static QString corr(const QString& originalString);

	public:
		//QList<QSqlQuery> queries;
		SqlHelper()
		{

		}

		void inform(QSqlQuery* new_query)
		{
			Q_UNUSED( new_query );
		//	queries.push_back( new_query );
		}

		//! execute the delete command on the whole database to remove
		//! @param id id of the song which shall be deleted
		void DELETE(const int id) const;

		//! executes the insert command on the whole database
		//! @param argumentList list of values, each in single quotes (except NULL) and seperated by comma. e.g.: "'a', NULL , 'c'"
		void INSERT(const char* filepath) const;

		void CREATE(void) const;

		inline bool start_insert_sequence(void) const {
			return ( mPlayerConnection = new MPlayerConnection(_MPLAYER_META_PIPE, true) ) != NULL;
		}

		inline void stop_insert_sequence(void) const {
			delete mPlayerConnection;
		}

};

/*
  WHO INSERTS WHAT?
  FLASH-DLG: Everything (if the user knows it)
  FILE-DLG: Only filename (mplayer does the rest)
  */

#endif // _SQLHELPER_H_
