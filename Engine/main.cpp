/****************************************************************
 *   Prog: The Last Crusade										*
 *     By: Peter S. VanLund									*
 *   Desc: All code written by Peter S. VanLund.				*
 *         Gameplay design, story, & sounds by Patrick Dwyer.	*
 *         Cross-platform port using SDL2 (Windows/Mac/Linux)	*
 ****************************************************************/

#include "Game.h"
#include <SDL2/SDL.h>
#include <cstring>
#include <cstdio>
#ifdef _WIN32
#include <direct.h>
#define CHDIR _chdir
#else
#include <unistd.h>
#define CHDIR chdir
#endif

// Translate SDL2 key event to engine key code.
// Letters arrive as lowercase from SDL; convert to uppercase so the
// engine's switch/case comparisons (e.g. case 'Y':) work correctly.
static WPARAM translateKey(SDL_Keycode sym)
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
			if(sym >= SDLK_a && sym <= SDLK_z)
				return (WPARAM)(sym - 32); // lowercase -> uppercase
			return (WPARAM)sym;
	}
}

int main(int argc, char* argv[])
{
	// Initialise SDL2 (video needed for window; audio is handled by miniaudio)
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
	{
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	// Change working directory to where the game assets live:
	//   macOS .app  -> Contents/Resources/
	//   Linux/Win   -> directory containing the executable
	// All relative asset paths in the engine resolve from here.
	{
		char *basePath = SDL_GetBasePath();
		if(basePath)
		{
#ifdef __APPLE__
			// SDL_GetBasePath() returns Contents/MacOS/ on macOS; step up
			// one level then into Resources/ to reach the asset directory.
			char resourcePath[1024];
			snprintf(resourcePath, sizeof(resourcePath), "%s../Resources", basePath);
			CHDIR(resourcePath);
#else
			// On Linux/Windows, assets are in the same directory as the executable
			CHDIR(basePath);
#endif
			SDL_free(basePath);
		}
	}

	// Create game object AFTER changing working directory so sounds can be loaded
	Game game;

	// If a map file is passed on the command line, load it as a custom map
	if(argc > 1 && strlen(argv[1]) > 0)
	{
		game.addMap(argv[1]);
		game.useCustom();
	}
	// Otherwise, play the game normally
	else
	{
		game.addMap("forest.map");
		game.addMap("graveyard.map");
		game.addMap("castle.map");
	}

	// Create a minimal window (the game is audio-only; window exists only
	// so the OS gives us a keyboard-focus target)
	SDL_Window *window = SDL_CreateWindow(
		"The Last Crusade",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		400, 200,
		SDL_WINDOW_SHOWN);
	if(!window)
	{
		fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// Draw the title text in the window
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	if(renderer)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	// Start the game (plays intro music / shows main menu)
	game.play();

	// Main event loop: wait for events and forward key presses to the engine
	SDL_Event event;
	while(SDL_WaitEvent(&event))
	{
		if(event.type == SDL_QUIT) break;
		if(event.type == SDL_KEYUP)
			game.processKey(translateKey(event.key.keysym.sym));
	}

	// Cleanup
	if(renderer) SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
