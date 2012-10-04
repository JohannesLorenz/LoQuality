#!/bin/bash --
# Taken from, and slightly modified:
# Myspace music downloader v6.0
# Author of Myspace music downloader <= 5.4: Luka Pusic <luka@pusic.si>
# Author of modifications for a working v6.0 version: Basile Bruneau <basilebruneau@gmail.com>
#

if [ -z "$1" ]; then
 echo "";echo "Usage: `basename $0` songid [outdir]";echo "";exit
else
 songid=$1
 link="http://www.myspace.com/music/player?sid=$songid"
 songpage=`wget -L "$link" --quiet --user-agent="Mozilla" -O -`
 title=`echo "$songpage" | grep 'class="song"' | sed -e 's/.*class="song" title="//' -e 's/".*//' -e 's/\\//-/g' | head -n 1`
 rtmp=`echo "$songpage" | grep "rtmpte://" | tr "," "\n" | grep 'rtmpte://' | cut -d '"' -f 4 | head -n 1`
 rtmpr=`echo "$rtmp" | sed -e 's/\.com.*$//' | head -n 1`
 file=`echo "$rtmp" | sed -e 's/.*\.com\///' | head -n 1`
 extension=`echo "$file" | sed -e 's/^.*\.//' | head -n 1`
 rtmpfile=`echo "$file" | sed -e 's/\..*$//' | head -n 1`
  
 if [ "$extension" = "mp3" ]; then
  urlfile="mp3:$rtmpfile"
 elif [ "$extension" = "m4a" ]; then
  urlfile="mp4:$file"
 fi
  
 player=`echo "$songpage" | grep 'PixelPlayerUrl' | sed -e 's/^.*{"PixelPlayerUrl":"//' -e 's/".*//' | head -n 1`
 if [ ! "$title" ]; then
  title="$i"  #use number if no title found
 fi
 echo "Downloading $title..."
 rtmpdump -r "$rtmpr.com/" -a "" -f "LNX 11,2,202,235" -o "$artistname - $title.flv" -q -W "$player" -p "http://www.myspace.com" -y "$urlfile"
 artistname=$(echo "$artistname" | sed -e 's%/%_%g')
 #rtmpdump -l 2 -r "$rtmp" -o "$artistname - $title.flv" -q -W "http://lads.myspacecdn.com/videos/MSMusicPlayer.swf"
  
 if which ffmpeg >/dev/null; then
  echo "Converting $title to mp3..."
  ffmpeg -y -i "$artistname - $title.flv" -metadata TITLE="$title" -metadata ARTIST="$artistname" -acodec libmp3lame -ab 192000 -ar 44100 -f mp3 "$2" > /dev/null 2>&1 && rm "$artistname - $title.flv"
 fi
fi