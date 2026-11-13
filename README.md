# Dracore-Shader
Render Dragon shaders for Bedrock that are balanced and lightweight for users

## Features
- Sky colors with a smooth gradient from horizon to zenith, dawn color only around the sun
- Newb torch color for all light-emitting blocks
- Lightweight height fog with a low ground mist
- Realistic, calm water that replaces the vanilla water, with sun and moon reflection
- Reinhard-Jodie color correction with exposure and gamma

All settings are in `src/dracore/config.h`.

## Project structure

```
├── build.bat / build.sh   # build scripts (Windows, Linux/Android)
├── requirements.txt
├── include/               # shared Render Dragon headers
├── licenses/              # licenses of the code this project builds on
├── src/
│   ├── dracore/           # shader functions and config (include with <dracore/main.sh>)
│   └── materials/         # one folder per material, project.json = compiler config
└── tools/build.py         # build tool
```

## Building

Requires Python 3.9+.

```
pip install -r requirements.txt
```

Windows:

```
build.bat setup          # first time only, downloads shaderc and the base materials
build.bat multiplatform  # Android + iOS + Windows in one material.bin
build.bat RenderChunk    # a single material
build.bat clean
```

Linux/Android (Termux): the same, with `./build.sh` instead of `build.bat`.

The `.material.bin` files are written to `build/<platform>/`. Running without arguments opens an interactive menu.

## Credits
- [kren-0x/RenderDragon-Shader-Template](https://github.com/kren-0x/RenderDragon-Shader-Template): material sources, include files and build tool (MIT)
- [devendrn/newb-x-mcbe](https://github.com/devendrn/newb-x-mcbe): `material.json`, `uniforms/`, detection, sky colors, torch color and the water idea (MIT)
