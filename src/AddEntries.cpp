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

#include <QDateTime>
#include <QMessageBox>
#include <QSqlQuery>

#include "AddEntries.h"
#include "SqlHelper.h"
#include "md5sum.h"

void AddEntryDlg::setupUi()
{
	resize(600, 530);

	lbl_title.setText("Titel:");
	grid.addWidget(&lbl_title, 0, 0);
	
	lbl_artist.setText("Künstler:");
	grid.addWidget(&lbl_artist, 1, 0);
	
	lbl_album.setText("Album:");
	grid.addWidget(&lbl_album, 2, 0);
	
	lbl_tag.setText("Tags:");
	grid.addWidget(&lbl_tag, 3, 0);
	
	lbl_genre.setText("Genre:");
	grid.addWidget(&lbl_genre, 4, 0);
	
	lbl_year.setText("Jahr:");
	grid.addWidget(&lbl_year, 5, 0);
	
	lbl_interest_others.setText("Interessiert Andere?");
	grid.addWidget(&lbl_interest_others, 6, 0);
	
	lbl_interest_yours.setText("Interessiert Dich?");
	grid.addWidget(&lbl_interest_yours, 7, 0);
	
	lbl_filetype.setText("Dateityp:");
	grid.addWidget(&lbl_filetype, 8, 0);
	
	lbl_quality.setText("Qualität:");
	grid.addWidget(&lbl_quality, 9, 0);
	
	lbl_vote_yours.setText("Deine Bewertung:");
	grid.addWidget(&lbl_vote_yours, 10, 0);
	
	lbl_vote_others.setText("Externe Bewertung:");
	grid.addWidget(&lbl_vote_others, 11, 0);
	
	lbl_path.setText("Pfad:");
	grid.addWidget(&lbl_path, 12, 0);
	
	lbl_source.setText("Source:");
	grid.addWidget(&lbl_source, 13, 0);

	grid.addWidget(&le_title, 0, 1, 1, 2);
	
	grid.addWidget(&le_artist, 1, 1, 1, 2);
	
	grid.addWidget(&le_album, 2, 1, 1, 2);

	grid.addWidget(&le_tag, 3, 1, 1, 2);
	
	grid.addWidget(&le_genre, 4, 1, 1, 2);

	grid.addWidget(&le_year, 5, 1, 1, 2);

	grid.addWidget(&cb_interest_others, 6, 1, 1, 2);
	grid.addWidget(&cb_interest_yours, 7, 1, 1, 2);

	grid.addWidget(&le_filetype, 8, 1, 1, 2);

	grid.addWidget(&le_quality, 9, 1, 1, 2);

	sb_vote_yours.setMinimum(0);
	sb_vote_yours.setMaximum(10);
	grid.addWidget(&sb_vote_yours, 10, 1, 1, 2);

	sb_vote_others.setMinimum(0);
	sb_vote_others.setMaximum(10);
	grid.addWidget(&sb_vote_others, 11, 1, 1, 2);

	grid.addWidget(&le_path, 12, 1);

	pb_choose_path.setText("Durchsuchen...");
	grid.addWidget(&pb_choose_path, 12, 2);

	grid.addWidget(&le_source, 13, 1, 1, 2);

/*	pb_cancel.setGeometry(5, 495, 290, 30);
	pb_cancel.setText("Abbrechen");
	
	pb_insert.setGeometry(305, 495, 290, 30);
	pb_insert.setText("Übernehmen");
*/
	grid.addWidget(&button_box, 14, 0, 1, 3);

	setLayout(&grid);

	if(edit)
	{
		//++editnum; // increase row number // NOT ANYMORE - MainWindow must fix it!
	
		printf("Editing row %d...\n", editnum);
		
		QString q("SELECT * FROM main WHERE `id`='");
		q.append(QString::number(editnum));
		q.append("';");
		
		printf("Executing: %s\n", q.toAscii().data());
		
		QSqlQuery query = sqlhelper.exec(q);
		
		query.next();
		
		QString id = query.value(0).toString();
		QString title = query.value(1).toString();
		QString artist = query.value(2).toString();
		QString album = query.value(3).toString();
		QString tags = query.value(4).toString();
		QString genre = query.value(5).toString();
		QString year = query.value(6).toString();
		QString int_others = query.value(7).toString(); // int = interest
		QString int_yours = query.value(8).toString();
		QString type = query.value(9).toString();
		QString quality = query.value(10).toString();
		QString vote_yours = query.value(11).toString();
		QString vote_others = query.value(12).toString();
		QString path = query.value(13).toString();
		QString source = query.value(16).toString();
		
		le_title.setText(title);
		le_artist.setText(artist);
		le_album.setText(album);
		le_tag.setText(tags);
		le_genre.setText(genre);
		le_year.setText(year);
		
		if(!strncmp(int_others.toAscii().data(), "1", 1))
		 cb_interest_others.setChecked(true);
		
		if(!strncmp(int_yours.toAscii().data(), "1", 1))
		 cb_interest_yours.setChecked(true);
		
		le_filetype.setText(type);
		le_quality.setText(quality);
		
		int vote_yours_value = atoi(vote_yours.toAscii().data());
		sb_vote_yours.setValue(vote_yours_value);
		
		int vote_others_value = atoi(vote_others.toAscii().data());
		sb_vote_others.setValue(vote_others_value);
		
		le_path.setText(path);
		le_source.setText(source);
	}
	
	QSqlQuery queryArtist = sqlhelper.exec("SELECT * FROM main;");
	QStringList completitionListArtist;

	while (queryArtist.next()) {
		QString artist = queryArtist.value(2).toString();
		
		if(completitionListArtist.contains(artist))
		 continue;
		
		completitionListArtist << artist;
	}
	
	QCompleter *completerArtist = new QCompleter(completitionListArtist, this);
	completerArtist->setCaseSensitivity(Qt::CaseInsensitive);
	le_artist.setCompleter(completerArtist);
	
	QSqlQuery queryAlbum = sqlhelper.exec("SELECT * FROM main;");
	QStringList completitionListAlbum;

	while (queryAlbum.next()) {
		QString album = queryAlbum.value(2).toString();
		
		if(completitionListAlbum.contains(album))
		 continue;
		
		completitionListAlbum << album;
	}
	
	QCompleter *completerAlbum = new QCompleter(completitionListAlbum, this);
	completerAlbum->setCaseSensitivity(Qt::CaseInsensitive);
	le_artist.setCompleter(completerAlbum);

	/*
	 * signals
	 */

	QObject::connect(&pb_choose_path, SIGNAL(clicked()), this, SLOT(onButtonChoosePressed()));

	// for button boxes, see also:
	// http://qt-project.org/doc/qt-5.0/qtwidgets/dialogs-tabdialog.html

	QObject::connect(&button_box, SIGNAL(rejected()), this, SLOT(reject()));
	//QObject::connect(&pb_cancel, SIGNAL(clicked()), this, SLOT(onButtonCancelPressed()));
	
	if(!edit)
	 QObject::connect(&button_box, SIGNAL(accepted()), this, SLOT(OnButtonInsertPressed()));
	else
	 QObject::connect(&button_box, SIGNAL(accepted()), this, SLOT(OnButtonEditInsertPressed()));
}

void AddEntryDlg::onButtonChoosePressed()
{
	QString dir = QFileDialog::getOpenFileName(this, "LoQuality - Öffne Datei", QDir::homePath(), "Music files (*.wav *.flac *.ogg *.mp3 *.m4a *.mid *.mod *.wma *.)");
	
	if(dir.length() < 1)
	 return;
	
	le_path.setText(dir);
}

void AddEntryDlg::OnButtonInsertPressed() // depricated ?
{
	QString filepath = le_path.text();
	QByteArray md5sum;
	QDateTime last_changed;
	if(!filepath.isEmpty() && !QFile::exists(filepath))
	{
		if( QMessageBox::No == QMessageBox::question(this, "Incorrect Filepath", "The File does not exist. Do you really want to continue?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
		 return;

		calculate_md5sum(filepath.toAscii().data(), &md5sum);
		last_changed = QFileInfo(filepath).lastModified();
	}
	QString insert_cmd("INSERT INTO 'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'others' ,'yours' ,'dateityp' ,'qualitaet' ,'bew_yours' ,'bew_others' ,'pfad', 'last_changed', 'md5sum', 'url') VALUES (NULL , '");

	insert_cmd.append(corr(le_title.text()));
	insert_cmd.append("', '");
	insert_cmd.append(corr(le_artist.text()));
	insert_cmd.append("', '");
	insert_cmd.append(corr(le_album.text()));
	insert_cmd.append("', '");
	insert_cmd.append(corr(le_tag.text()));
	insert_cmd.append("', '");
	insert_cmd.append(corr(le_genre.text()));
	insert_cmd.append("', '");
	insert_cmd.append(le_year.text());
	insert_cmd.append("', '");
	
	if(cb_interest_others.isChecked())
	 insert_cmd.append("1");
	else
	 insert_cmd.append("0");
	
	insert_cmd.append("', '");
	
	if(cb_interest_yours.isChecked())
	 insert_cmd.append("1");
	else
	 insert_cmd.append("0");
	
	insert_cmd.append("', '");
	insert_cmd.append(le_filetype.text());
	insert_cmd.append("', '");
	insert_cmd.append(le_quality.text());
	insert_cmd.append("', '");
	insert_cmd.append(QString::number(sb_vote_yours.value()));
	insert_cmd.append("', '");
	insert_cmd.append(QString::number(sb_vote_others.value()));
	insert_cmd.append("', '");
	insert_cmd.append(filepath);
	insert_cmd.append("', '");
	insert_cmd.append(last_changed.toTime_t());
	insert_cmd.append("', '");
	insert_cmd.append(md5sum.toHex().data());
	insert_cmd.append("', '");
	insert_cmd.append(corr(le_source.text()));
	insert_cmd.append("');");
	
	printf("Query command: %s\n", insert_cmd.toAscii().data());
	
	sqlhelper.exec(insert_cmd);
	// sqlhelper.INSERT(insert_cmd);

	accept();
}

QString AddEntryDlg::corr(const QString& originalString)
{
	QString result = originalString;
	result.replace('\'', "''");
	return result;
}

void AddEntryDlg::OnButtonEditInsertPressed()
{
	//QString insert_cmd("UPDATE 'musicdb'.'main' SET 'titel' = '");
	QString insert_cmd("UPDATE 'main' SET 'titel' = '");
	insert_cmd.append(corr(le_title.text()));
	insert_cmd.append("', 'kuenstler' = '");
	insert_cmd.append(corr(le_artist.text()));
	insert_cmd.append("', 'album' = '");
	insert_cmd.append(corr(le_album.text()));
	insert_cmd.append("', 'tag' = '");
	insert_cmd.append(corr(le_tag.text()));
	insert_cmd.append("', 'genre' = '");
	insert_cmd.append(corr(le_genre.text()));
	insert_cmd.append("', 'jahr' = '");
	insert_cmd.append(le_year.text());
	insert_cmd.append("', 'others' = '");
	
	if(cb_interest_others.isChecked())
	 insert_cmd.append("1");
	else
	 insert_cmd.append("0");
	
	insert_cmd.append("', 'yours' = '");
	
	if(cb_interest_yours.isChecked())
	 insert_cmd.append("1");
	else
	 insert_cmd.append("0");
	
	insert_cmd.append("', 'dateityp' = '");
	insert_cmd.append(le_filetype.text());
	insert_cmd.append("', 'qualitaet' = '");
	insert_cmd.append(le_quality.text());
	insert_cmd.append("', 'bew_yours' = '");
	insert_cmd.append(QString::number(sb_vote_yours.value()));
	insert_cmd.append("', 'bew_others' = '");
	insert_cmd.append(QString::number(sb_vote_others.value()));
	insert_cmd.append("', 'pfad' = '");
	insert_cmd.append(le_path.text());
	insert_cmd.append("', 'url' = '");
	insert_cmd.append(corr(le_source.text()));

	insert_cmd.append("' WHERE 'main'.'id' =");
	insert_cmd.append(QString::number(editnum));
	
	printf("Query command: %s\n", insert_cmd.toAscii().data());
	
	sqlhelper.exec(insert_cmd);
	
	accept();
}
