# scop
A simple graphics `.obj` renderer, made with Vulkan.

## Usage
`./scop filepath {texturepath}`

You can press `escape` to close the window.

**Texture enabling keys**:
- `t`	toggles/untoggles texture

**Object moving keys**
- `w`		forward
- `s`		backward
- `a`		left hand side
- `d`		right hand side
- `ctrl`	downward
- `space`	upward

**Rotation keys**:
- `1`	around X axis of the object (`7` to rotate counter clockwise)
- `2`	around Y axis of the object (`8` to rotate counter clockwise)
- `3`	around Z axis of the object (`9` to rotate counter clockwise)

**Reset model edits**
- `r`	reset model

**Camera orientation keys**
- `tab`	camera orientation: +90°, 0° or -90°

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