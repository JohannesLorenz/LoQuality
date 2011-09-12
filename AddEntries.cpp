#include "AddEntries.h"

#include "SqlHelper.h"

void AddEntryDlg::setupUi()
{
	resize(600, 495);

	lbl_title.setGeometry(5, 5, 290, 30);
	lbl_title.setText("Titel:");
	
	lbl_artist.setGeometry(5, 40, 290, 30);
	lbl_artist.setText("Künstler:");
	
	lbl_album.setGeometry(5, 75, 290, 30);
	lbl_album.setText("Album:");
	
	lbl_tag.setGeometry(5, 110, 290, 30);
	lbl_tag.setText("Tags:");
	
	lbl_genre.setGeometry(5, 145, 290, 30);
	lbl_genre.setText("Genre:");
	
	lbl_year.setGeometry(5, 180, 290, 30);
	lbl_year.setText("Jahr:");
	
	lbl_interest_philipp.setGeometry(5, 215, 290, 30);
	lbl_interest_philipp.setText("Interessiert Philipp?");
	
	lbl_interest_johannes.setGeometry(5, 250, 290, 30);
	lbl_interest_johannes.setText("Interessiert Johannes?");
	
	lbl_filetype.setGeometry(5, 285, 290, 30);
	lbl_filetype.setText("Dateityp:");
	
	lbl_quality.setGeometry(5, 320, 290, 30);
	lbl_quality.setText("Qualität:");
	
	lbl_vote_johannes.setGeometry(5, 355, 320, 30);
	lbl_vote_johannes.setText("Bewertung Johannes:");
	
	lbl_vote_philipp.setGeometry(5, 390, 320, 30);
	lbl_vote_philipp.setText("Bewertung Philipp:");
	
	lbl_path.setGeometry(5, 425, 355, 30);
	lbl_path.setText("Pfad:");
	
	le_title.setGeometry(305, 5, 290, 30);
	
	le_artist.setGeometry(305, 40, 290, 30);
	
	le_album.setGeometry(305, 75, 290, 30);
	
	le_tag.setGeometry(305, 110, 290, 30);
	
	le_genre.setGeometry(305, 145, 290, 30);
	
	le_year.setGeometry(305, 180, 290, 30);
	
	cb_interest_philipp.setGeometry(305, 215, 290, 30);
	cb_interest_johannes.setGeometry(305, 250, 290, 30);
	
	le_filetype.setGeometry(305, 285, 290, 30);
	
	le_quality.setGeometry(305, 320, 290, 30);
	
	sb_vote_johannes.setGeometry(305, 355, 290, 30);
	sb_vote_johannes.setMinimum(0);
	sb_vote_johannes.setMaximum(10);
	
	sb_vote_philipp.setGeometry(305, 390, 290, 30);
	sb_vote_philipp.setMinimum(0);
	sb_vote_philipp.setMaximum(10);
	
	le_path.setGeometry(305, 425, 140, 30);
	
	pb_choose_path.setGeometry(450, 425, 145, 30);
	pb_choose_path.setText("Durchsuchen...");
	
	pb_cancel.setGeometry(5, 460, 290, 30);
	pb_cancel.setText("Abbrechen");
	
	pb_insert.setGeometry(305, 460, 290, 30);
	pb_insert.setText("Übernehmen");
	
	if(edit)
	{
		++editnum; // increase row number
	
		printf("Editing row %d...\n", editnum);
		
		QString q("SELECT * FROM main WHERE 'id'='");
		q.append(QString::number(editnum));
		q.append("';");
		
		printf("Executing: %s\n", q.toAscii().data());
		
		QSqlQuery query(q);
		
		query.next();
		
		QString id = query.value(0).toString();
		QString title = query.value(1).toString();
		QString artist = query.value(2).toString();
		QString album = query.value(3).toString();
		QString tags = query.value(4).toString();
		QString genre = query.value(5).toString();
		QString year = query.value(6).toString();
		QString int_philipp = query.value(7).toString(); // int = interest
		QString int_johannes = query.value(8).toString();
		QString type = query.value(9).toString();
		QString quality = query.value(10).toString();
		QString vote_johannes = query.value(11).toString();
		QString vote_philipp = query.value(12).toString();
		QString path = query.value(13).toString();
		
		le_title.setText(title);
		le_artist.setText(artist);
		le_album.setText(album);
		le_tag.setText(tags);
		le_genre.setText(genre);
		le_year.setText(year);
		
		if(!strncmp(int_philipp.toAscii().data(), "1", 1))
		 cb_interest_philipp.setChecked(true);
		
		if(!strncmp(int_johannes.toAscii().data(), "1", 1))
		 cb_interest_johannes.setChecked(true);
		
		le_filetype.setText(type);
		le_quality.setText(quality);
		
		int vote_joh = atoi(vote_johannes.toAscii().data());
		sb_vote_johannes.setValue(vote_joh);
		
		int vote_phil = atoi(vote_philipp.toAscii().data());
		sb_vote_philipp.setValue(vote_phil);
		
		le_path.setText(path);
	}
	
	QSqlQuery queryArtist("SELECT * FROM main;");
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
	
	QSqlQuery queryAlbum("SELECT * FROM main;");
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
	
	QObject::connect(&pb_choose_path, SIGNAL(clicked()), this, SLOT(onButtonChoosePressed()));
	QObject::connect(&pb_cancel, SIGNAL(clicked()), this, SLOT(close()));
	
	if(!edit)
	 QObject::connect(&pb_insert, SIGNAL(clicked()), this, SLOT(OnButtonInsertPressed()));
	else
	 QObject::connect(&pb_insert, SIGNAL(clicked()), this, SLOT(OnButtonEditInsertPressed()));
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
#if 0
	//QString insert_cmd("INSERT INTO 'musicdb'.'main' ('id' ,'titel' ,'kuenstler' ,'album' ,'tag' ,'genre' ,'jahr' ,'philipp' ,'johannes' ,'dateityp' ,'qualitaet' ,'bew_joh' ,'bew_phil' ,'pfad') VALUES (NULL , '");
	QString insert_cmd("NULL , '");
	insert_cmd.append(le_title.text());
	insert_cmd.append("', '");
	insert_cmd.append(le_artist.text());
	insert_cmd.append("', '");
	insert_cmd.append(le_album.text());
	insert_cmd.append("', '");
	insert_cmd.append(le_tag.text());
	insert_cmd.append("', '");
	insert_cmd.append(le_genre.text());
	insert_cmd.append("', '");
	insert_cmd.append(le_year.text());
	insert_cmd.append("', '");
	
	if(cb_interest_philipp.isChecked())
	 insert_cmd.append("1");
	else
	 insert_cmd.append("0");
	
	insert_cmd.append("', '");
	
	if(cb_interest_johannes.isChecked())
	 insert_cmd.append("1");
	else
	 insert_cmd.append("0");
	
	insert_cmd.append("', '");
	insert_cmd.append(le_filetype.text());
	insert_cmd.append("', '");
	insert_cmd.append(le_quality.text());
	insert_cmd.append("', '");
	insert_cmd.append(QString::number(sb_vote_johannes.value()));
	insert_cmd.append("', '");
	insert_cmd.append(QString::number(sb_vote_philipp.value()));
	insert_cmd.append("', '");
	insert_cmd.append(le_path.text());
	insert_cmd.append("'");
	
	printf("Query command: %s\n", insert_cmd.toAscii().data());
	
	//QSqlQuery query;
	//query.exec(insert_cmd);
	sqlhelper.INSERT(insert_cmd);
#endif
	close();
}

void AddEntryDlg::OnButtonEditInsertPressed()
{
	QString insert_cmd("UPDATE 'musicdb'.'main' SET 'titel' = '");
	insert_cmd.append(le_title.text());
	insert_cmd.append("', 'kuenstler' = '");
	insert_cmd.append(le_artist.text());
	insert_cmd.append("', 'album' = '");
	insert_cmd.append(le_album.text());
	insert_cmd.append("', 'tag' = '");
	insert_cmd.append(le_tag.text());
	insert_cmd.append("', 'genre' = '");
	insert_cmd.append(le_genre.text());
	insert_cmd.append("', 'jahr' = '");
	insert_cmd.append(le_year.text());
	insert_cmd.append("', 'philipp' = '");
	
	if(cb_interest_philipp.isChecked())
	 insert_cmd.append("1");
	else
	 insert_cmd.append("0");
	
	insert_cmd.append("', 'johannes' = '");
	
	if(cb_interest_johannes.isChecked())
	 insert_cmd.append("1");
	else
	 insert_cmd.append("0");
	
	insert_cmd.append("', 'dateityp' = '");
	insert_cmd.append(le_filetype.text());
	insert_cmd.append("', 'qualitaet' = '");
	insert_cmd.append(le_quality.text());
	insert_cmd.append("', 'bew_joh' = '");
	insert_cmd.append(QString::number(sb_vote_johannes.value()));
	insert_cmd.append("', 'bew_phil' = '");
	insert_cmd.append(QString::number(sb_vote_philipp.value()));
	insert_cmd.append("', 'pfad' = '");
	insert_cmd.append(le_path.text());
	insert_cmd.append("' WHERE 'main'.'id' =");
	insert_cmd.append(QString::number(editnum));
	
	printf("Query command: %s\n", insert_cmd.toAscii().data());
	
	QSqlQuery query;
	query.exec(insert_cmd);
	
	accept();
}
