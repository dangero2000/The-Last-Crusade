/****************************************************************
 *  File: Platform.h                                           *
 *   Desc: Cross-platform type definitions and key constants.  *
 *         Replaces Windows-specific types (WPARAM, VK_*)      *
 *         enabling compilation on Mac and Linux with SDL2.    *
 ****************************************************************/

#pragma once

// On non-Windows, define Windows-compatible types and constants
// that the engine uses for key handling.
#ifndef _WIN32

// Key code type (Windows uses WPARAM = UINT_PTR; we use unsigned int)
typedef unsigned int WPARAM;

// Virtual key codes (must match Windows VK_* values so Game.h switch
// cases work correctly after SDL key translation in MyWin.h)
#define VK_ESCAPE  0x1B
#define VK_SPACE   0x20
#define VK_LEFT    0x25
#define VK_UP      0x26
#define VK_RIGHT   0x27
#define VK_DOWN    0x28

#else
// On Windows, pull the real definitions from the SDK.
// SDL2 on Windows does not include windows.h automatically.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
