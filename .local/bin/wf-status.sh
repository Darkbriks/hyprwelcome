#!/bin/bash

PIDFILE="/tmp/wf-recorder.pid"
STARTFILE="/tmp/wf-recorder.start"

if pgrep -x wf-recorder > /dev/null && [ -f "$STARTFILE" ]; then
    START=$(cat "$STARTFILE")
    NOW=$(date +%s)
    ELAPSED=$((NOW - START))

    MIN=$((ELAPSED / 60))
    SEC=$((ELAPSED % 60))

    printf -v TIME "%02d:%02d" "$MIN" "$SEC"

    echo "{\"text\":\"󰑋 $TIME\",\"class\":\"recording\",\"tooltip\":\"Enregistrement en cours ($TIME)\"}"
else
    echo '{"text":"󰑊","class":"idle","tooltip":"Pas d enregistrement"}'
fi
