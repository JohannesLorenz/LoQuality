/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2016                                               */
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

#include <stdio.h> // only for printf
#include <QSqlDatabase>
#include <QSqlQuery>

#include "MPlayerConnection.h"
class QSqlQuery;
class QString;

//! Generic helper class for sql commands
class SqlHelperBase
{
	private:
		static int dbs_open;
	protected:
		QSqlDatabase db; // todo: should be private...

		bool table_exists(const char* table_name) const;

	public:
		/**
			replaces all ' to \' in a string which begins and ends on '
			beginning and end are not replaced, if they exist
			there are no characters added or removed
		*/
		static QString corr(const QString& originalString);

		SqlHelperBase(const QString &dbname);
		inline ~SqlHelperBase() { db.close(); }

	/*	void inform(QSqlQuery* new_query)
		{
			Q_UNUSED( new_query );
		//	queries.push_back( new_query );
		}*/

		inline QSqlQuery exec(const QString & query ) const {
			return db.exec(query);
		}
};

//! Helper class for song databases
class SqlHelper : public SqlHelperBase
{
	private:
		mutable MPlayerConnection* mPlayerConnection;

	public:
		SqlHelper(const QString &dbname);
		inline ~SqlHelper() {}

		//! execute the delete command on the whole database to remove
		//! @param id id of the song which shall be deleted
		void DELETE(const int id) const;

		//! executes the insert command on the whole database
		//! @param argumentList list of values, each in single quotes (except NULL) and seperated by comma. e.g.: "'a', NULL , 'c'"
		bool INSERT(const char* filepath, const char* url = "") const;

		void CREATE_main(void) const;

		inline bool start_insert_sequence(void) const {
			return ( mPlayerConnection = new MPlayerConnection(true) ) != NULL;
		}

		inline void stop_insert_sequence(void) const {
			delete mPlayerConnection;
		}

		MPlayerConnection* getMPlayerConnection() const {
			return mPlayerConnection;
		}

};

//! Helper class for private options
class SqlHelperPrivateOptions : public SqlHelperBase
{
	public:
		SqlHelperPrivateOptions(const QString &dbname);
		inline ~SqlHelperPrivateOptions() {}

		//! inserts url of opened file to database
		//! @param url url to opened file
		//! @param time_opened time the file was recently opened
		//! (which is now if not given)
		void INSERT(const char* url, time_t time_opened = time(NULL)) const;

		//void CREATE_images(void) const;
		void CREATE_opened_files(void) const
		{
			db.exec("CREATE TABLE opened_files ('url' varchar(255), 'last_opened' int);");
		}
};

/*
  WHO INSERTS WHAT?
  FLASH-DLG: Everything (if the user knows it)
  FILE-DLG: Only filename (mplayer does the rest)
  */

#endif // _SQLHELPER_H_
