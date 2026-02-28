## Required packages for hyprland dotfiles

- hyprland
- hyprpaper
- hyprlock
- hypridle
- hyprpicker
- waybar
- rofi
- wl-clipboard
- cliphist
- grim
- slurp
- grimblast
- gradi
- wf-recorder
- brightnessctl
- qt6-wayland
- xdg-desktop-portal-hyprland
- kitty
- fish
- starship
- lsd
- fastfetch
- btop
- playerctl
- pipewire
- wireplumber
- pavucontrol
- ttf-jetbrains-mono-nerd
- papirus-icon-theme
- breeze-cursors
- glow
- blueman

## Build and install hyprpanel

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
cmake --install build
```