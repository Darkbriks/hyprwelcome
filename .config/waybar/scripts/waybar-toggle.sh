#!/usr/bin/env bash
# ============================================================================
# waybar-toggle.sh — Contrôle de la visibilité et du mode de Waybar
#
# Usage :
#   waybar-toggle.sh visibility   → masque/affiche la barre
#   waybar-toggle.sh exclusive    → toggle fenêtres sous/au-dessus de la barre
# ============================================================================

CONFIG="$HOME/.config/waybar/config.jsonc"
STATE_EXCL="/tmp/waybar-exclusive"

case "$1" in

    # ─── Masquer / Afficher ──────────────────────────────────────
    visibility)
        if pgrep -x waybar > /dev/null; then
            pkill waybar
        else
            waybar &
        fi
        ;;

    # ─── Toggle exclusive (fenêtres passent sous la barre ou non) ─
    exclusive)
        if [[ ! -f "$STATE_EXCL" ]]; then
            # État initial : exclusive = true → on passe à false
            sed -i 's/"exclusive": true/"exclusive": false/' "$CONFIG"
            touch "$STATE_EXCL"
            notify-send "Waybar" "Fenêtres passent sous la barre" \
                --icon=preferences-desktop --expire-time=2000
        else
            # Retour à exclusive = true
            sed -i 's/"exclusive": false/"exclusive": true/' "$CONFIG"
            rm "$STATE_EXCL"
            notify-send "Waybar" "Barre réserve son espace" \
                --icon=preferences-desktop --expire-time=2000
        fi
        pkill -SIGUSR2 waybar   # reload sans fermer
        ;;

    *)
        echo "Usage: $0 {visibility|exclusive}"
        exit 1
        ;;
esac
