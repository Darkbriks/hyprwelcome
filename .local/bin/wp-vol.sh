#!/bin/bash

STEP=5

case "$1" in
  up)
    wpctl set-volume -l 1 @DEFAULT_AUDIO_SINK@ ${STEP}%+
    ;;
  down)
    wpctl set-volume @DEFAULT_AUDIO_SINK@ ${STEP}%-
    ;;
  mute)
    wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle
    ;;
esac

VOLUME=$(wpctl get-volume @DEFAULT_AUDIO_SINK@ | awk '{print int($2*100)}')
MUTED=$(wpctl get-volume @DEFAULT_AUDIO_SINK@ | grep -o MUTED)

if [ "$MUTED" = "MUTED" ] || [ "$VOLUME" -eq 0 ]; then
  ICON=" "
elif [ "$VOLUME" -lt 30 ]; then
  ICON=" "
elif [ "$VOLUME" -lt 70 ]; then
  ICON=" "
else
  ICON=" "
fi

if [ "$1" = "mute" ]; then
  URGENCY="critical"
else
  URGENCY="normal"
fi

notify-send \
  -h string:x-canonical-private-synchronous:volume \
  -h int:value:$VOLUME \
  -a 'wp-vol' \
  -u $URGENCY \
  "$ICON  Volume : ${VOLUME}%"
