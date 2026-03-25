# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

The Last Crusade is an audio-only RPG game engine. The game has no graphics - all gameplay is communicated through spatial audio and sound effects. Players navigate a fantasy world through keyboard input while the engine narrates events through audio.

## Build System

**Building the project:**
```bash
cmake -S . -B Build
cmake --build Build
```

**Running the game:**
```bash
# macOS
open Build/LastCrusade.app

# Direct execution (for debugging)
Build/LastCrusade.app/Contents/MacOS/LastCrusade
```

**Clean rebuild:**
```bash
rm -rf Build/CMakeCache.txt Build/CMakeFiles Build/cmake_install.cmake Build/Makefile Build/_deps Build/LastCrusade.app
cmake -S . -B Build && cmake --build Build
```

## Architecture

### Header-Only Design
The engine is implemented entirely in header files (`.h`) in the `Engine/` directory. The only `.cpp` file is `main.cpp`, which handles platform initialization and event loop setup.

**Key architectural components:**
- **Game.h**: State machine managing game states (OFF, MENU, MOVE, FIGHT). Loads all global sounds in constructor.
- **Map.h**: Represents game levels/areas with nodes (locations), spatial audio, and items.
- **Player.h**: Player stats (strength, defense, HP, gold, inventory).
- **NPC.h**: Enemy/friend/vendor/leprechaun characters.
- **Sound.h**: Cross-platform audio using miniaudio (2D stereo and 3D spatial audio).
- **MyWin.h**: Platform abstraction layer for keyboard input and event handling.

### State Machine Flow
```
OFF → MENU → MOVE ⇄ FIGHT → (victory/death) → MENU
```

The Game class is a large state machine that processes keyboard input differently based on the current state.

## Critical Implementation Details

### Game Object Initialization Timing
**IMPORTANT:** The `Game` object in `main.cpp` must be instantiated AFTER `SDL_GetBasePath()` is called and the working directory is changed. Initializing it as a global variable (before `main()`) will cause all sound files to fail loading because the working directory won't be set yet.

```cpp
// ❌ WRONG - Game constructor runs before main(), before chdir
Game game;  // Global variable

int main() {
    SDL_Init(...);
    chdir(SDL_GetBasePath());  // Too late!
    // ...
}

// ✅ CORRECT - Game constructor runs after chdir
int main() {
    SDL_Init(...);
    chdir(SDL_GetBasePath());
    Game game;  // Local variable, created after chdir
    // ...
}
```

### Asset Management
Game assets are stored in `Build/` directory (with capital B):
- `Build/*.map` - Map files defining levels
- `Build/sounds/` - Global sound effects (fight.mp3, main_theme.mp3, etc.)
- `Build/forest.sounds/`, `graveyard.sounds/`, `castle.sounds/` - Map-specific spatial audio
- `Build/numbers/` - Number pronunciation audio files

**Platform-specific asset paths:**
- **macOS**: Assets copied to `LastCrusade.app/Contents/Resources/` during build. At runtime, `SDL_GetBasePath()` returns `Contents/Resources/` (not `Contents/MacOS/`).
- **Linux/Windows**: Assets copied next to the executable.

The engine loads sounds using relative paths (`./sounds/fight.mp3`), so the working directory must be set correctly.

### macOS-Specific Build Steps
1. **Bundle fixup**: `cmake/BundleFixup.cmake` copies SDL2 dylibs into the app bundle and rewrites install names.
2. **Code signing**: After modifying dylibs with `install_name_tool`, the app must be re-signed using `codesign --force --deep --sign -` to prevent SIGKILL on launch.
3. **Resources directory**: Must be explicitly created before copying assets (CMake's `copy_if_different` doesn't auto-create directories).

## Dependencies

- **SDL2**: Window creation, keyboard input, and base path resolution. Install via Homebrew: `brew install sdl2`
- **miniaudio**: Single-header audio library (WAV/MP3 support, 3D spatial audio). Auto-downloaded by CMake via FetchContent.
- **Platform frameworks (macOS)**: CoreAudio, AudioToolbox, CoreFoundation

## File Organization

```
Engine/
  main.cpp              - Entry point, SDL initialization, event loop
  Game.h                - Main game state machine (loads global sounds)
  Map.h                 - Level/area representation with nodes
  Sound.h               - Audio playback wrapper (miniaudio)
  Player.h              - Player character with stats/inventory
  NPC.h                 - Non-player characters (enemies, friends, vendors, leprechauns)
  Node.h                - Single location in a map with spatial audio
  Item.h                - Weapons, armor, potions, special items
  MyWin.h               - Platform abstraction (keyboard input)
  NumberReader.h        - Reads numbers aloud using audio files
  Platform.h            - Platform-specific constants (VK_* key codes)

Build/                  - Asset directory (maps, sounds, numbers)
cmake/
  BundleFixup.cmake     - macOS dylib bundling script
```

## Common Patterns

### Adding New Sounds
1. Place audio file in `Build/sounds/` (or appropriate subdirectory)
2. Declare `Sound*` pointer in Game.h private section
3. Initialize in `Game::Game()` constructor: `newSound = new Sound("./sounds/filename.mp3");`
4. Delete in `Game::~Game()`: `delete newSound;`

### Adding New Game States
1. Add enum value to `GameState` in Game.h
2. Update `Game::processKey(WPARAM key)` with new case for the state
3. Create state-specific helper methods as private functions in Game class

### Spatial Audio (3D positioning)
```cpp
// Non-spatial (stereo)
Sound s("./sounds/music.mp3");
s.play(true);  // loop

// Spatial (3D positioned)
Sound s("./sounds/effect.wav", x, y);
s.play(false);

// Update listener position
Sound::setListener(playerX, playerY);
```

## Platform Considerations

- **Keyboard codes**: Use `VK_*` constants from Platform.h (e.g., `VK_UP`, `VK_SPACE`). SDL keycodes are translated in `main.cpp:translateKey()`.
- **Working directory**: Must be set to asset directory before Game object creation.
- **File paths**: Always use forward slashes (`/`) in paths, even on Windows.
- **Audio formats**: MP3 and WAV supported by miniaudio's built-in decoders.

## Debugging Audio Issues

If sounds don't play:
1. Check stderr output - Sound.h logs all load failures with error codes
2. Verify working directory: Add `fprintf(stderr, "cwd=%s\n", getcwd(...))`
3. Confirm files exist: `ls -la` the asset directories in the bundle
4. Ensure Game object created after `chdir()` in main.cpp
5. Check miniaudio error codes: `-7` = MA_DOES_NOT_EXIST (file not found)
