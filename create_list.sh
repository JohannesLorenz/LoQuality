#!/bin/sh
# Author: Philipp Lorenz
# License GPL3

function myread
{
  while true; do
    read MYREAD_RESULT
    
    if [ "${MYREAD_RESULT:0:1}" != "#" ]; then
      break;
    fi
  done
}

printf "DB-Name? "
myread && DBNAME=$MYREAD_RESULT

sqlite3 $DBNAME <<< "CREATE TABLE main ('id' INTEGER PRIMARY KEY, 'titel' varchar(128),'kuenstler' varchar(128),'album' varchar(64),'tag' varchar(255),'genre' varchar(32),'jahr' smallint(6),'others' tinyint(4),'yours' tinyint(4),'dateityp' varchar(8),'qualitaet' smallint(6),'bew_yours' tinyint(4),'bew_others' tinyint(4),'pfad' varchar(255),'last_changed' int,'md5sum' varchar(128),'url' varchar(255));"

ID=1
ALBUM=""
TAG=""
OTHERS=0
YOURS=0
DATEITYP="mp3"
QUALITAET=320
BEW_YOURS=0
BEW_OTHERS=0
PFAD=""
LAST_CHANGED="strftime('%s', 'now')"
MD5SUM=""

echo "Generelle Infos:"
printf "Jahr? "
myread && JAHR=$MYREAD_RESULT
printf "Genre? "
myread && GENRE=$MYREAD_RESULT

echo
echo "Tracks einfügen:"
echo

while true; do
	printf "Titel ('END' zum Abbrechen)? "
	myread && TITEL=$MYREAD_RESULT

	if [ "$TITEL" == "END" ]; then
		break;
	fi

	printf "Künstler? "
	myread && KUENSTLER=$MYREAD_RESULT
	printf "URL? "
	myread && URL=$MYREAD_RESULT

	sqlite3 $DBNAME <<< "insert into main (id, titel, kuenstler, album, tag, genre, jahr, others, yours, dateityp, qualitaet, bew_yours, bew_others, pfad, last_changed, md5sum, url) values ('$ID', '$TITEL', '$KUENSTLER', '$ALBUM', '$TAG', '$GENRE', '$JAHR', '$OTHERS', '$YOURS', '$DATEITYP', '$QUALITAET', '$BEW_YOURS', '$BEW_OTHERS', '$PFAD', $LAST_CHANGED, '$MD5SUM', '$URL');"

	ID=$((ID + 1))
done

echo "Done."
echo ""
echo "  You can now open $DBNAME in LoQuality."
echo "  Press Ctrl+D there to download new songs."
echo ""
