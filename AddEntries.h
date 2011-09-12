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

//sql
#include <QSqlDatabase>
#include <QSqlQuery>
class SqlHelper;

class AddEntryDlg : public QDialog
{
	Q_OBJECT

	private:
		bool edit;
		int editnum;
		const SqlHelper& sqlhelper;

		QLabel lbl_title;
		QLabel lbl_artist;
		QLabel lbl_album;
		QLabel lbl_tag;
		QLabel lbl_genre;
		QLabel lbl_year;
		QLabel lbl_interest_philipp;
		QLabel lbl_interest_johannes;
		QLabel lbl_filetype;
		QLabel lbl_quality;
		QLabel lbl_vote_johannes;
		QLabel lbl_vote_philipp;
		QLabel lbl_path;
		
		QLineEdit le_title;
		QLineEdit le_artist;
		QLineEdit le_album;
		QLineEdit le_tag;
		QLineEdit le_genre;
		QLineEdit le_year;

		QCheckBox cb_interest_philipp;
		QCheckBox cb_interest_johannes;

		QLineEdit le_filetype;
		QLineEdit le_quality;
		
		QSpinBox sb_vote_johannes;
		QSpinBox sb_vote_philipp;
		
		QLineEdit le_path;
		QPushButton pb_choose_path;
	
		QPushButton pb_cancel;
		QPushButton pb_insert;
	
	public:
		void setupUi();
	
		AddEntryDlg(const SqlHelper& _sqlhelper, bool _edit=false, int _editnum=0) : sqlhelper(_sqlhelper), lbl_title(this), lbl_artist(this), lbl_album(this), lbl_tag(this), lbl_genre(this), lbl_year(this), lbl_interest_philipp(this), lbl_interest_johannes(this), lbl_filetype(this), lbl_quality(this), lbl_vote_johannes(this), lbl_vote_philipp(this), lbl_path(this), le_title(this), le_artist(this), le_album(this), le_tag(this), le_genre(this), le_year(this), cb_interest_philipp(this), cb_interest_johannes(this), le_filetype(this), le_quality(this), sb_vote_johannes(this), sb_vote_philipp(this), le_path(this), pb_choose_path(this), pb_cancel(this), pb_insert(this)
		{
			if(_edit)
			{
				edit=true;
				editnum=_editnum;
			}
			else
			 edit=false;
		
			setupUi();
		}
	
	public slots:
		void onButtonChoosePressed();
		void OnButtonInsertPressed();
		void OnButtonEditInsertPressed();
};

#endif // ADD_ENTRIES_H
