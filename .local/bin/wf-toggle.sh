#!/bin/bash

DIR="$HOME/Vidéos/Screenshots"
PIDFILE="/tmp/wf-recorder.pid"
STARTFILE="/tmp/wf-recorder.start"

if pgrep -x wf-recorder > /dev/null; then
    kill -INT $(cat "$PIDFILE") 2>/dev/null
    rm -f "$PIDFILE" "$STARTFILE"
    notify-send "  Enregistrement arrêté"
    dolphin "$DIR" &
else
    mkdir -p "$DIR"
    FILE="$DIR/$(date +%Y%m%d_%H%M%S).mp4"

    notify-send "  Enregistrement démarré"

    wf-recorder -g "$(slurp)" --audio --file "$FILE" &
    echo $! > "$PIDFILE"
    date +%s > "$STARTFILE"
fi
