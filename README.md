# scop
A simple graphics `.obj` renderer, made with Vulkan.

## Usage
`./scop filepath {texturepath}`

You can press `escape` to close the window.

**Texture enabling keys**:
- `Space`	toggles/untoggles texture

**Rotation keys**:
- `1`	around X axis of the object
- `2`	around Y axis of the object
- `3`	around Z axis of the object
- `4`	disable rotation

**Camera orientation keys**
- `q`	camera orientation: -90°
- `w`	camera orientation: default
- `e`	camera orientation: +90°

**Zoom scroll**
- `up/down`	zoom in/out
- `press`	reset zoom

## Examples

**Basic cube**
![Basic cube, with texture](./resource/basic_cube.png)

**Color toggled**
![Basic cube, with color](./resource/basic_cube_color.png)

**Rotation toggled**
![Basic cube, with texture and rotating](./resource/basic_cube_rot.png)

**Color toggled while rotating**
![Basic cube, with color transition](./resource/basic_cube_rot_trans.png)

