/*************************************************************************/
/* LoQuality - A music player for Linux/UNIX.                            */
/* Copyright (C) 2010-2018                                               */
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

#ifndef ADD_ENTRIES_H
#define ADD_ENTRIES_H ADD_ENTRIES_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QVariant>
#include <QCompleter>
#include <QDialogButtonBox>
#include <QGridLayout>

//sql
#include <QSqlDatabase>
class SqlHelper;

class AddEntryDlg : public QDialog
{
Q_OBJECT

private:
	bool edit; //!< whether we edit a file or add one
	int editnum;
	const SqlHelper& sqlhelper;

	QLabel lbl_title;
	QLabel lbl_artist;
	QLabel lbl_album;
	QLabel lbl_tag;
	QLabel lbl_genre;
	QLabel lbl_year;
	QLabel lbl_interest_others;
	QLabel lbl_interest_yours;
	QLabel lbl_filetype;
	QLabel lbl_quality;
	QLabel lbl_vote_yours;
	QLabel lbl_vote_others;
	QLabel lbl_path;
	QLabel lbl_source;

	QLineEdit le_title;
	QLineEdit le_artist;
	QLineEdit le_album;
	QLineEdit le_tag;
	QLineEdit le_genre;
	QLineEdit le_year;

	QCheckBox cb_interest_others;
	QCheckBox cb_interest_yours;

	QLineEdit le_filetype;
	QLineEdit le_quality;

	QSpinBox sb_vote_yours;
	QSpinBox sb_vote_others;

	QLineEdit le_path;
	QPushButton pb_choose_path;

	QLineEdit le_source;

	QDialogButtonBox button_box;

	QGridLayout grid;

	static QString corr(const QString &originalString);
public:
	void setupUi();

	AddEntryDlg(const SqlHelper& _sqlhelper, bool _edit=false, QList<int>* _editnums = NULL)
		: sqlhelper(_sqlhelper), lbl_title(this), lbl_artist(this),
			// TODO: this should be replaced by an array of lbls?
		lbl_album(this), lbl_tag(this), lbl_genre(this),
		lbl_year(this), lbl_interest_others(this),
		lbl_interest_yours(this), lbl_filetype(this),
		lbl_quality(this), lbl_vote_yours(this),
		lbl_vote_others(this), lbl_path(this), lbl_source(this),
		le_title(this), le_artist(this), le_album(this),
		le_tag(this), le_genre(this), le_year(this),
		cb_interest_others(this), cb_interest_yours(this),
		le_filetype(this), le_quality(this), sb_vote_yours(this),
		sb_vote_others(this), le_path(this), pb_choose_path(this),
		le_source(this), button_box(QDialogButtonBox::Ok
		| QDialogButtonBox::Cancel, Qt::Horizontal, this)
	{
		edit = _edit;
		if(_edit)
		 editnum=_editnums->front();

		setupUi();
	}
private slots:
	void onButtonChoosePressed(); //!< choose file
	void OnButtonInsertPressed(); //!< accept, if adding a file
	void OnButtonEditInsertPressed(); //!< accept if only edit
};

#endif // ADD_ENTRIES_H
