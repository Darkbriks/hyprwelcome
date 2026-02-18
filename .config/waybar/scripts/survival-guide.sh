#!/usr/bin/env bash
# ============================================================================
# survival-guide.sh — Ouvre le guide de survie dans une fenêtre Kitty flottante
# Nécessite : glow (renderer Markdown terminal)
#   yay -S glow
# ============================================================================

GUIDE="$HOME/.config/hypr/SURVIVAL_GUIDE.md"

if [[ ! -f "$GUIDE" ]]; then
    echo "Guide introuvable : $GUIDE" >&2
    exit 1
fi

kitty --class survival-guide \
      --title "Guide de survie — Hyprland" \
      --override font_size=13 \
      sh -c "glow -p '$GUIDE'"