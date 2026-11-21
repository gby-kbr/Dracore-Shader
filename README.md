# Dracore Shader

A balanced and lightweight shader for Minecraft Bedrock (RenderDragon).

## Building

The pack is compiled for Android, iOS and Windows at once (`multiplatform`).
The Windows (HLSL) part can only be compiled on Windows, so releases are built by the
GitHub workflow.

- **GitHub:** Actions → `build` → *Run workflow*. It only runs when started by hand,
  nothing is built when files are pushed. The `.mcpack` is attached to the run.
- **Locally on Windows:** `pip install -r requirements.txt`, then `build.bat setup` and `build.bat build`.
- **Locally on Linux/macOS/Termux:** `./build.sh setup`, then `./build.sh check` tests
  Android and iOS only (no files are made).

`build` audits the sources first (every function and config define is used, every
header is included, HLSL rules) and then compiles. The version comes from `mcpack/manifest.json`.

## Layout

- `src/dracore/` config and functions
- `src/materials/` the materials
- `mcpack/` pack files that are copied as they are (manifest, textures, End fog and biome)
- `script/` generators for the textures (`generate_caustic_noise.py` makes the caustics, `upscale_textures.py` makes the weather and particle textures from the Newb sources)
- `tool/` the builder

## Credits

- The sRGB to linear texture decode and the linear to sRGB output encode in `tonemap.h` follow the approach shown by the creator of IzH RD Shader (CurseForge). The formulas themselves are the standard sRGB transfer functions.
- ACES film curve: Krzysztof Narkowicz.
- Weather and particle textures, and the idea of an analytic sky: Newb (devendrn). Caustics and tooling: Bedroalism.
