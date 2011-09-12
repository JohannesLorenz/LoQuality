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
		void DELETE(const QString& id) const;

		//! executes the insert command on the whole database
		//! @param argumentList list of values, each in single quotes (except NULL) and seperated by comma. e.g.: "'a', NULL , 'c'"
		void INSERT(const char* filepath) const;

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
