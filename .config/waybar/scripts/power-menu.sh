#!/usr/bin/env bash
# ============================================================================
# power-menu.sh — Menu d'alimentation via Rofi
# ============================================================================

LOCK="󰌾  Verrouiller"
SUSPEND="󰤄  Veille"
REBOOT="󰑓  Redémarrer"
SHUTDOWN="󰐥  Éteindre"
LOGOUT="󰍃  Déconnexion"

CHOICE=$(printf '%s\n' "$LOCK" "$SUSPEND" "$LOGOUT" "$REBOOT" "$SHUTDOWN" \
    | rofi -dmenu \
           -p "Alimentation" \
           -theme-str 'window { width: 220px; }' \
           -theme-str 'listview { lines: 5; }')

case "$CHOICE" in
    "$LOCK")     hyprlock ;;
    "$SUSPEND")  systemctl suspend ;;
    "$REBOOT")   systemctl reboot ;;
    "$SHUTDOWN")  systemctl poweroff ;;
    "$LOGOUT")   hyprctl dispatch exit ;;
esac