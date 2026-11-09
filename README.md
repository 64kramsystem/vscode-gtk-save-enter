# vscode-gtk-save-enter

Restores Enter-to-save in VS Code's native GTK 3 Save As dialog. Workaround for [electron/electron#54670](https://github.com/electron/electron/issues/54670).

Requires a C compiler, Make, pkg-config, and GTK 3 development headers (`libgtk-3-dev` on Ubuntu).

```sh
git clone https://github.com/64kramsystem/vscode-gtk-save-enter.git ~/local/vscode-gtk-save-enter
cd ~/local/vscode-gtk-save-enter
make
mkdir -p ~/.local/bin
ln -s "$PWD/code" ~/.local/bin/code
```

Ensure `~/.local/bin` comes before `/usr/bin` in `PATH`. Quit VS Code completely, then run `code`. Child processes inherit `GTK3_MODULES`, but the module only installs its hook when the executable is named `code`. `/usr/bin/code` remains available without the launcher.

The module replaces Chromium's custom Save response with `GTK_RESPONSE_ACCEPT`, restoring the default button and overwrite confirmation. Open dialogs are untouched. Tested with VS Code 1.145.2 / Electron 43.8.3 / GTK 3.24.41.

Only the native GTK 3 chooser is supported; GTK 4 and XDG portal dialogs are unaffected.
