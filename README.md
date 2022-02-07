[![build](https://github.com/beauregardes/baphomet/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/beauregardes/baphomet/actions/workflows/c-cpp.yml)

# Baphomet

### What is it?

Baphomet is a 2D graphics engine for modern C++ (20+). I wasn't satisfied with the options that were available, epsecially the many that rely on either (1) deprecated OpenGL functionality, a.k.a. the fixed function pipeline, or (2) were written for C and therefore feel very unnatural to use from modern C++.

### What can it do?

A non-exhaustive list of existing/planned features (subject to change as Baphomet is still in very early development)

- [x] Windowing à la GLFW, supports regular windowed, fullscreen, and borderless/windowed fullscreen
    - [ ] Bind keys to names
    - [x] Key repeats/intervals/delay
    - [ ] Input sequences
- [x] Timers
- [ ] Render targets/viewports
- [x] Primitive support: pixels/points, lines, triangles, rectangles, and ovals
    - [x] Filled
    - [ ] Lined
- [x] Robust color support: RGB, HSL, HSV, XYZ, L\*ab, and L\*Ch°
    - [x] CSS colors (in case you want those for some reason ¯\\\_(ツ)\_/¯)
    - [ ] Generative palettes
    - [ ] Color adjustments (lighten, darken, invert, etc.)
- [x] Textures, offers nearest neighbor or linear scaling to handle different types of games
- [x] Spritesheets
    - [x] Defined in code
    - [ ] Load from specialized INI format (see [docs (NYI)]())
- [ ] Text rendering
    - [x] Code Page 437 bitmaps
    - [ ] TTF
- [ ] Particle systems
- [ ] Audio
- [ ] GUI toolkit

### Credits

- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [Glad](https://github.com/Dav1dde/glad/tree/glad2)
- [{fmt}](https://github.com/fmtlib/fmt)
- [libnyquist](https://github.com/ddiakopoulos/libnyquist)
- [OpenAL soft](https://github.com/kcat/openal-soft)
- [spdlog](https://github.com/gabime/spdlog)
- [stb](https://github.com/nothings/stb)
- [pcg-cpp](https://github.com/imneme/pcg-cpp)