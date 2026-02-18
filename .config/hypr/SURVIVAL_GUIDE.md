# 󰣇 Guide de survie — Hyprland

## Raccourcis essentiels

### Fenêtres
| Raccourci | Action                            |
|-----------|-----------------------------------|
| `Super + Entrée` | Terminal (Kitty)                  |
| `Super + R` | Lanceur d'apps (Rofi)             |
| `Super + E` | HyprWelcome                       |
| `Super + Shift + E` | Gestionnaire de fichiers          |
| `Super + C` | Fermer la fenêtre active          |
| `Super + V` | Flottant / Tiling                 |
| `Super + F` | Plein écran                       |
| `Super + Shift + F` | Faux plein écran (garde les gaps) |
| `Super + P` | Pseudotile (dwindle)              |

### Navigation
| Raccourci | Action |
|-----------|--------|
| `Super + ←/→/↑/↓` | Changer de fenêtre active |
| `Super + Shift + ←/→/↑/↓` | Déplacer la fenêtre |
| `Super + Ctrl + ←/→/↑/↓` | Redimensionner |
| `Super + clic gauche` | Déplacer fenêtre flottante |
| `Super + clic droit` | Redimensionner fenêtre flottante |

### Workspaces
| Raccourci | Action |
|-----------|--------|
| `Super + &` | Workspace 1 — neutre |
| `Super + é` | Workspace 2 — code |
| `Super + "` | Workspace 3 — browser |
| `Super + '` | Workspace 4 — communication |
| `Super + Tab` | Workspace suivant |
| `Super + Shift + Tab` | Workspace précédent |
| `Super + Shift + &/é/"/'` | Envoyer fenêtre vers workspace |
| `Super + S` | Scratchpad (toggle) |
| `Super + Shift + S` | Envoyer vers scratchpad |

### Screenshots
| Raccourci | Action |
|-----------|--------|
| `Impr. écran` | Capturer une zone → presse-papier |
| `Shift + Impr. écran` | Écran entier → presse-papier |
| `Super + Impr. écran` | Écran entier → fichier |

### Média & volume
| Raccourci | Action |
|-----------|--------|
| Touches média | Lecture / Pause / Suivant / Précédent |
| Touches volume | Volume +/- 5%, Mute |

---

## Waybar

- **Clic gauche sur GPU** — change la métrique affichée (usage → VRAM → temp)
- **Clic gauche sur l'heure** — ouvre HyprWelcome
- **Clic gauche sur le volume** — ouvre pavucontrol
- **Clic molette sur le volume** — volume +/- 2%
- **Clic molette sur les workspaces** — navigate

---

## HyprWelcome — Actions disponibles

- Mise à jour système (`yay -Syu`)
- Nettoyage cache (`yay -Sc`)
- Mise à jour des miroirs (reflector)
- Switch de thème
- Recharger la config Hyprland

---

## Fichiers de config

| Fichier                                    | Rôle |
|--------------------------------------------|------|
| `~/.config/hypr/hyprland.conf`             | Config principale Hyprland |
| `~/.config/hypr/theme.conf`                | Couleurs actives (généré par HyprWelcome) |
| `~/.config/hyprwelcome/config.toml`        | Config HyprWelcome + actions |
| `~/.config/hyprwelcome/themes/*/colors.toml` | Palette d'un thème |
| `~/.config/waybar/config.jsonc`            | Config Waybar |
| `~/.config/waybar/style.css`               | Style Waybar |
| `~/.config/kitty/kitty.conf`               | Config terminal |
| `~/.config/mako/config`                    | Config notifications |

---

## Commandes utiles

```bash
hyprctl reload          # Recharger la config Hyprland
hyprctl clients         # Lister les fenêtres ouvertes (classes, titres)
hyprctl monitors        # Infos moniteurs
pkill -SIGUSR2 waybar   # Recharger Waybar
makoctl reload          # Recharger Mako
```

---

*Appuie sur `q` pour fermer, flèches pour naviguer*
