#!/bin/bash

CFGFILE="/etc/kivijalka.conf"

unset KIVIJALKA_ADDR KIVIJALKA_PORT KIVIJALKA_RUNPATH KIVIJALKA_CACHEPATH \
      KIVIJALKA_DISPLAY_WIDTH KIVIJALKA_DISPLAY_HEIGHT KIVIJALKA_STOP \
      KIVIJALKA_FONT_SIZE KIVIJALKA_ROW_HEIGHT

if [[ -f "$CFGFILE" ]]
then
  . "$CFGFILE"
fi

if [[ -n $KIVIJALKA_ADDR ]]
then
  ADDR="$KIVIJALKA_ADDR"
else
  ADDR='127.0.0.1'
fi

if [[ -n $KIVIJALKA_PORT ]]
then
  PORT="$KIVIJALKA_PORT"
else
  PORT='8001'
fi

if [[ -n $KIVIJALKA_RUNPATH ]]
then
  RUNPATH="$KIVIJALKA_RUNPATH"
else
  RUNPATH='/run/kivijalka'
fi

if [[ -n $KIVIJALKA_CACHEPATH ]]
then
  CACHEPATH="$KIVIJALKA_CACHEPATH"
else
  CACHEPATH='/var/cache/kivijalka'
fi

if [[ -n $KIVIJALKA_DISPLAY_WIDTH ]] &&
   (( KIVIJALKA_DISPLAY_WIDTH > 0 ))
then
  PX_W="$KIVIJALKA_DISPLAY_WIDTH"
else
  PX_W=''
fi

if [[ -n $KIVIJALKA_DISPLAY_HEIGHT ]] &&
   (( KIVIJALKA_DISPLAY_HEIGHT > 0 ))
then
  PX_H="$KIVIJALKA_DISPLAY_HEIGHT"
else
  PX_H=''
fi

if [[ -n $KIVIJALKA_STOP ]]
then
  STOP="$KIVIJALKA_STOP"
else
  STOP='1'
fi

if [[ -n $KIVIJALKA_FONT_SIZE ]]
then
  FONT_SIZE="$KIVIJALKA_FONT_SIZE"
else
  FONT_SIZE=''
fi

if [[ -n $KIVIJALKA_ROW_HEIGHT ]]
then
  ROW_HEIGHT="$KIVIJALKA_ROW_HEIGHT"
else
  ROW_HEIGHT=''
fi

BASE="http://lissu.tampere.fi/monitor.php?stop="
OUTPATH="/dev/shm/kivijalka"
CAPFILE="$OUTPATH/cap.png"
OUTFILE="$OUTPATH/out.png"
PIDFILE="$RUNPATH/pid"

mkdir -p "$OUTPATH"
mkdir -p "$CACHEPATH"
mkdir -p "$RUNPATH"

if [[ -f "$PIDFILE" ]]
then
  kill -9 $(< "$PIDFILE") &>/dev/null
  rm -f "$PIDFILE"
fi

(( FONT_SIZE >= 30 && FONT_SIZE <= 50 )) || FONT_SIZE=''
(( ROW_HEIGHT >= 20 && ROW_HEIGHT <= 80 )) || ROW_HEIGHT=''
(( PX_W > 0 )) || PX_W=1280
(( PX_H > 0 )) || PX_H=1024

# stop number
if [[ -f "$RUNPATH/stop" ]]
then
  TMP="$(< "$RUNPATH/stop")"
  [[ $TMP == $STOP ]] || (( TMP < 1 )) || STOP="$TMP"
fi

# font params
if [[ -f "$RUNPATH/font" ]]
then
  TMP="$(< "$RUNPATH/font")"
  TMP2="$(grep -o ',[0-9]*' <<< "$TMP")"
  TMP2="${TMP2##*,}"
  TMP="${TMP%%,*}"
  (( TMP == FONT_SIZE || TMP < 30 || TMP > 50 )) || (( FONT_SIZE=TMP ))
  (( TMP2 == ROW_HEIGHT || TMP2 < 20 || TMP2 > 80 )) || (( ROW_HEIGHT=TMP2 ))
fi

# display geometry
if [[ -f "$RUNPATH/disp" ]]
then
  TMP="$(< "$RUNPATH/disp")"
  TMP2="$(grep -o 'x[0-9]*' <<< "$TMP")"
  TMP2="${TMP2##*x}"
  TMP="${TMP%%x*}"
  (( TMP == PX_W || TMP < 1 )) || (( PX_W=TMP ))
  (( TMP2 == PX_H || TMP2 < 1 )) || (( PX_H=TMP2 ))
fi

URL="$BASE$STOP${FONT_SIZE:+&font_size=$FONT_SIZE}${ROW_HEIGHT:+&row_height=$ROW_HEIGHT}"

cutycapt --url="$URL" \
  --private-browsing=on \
  --javascript=off \
  --java=off \
  --plugins=off \
  --auto-load-images=on \
  --min-width=$PX_W \
  --min-height=$PX_H \
  --out="$CAPFILE"

if [[ -f "$CAPFILE" ]]; then
  cp "$CAPFILE" "$OUTFILE"
fi

(
  echo $BASHPID > "$PIDFILE"
  while true
  do
    # stop number
    if [[ -f "$RUNPATH/stop" ]]
    then
      TMP="$(< "$RUNPATH/stop")"
      [[ $TMP == $STOP ]] || (( TMP < 1 )) || STOP="$TMP"
    fi

    if [[ -f "$RUNPATH/font" ]]
    then
      TMP="$(< "$RUNPATH/font")"
      TMP2="$(grep -o ',[0-9]*' <<< "$TMP")"
      TMP2="${TMP2##*,}"
      TMP="${TMP%%,*}"
      (( TMP == FONT_SIZE || TMP < 30 || TMP > 50 )) || (( FONT_SIZE=TMP ))
      (( TMP2 == ROW_HEIGHT || TMP2 < 20 || TMP2 > 80 )) || (( ROW_HEIGHT=TMP2 ))
    fi

    if [[ -f "$RUNPATH/disp" ]]
    then
      TMP="$(< "$RUNPATH/disp")"
      TMP2="$(grep -o 'x[0-9]*' <<< "$TMP")"
      TMP2="${TMP2##*x}"
      TMP="${TMP%%x*}"
      (( TMP == PX_W || TMP < 1 )) || (( PX_W=TMP ))
      (( TMP2 == PX_H || TMP2 < 1 )) || (( PX_H=TMP2 ))
    fi

    (( FONT_SIZE >= 30 && FONT_SIZE <= 50 )) || FONT_SIZE=''
    (( ROW_HEIGHT >= 20 && ROW_HEIGHT <= 80 )) || ROW_HEIGHT=''

    URL="$BASE$STOP${FONT_SIZE:+&font_size=$FONT_SIZE}${ROW_HEIGHT:+&row_height=$ROW_HEIGHT}"

    if cutycapt --url="$URL" \
         --private-browsing=on \
         --javascript=off \
         --java=off \
         --plugins=off \
         --auto-load-images=on \
         --min-width=$PX_W \
         --min-height=$PX_H \
         --out="$CAPFILE"
    then
      sleep 5
    fi
  done
) &>/dev/null &

(
  wsserver -c "$CAPFILE" -o "$OUTFILE" -b "$CACHEPATH" \
           -g "${PX_W}x${PX_H}" -a "$ADDR" -p "$PORT"
) &>/dev/null &

xset -dpms
xset s off

while true; do
  /usr/bin/qiv -fiRT "$OUTFILE"
  sleep 5
done

if [[ -f "$PIDFILE" ]]
then
  kill -9 $(< "$PIDFILE") &>/dev/null
  rm -f "$PIDFILE"
fi
