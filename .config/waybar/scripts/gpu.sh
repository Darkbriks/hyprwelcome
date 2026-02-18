#!/usr/bin/env bash
# ============================================================================
# gpu.sh — Affichage cyclique GPU AMD pour Waybar
# Modes : usage (%) → vram (Mo) → temp (°C) → retour usage
# Click gauche : passe au mode suivant
# ============================================================================

STATE_FILE="/tmp/waybar-gpu-mode"

# ─── Détection automatique du bon card AMD ────────────────────────
find_amd_card() {
    for card in /sys/class/drm/card*/; do
        local vendor_file="${card}device/vendor"
        if [[ -r "$vendor_file" ]] && grep -q "0x1002" "$vendor_file" 2>/dev/null; then
            echo "${card}device"
            return
        fi
    done
    echo ""
}

GPU_PATH=$(find_amd_card)

if [[ -z "$GPU_PATH" ]]; then
    echo '{"text": "󰍛 N/A", "tooltip": "GPU AMD introuvable", "class": "gpu-error"}'
    exit 0
fi

# Initialisation de l'état si absent
if [[ ! -f "$STATE_FILE" ]]; then
    echo "usage" > "$STATE_FILE"
fi

# Changement de mode sur click
if [[ "$1" == "cycle" ]]; then
    current=$(cat "$STATE_FILE")
    case "$current" in
        usage) echo "vram"  > "$STATE_FILE" ;;
        vram)  echo "temp"  > "$STATE_FILE" ;;
        temp)  echo "usage" > "$STATE_FILE" ;;
    esac
    exit 0
fi

MODE=$(cat "$STATE_FILE")

# ─── Lecture des métriques via sysfs ──────────────────────────────

read_usage() {
    local busy="$GPU_PATH/gpu_busy_percent"
    [[ -r "$busy" ]] && cat "$busy" || echo "N/A"
}

read_vram() {
    local used="$GPU_PATH/mem_info_vram_used"
    local total="$GPU_PATH/mem_info_vram_total"
    if [[ -r "$used" && -r "$total" ]]; then
        local used_mb=$(( $(cat "$used") / 1024 / 1024 ))
        local total_mb=$(( $(cat "$total") / 1024 / 1024 ))
        echo "${used_mb}/${total_mb}"
    else
        echo "N/A"
    fi
}

read_temp() {
    for hwmon in /sys/class/hwmon/hwmon*/; do
        if grep -q "amdgpu" "${hwmon}name" 2>/dev/null; then
            local temp_file="${hwmon}temp1_input"
            [[ -r "$temp_file" ]] && echo "$(( $(cat "$temp_file") / 1000 ))" && return
        fi
    done
    echo "N/A"
}

# ─── Formatage de la sortie Waybar (JSON) ─────────────────────────
case "$MODE" in
    usage)
        value=$(read_usage)
        text="󰍛 ${value}%"
        tooltip="GPU — Utilisation\nClic pour VRAM →"
        ;;
    vram)
        value=$(read_vram)
        text="󰘚 ${value} Mo"
        tooltip="GPU — VRAM (utilisée/totale)\nClic pour Température →"
        ;;
    temp)
        value=$(read_temp)
        text="󰔄 ${value}°C"
        tooltip="GPU — Température\nClic pour Utilisation →"
        ;;
esac

echo "{\"text\": \"$text\", \"tooltip\": \"$tooltip\", \"class\": \"gpu-$MODE\"}"
