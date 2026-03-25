/****************************************************************
 *  Class: MyWin												*
 *     By: Peter S. VanLund									*
 *   Desc: Cross-platform input routines using SDL2.			*
 *         Replaces Windows-specific Win32 message handling.	*
 ****************************************************************/

#include <SDL2/SDL.h>
#include "Platform.h"

// Translate an SDL2 keycode to the platform-neutral WPARAM key code
// used throughout the engine (matches Windows VK_* values).
static WPARAM translateSDLKey(SDL_Keycode sym)
{
	switch(sym)
	{
		case SDLK_UP:     return VK_UP;
		case SDLK_DOWN:   return VK_DOWN;
		case SDLK_LEFT:   return VK_LEFT;
		case SDLK_RIGHT:  return VK_RIGHT;
		case SDLK_SPACE:  return VK_SPACE;
		case SDLK_ESCAPE: return VK_ESCAPE;
		default:
			// SDL uses lowercase letters; convert to uppercase ASCII
			if(sym >= SDLK_a && sym <= SDLK_z)
				return (WPARAM)(sym - 32);
			return (WPARAM)sym;
	}
}

class MyWin
{
	public:
		static void DoEvents();		// Process pending SDL events
		static WPARAM getKey();		// Get keystroke
		static WPARAM getYorN();	// Get 'Y' or 'N' keystrokes
		static WPARAM getYorNRun();	// Get 'Y'('R') or 'N'('A') keystrokes
		static WPARAM get123();		// Get '1', '2', or '3' keystrokes
};

// Process pending events (keeps the app responsive, discards key events
// so they don't interfere with the engine's explicit key reads)
void MyWin::DoEvents()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT) exit(0);
		// Non-key events are handled above; key events are intentionally
		// discarded here (callers use getKey() when they want input)
	}
}

// Get keystroke (non-blocking; returns 0 if no key is pending)
WPARAM MyWin::getKey()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT) exit(0);
		if(event.type == SDL_KEYUP)
			return translateSDLKey(event.key.keysym.sym);
	}
	return 0;
}

// Get 'Y' or 'N' keystrokes (blocking)
WPARAM MyWin::getYorN()
{
	WPARAM key = 0;
	while(key!='Y' && key!='N')
	{
		SDL_Delay(1);
		key = getKey();
	}
	return key;
}

// Get 'Y'('R') or 'N'('A') keystrokes (blocking)
WPARAM MyWin::getYorNRun()
{
	WPARAM key = 0;
	while(key!='Y' && key!='N')
	{
		SDL_Delay(1);
		key = getKey();
		key = (key=='R') ? 'Y' : (key=='A') ? 'N' : key;
	}
	return key;
}

// Get '1', '2', or '3' keystrokes (blocking)
WPARAM MyWin::get123()
{
	WPARAM key = 0;
	while(key!='1' && key!='2' && key!='3')
	{
		SDL_Delay(1);
		key = getKey();
	}
	return key;
}
