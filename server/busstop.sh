#!/bin/bash

BASE="http://lissu.tampere.fi/monitor.php?stop="
STOP="0510"
FONT_SIZE=32
ROW_HEIGHT=35
PARAMS="&font_size=$FONT_SIZE&row_height=$ROW_HEIGHT"
URL="$BASE$STOP$PARAMS"
OUTPATH="/dev/shm/busstop"
CAP="cap-$STOP.png"
TMP="tmp-$STOP.png"
OUT="out-$STOP.png"
PIDFILE="$OUTPATH/lock-$STOP.pid"
#DELAY=5
BANNER_SRC="/usr/share/kivijalka/banners/banner.png"
BANNER="$OUTPATH/banner.png"
BANNER_PAD=10
THUMBNAIL="$OUTPATH/thumb.png"

mkdir -p "$OUTPATH"

# get resolution
#x=$(xrandr -q|egrep "^Screen 0"|sed -r 's|(^.*current )([1-9][0-9]*) x ([1-9][0-9]*),.*$|\2 \3|')
#PX_W="${x%% *}"
#PX_H="${x##* }"
PX_W=1920
PX_H=1080

# defaults if we didn't find a resolution
if [[ -z $PX_W ]]
then
  PX_W=1280
fi
if [[ -z $PX_H ]]
then
  PX_H=1024
fi

if [[ -n "$BANNER" ]]
then
  if [[ ! -f "$BANNER" && -n "$BANNER_SRC" ]]
  then
    cp "$BANNER_SRC" "$BANNER"
  fi

  if [[ -f "$BANNER" ]]
  then
    x=$(identify -format "%w %h\n" "$BANNER")
    if [[ -n $x ]]
    then
      BANNER_W="${x%% *}"
      BANNER_H="${x##* }"
      ((x=BANNER_W+BANNER_PAD))
      ((BANNER_X=PX_W-x))
      ((x=BANNER_H+BANNER_PAD))
      ((BANNER_Y=PX_H-x))
    else
      unset BANNER
    fi
  else
    unset BANNER
  fi
fi

if [[ -f "$PIDFILE" ]]
then
  kill -9 $(< "$PIDFILE") &>/dev/null
  rm -f "$PIDFILE"
fi

if [[ -f "$OUTPATH/$OUT" ]]
then
  rm -f "$OUTPATH/$OUT"
fi

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
  if [[ -n $BANNER ]]
  then
    /usr/bin/composite -geometry "+${BANNER_X}+${BANNER_Y}" \
      "$BANNER" "$OUTPATH/$CAP" "$OUTPATH/$OUT"
  else
    mv "$OUTPATH/$CAP" "$OUTPATH/$OUT"
  fi
  /usr/bin/convert "$OUTPATH/$OUT" -scale 50% "$THUMBNAIL"
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
      if [[ -n $BANNER ]]
      then
        /usr/bin/composite -geometry "+${BANNER_X}+${BANNER_Y}" \
          "$BANNER" "$OUTPATH/$CAP" "$OUTPATH/$TMP" \
        && mv "$OUTPATH/$TMP" "$OUTPATH/$OUT"
      else
        mv "$OUTPATH/$CAP" "$OUTPATH/$OUT"
      fi
      /usr/bin/convert "$OUTPATH/$OUT" -scale 50% "$THUMBNAIL"
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
