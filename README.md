# Enhanced Camera - Unreal Engine Plugin

An easy way to add an RTS/MOBA camera to any project.

## Core Features

- [x] Camera Movement Bounds
- [x] Unit selection
- [x] Follow Target
- [x] Ground Height Adaptation
- [x] Mouse-based movement (Edge Scrolling)
- [x] Key-based movement
- [x] Smooth camera rotation
- [x] Smooth camera zoom
- [x] Reset camera state

**Input Actions:**

- <kbd>W</kbd>,<kbd>S</kbd>,<kbd>A</kbd>,<kbd>D</kbd> or <kbd>Mouse Move</kbd> - Camera movement
- <kbd>MMB</kbd>+<kbd>Mouse Move</kbd> - Camera rotate
- <kbd>Scroll Up</kbd>/<kbd>Scroll Down</kbd> - Camera zoom
- <kbd>Home</kbd> - Reset Camera

## Installation

1. Navigate to your project's directory and create a `Plugins/` folder if it does not exist.
2. Download and extract the archive into the `Plugins/ `folder.
3. Launch your project. Ensure that the `EnhancedCamera` plugin is enabled.
4. Create a new blueprint and inherit from `ECRTSCamera` or `BP_RTSCamera`.
5. Open your blueprint and assign your `InputMappingContext` and `InputActions`, or use the ready-made ones from `EnhancedCamera/Content/Input/`.
6. Add your blueprint to the scene.

## Example

https://github.com/XpycT/EnhancedCamera/assets/120016/050eab40-880f-4eee-adf2-a5948dc8eb87


