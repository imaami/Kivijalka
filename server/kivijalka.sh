#!/bin/bash

BASE="http://lissu.tampere.fi/monitor.php?stop="
STOP="0516"
FONT_SIZE=50
ROW_HEIGHT=55
PARAMS="&font_size=$FONT_SIZE&row_height=$ROW_HEIGHT"
URL="$BASE$STOP$PARAMS"
OUTPATH="/dev/shm/kivijalka"
BANPATH="/usr/share/kivijalka/banners"
CAP="cap-$STOP.png"
OUT="out-$STOP.png"
PIDFILE="$OUTPATH/lock-$STOP.pid"
ADDR='127.0.0.1'
PORT='8001'

mkdir -p "$OUTPATH"
mkdir -p "$BANPATH"

# get resolution
#x=$(xrandr -q|egrep "^Screen 0"|sed -r 's|(^.*current )([1-9][0-9]*) x ([1-9][0-9]*),.*$|\2 \3|')
#PX_W="${x%% *}"
#PX_H="${x##* }"
PX_W=1080
PX_H=1920

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

if [[ -f "$OUTPATH/$CAP" ]]; then
  cp "$OUTPATH/$CAP" "$OUTPATH/$OUT"
fi

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

(
  wsserver -c "$OUTPATH/$CAP" -o "$OUTPATH/$OUT" -b "$BANPATH" \
           -g "${PX_W}x${PX_H}" -a "$ADDR" -p "$PORT"
) &>/dev/null &

xset -dpms
xset s off

while true; do
  /usr/bin/qiv -fiRT "$OUTPATH/$OUT"
  sleep 5
done

if [[ -f "$PIDFILE" ]]
then
  kill -9 $(< "$PIDFILE") &>/dev/null
  rm -f "$PIDFILE"
fi
