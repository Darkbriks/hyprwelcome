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
        usage)    echo "vram_gb"  > "$STATE_FILE" ;;
        vram)  echo "temp"  > "$STATE_FILE" ;;
        vram_gb)  echo "temp"  > "$STATE_FILE" ;;
        vram_mb)  echo "temp"  > "$STATE_FILE" ;;
        temp)     echo "usage" > "$STATE_FILE" ;;
    esac
    exit 0
fi

MODE=$(cat "$STATE_FILE")

# ─── Lecture des métriques via sysfs ──────────────────────────────

read_usage() {
    local busy="$GPU_PATH/gpu_busy_percent"
    [[ -r "$busy" ]] && cat "$busy" || echo "N/A"
}

read_used_vram() {
    local used="$GPU_PATH/mem_info_vram_used"
    [[ -r "$used" ]] && echo "$(( $(cat "$used") / 1024 / 1024 ))" || echo "N/A"
}

read_total_vram() {
    local total="$GPU_PATH/mem_info_vram_total"
    [[ -r "$total" ]] && echo "$(( $(cat "$total") / 1024 / 1024 ))" || echo "N/A"
}

read_used_vram_gb() {
    local used="$GPU_PATH/mem_info_vram_used"
    [[ -r "$used" ]] && printf "%.1f" "$(( $(cat "$used") / 1024 / 1024 / 1024 ))" || echo "N/A"
}

read_total_vram_gb() {
    local total="$GPU_PATH/mem_info_vram_total"
    [[ -r "$total" ]] && printf "%.1f" "$(( $(cat "$total") / 1024 / 1024 / 1024 ))" || echo "N/A"
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
        tooltip="Utilisation : ${value}%"
        ;;
    vram_mb)
        value=$(read_used_vram)
        text="󰘚 ${value}M"
        total=$(read_total_vram)
        tooltip="VRAM : ${value}M / ${total}M"
        ;;
    vram_gb)
        value=$(read_used_vram_gb)
        text="󰘚 ${value}G"
        total=$(read_total_vram_gb)
        tooltip="VRAM : ${value}G / ${total}G"
        ;;
    temp)
        value=$(read_temp)
        text="󰔄 ${value}°C"
        tooltip="Température : ${value}°C"
        ;;
esac

echo "{\"text\": \"$text\", \"tooltip\": \"$tooltip\", \"class\": \"gpu-$MODE\"}"
