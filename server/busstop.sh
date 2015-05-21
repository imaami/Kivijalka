#!/bin/bash

BASE="http://lissu.tampere.fi/monitor.php?stop="
STOP="0510"
FONT_SIZE=32
ROW_HEIGHT=35
PARAMS="&font_size=$FONT_SIZE&row_height=$ROW_HEIGHT"
URL="$BASE$STOP$PARAMS"
OUTPATH="/dev/shm/busstop"
CAP="cap-$STOP.png"
OUT="out-$STOP.png"
PIDFILE="$OUTPATH/lock-$STOP.pid"
#DELAY=5

mkdir -p "$OUTPATH"

# get resolution
#x=$(xrandr -q|egrep "^Screen 0"|sed -r 's|(^.*current )([1-9][0-9]*) x ([1-9][0-9]*),.*$|\2 \3|')
#PX_W="${x%% *}"
#PX_H="${x##* }"
PX_W=1280
PX_H=1024

# defaults if we didn't find a resolution
if [[ -z $PX_W ]]
then
  PX_W=1280
fi
if [[ -z $PX_H ]]
then
  PX_H=1024
fi

if [[ -f "$PIDFILE" ]]
then
  kill -9 $(< "$PIDFILE") &>/dev/null
  rm -f "$PIDFILE"
fi

cutycapt --url="$URL" \
  --private-browsing=on \
  --javascript=off \
  --java=off \
  --plugins=off \
  --auto-load-images=on \
  --min-width=$PX_W \
  --min-height=$PX_H \
  --out="$OUTPATH/$CAP"

(
  echo $BASHPID > "$PIDFILE"
  while true
  do
    if cutycapt --url="$URL" \
         --private-browsing=on \
         --javascript=off \
         --java=off \
         --plugins=off \
         --auto-load-images=on \
         --min-width=$PX_W \
         --min-height=$PX_H \
         --out="$OUTPATH/$CAP"
    then
      sleep 5
    fi
  done
) &>/dev/null &

#/usr/bin/feh --no-fehbg --reload $DELAY -g "${PX_W}x${PX_H}+0+0" --zoom 100 --keep-zoom-vp -qpxY "$OUTPATH/$OUT"
/usr/bin/qiv -RT "$OUTPATH/$OUT"

if [[ -f "$PIDFILE" ]]
then
  kill -9 $(< "$PIDFILE") &>/dev/null
  rm -f "$PIDFILE"
fi
