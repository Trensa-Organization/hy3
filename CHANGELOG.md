# Changelog

## hl0.37.1 and before

- Added `no_gaps_when_only = 2`
- Fixed fullscreen not working on workspaces with only floating windows

## hl0.36.0 and before

- Implement `resizeactivewindow` for floating windows
- Fully implement `resizeactivewindow` for tiled windows
- `hy3:resizenode` added, drop-in replacement for `resizeactivewindow` applied to a whole node.
- Implement keyboard-based focusing for floating windows
- Implement keyboard-based movement for floating windows
  - Add configuration `kbd_shift_delta` providing delta [in pixels] for shift
## hl0.35.0 and before

- Fixed `hy3:killactive` and `hy3:movetoworkspace` not working in fullscreen.
- `hy3:movetoworkspace` added to move a whole node to a workspace.
- Newly tiled windows (usually from moving a window to a new workspace) are now
placed relative to the last selected node.

## hl0.34.0 and before
*check commit history*
