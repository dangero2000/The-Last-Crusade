/********************************************************
 *  Class: Sound										*
 *     By: Peter S. VanLund								*
 *   Desc: Contains all methods for creating & playing	*
 *         sounds (spatial or traditional stereo).		*
 *         Uses miniaudio for cross-platform audio		*
 *         (Windows, Mac, Linux) with WAV/MP3 support	*
 *         and 3D spatial audio.						*
 ********************************************************/

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "MyWin.h"

using namespace std;

class Sound
{
	private:
		ma_sound maSound;					// miniaudio sound object
		bool     loaded;					// Whether sound loaded successfully
		int posX, posY;						// Position of sound
		bool spatial;						// Spatial boolean
		static int listenerX, listenerY;	// Listener location
		string filepath;					// Path to sound file
		static ma_engine engine;			// Shared audio engine (one per process)
		static bool engineInited;			// Whether engine has been initialized
	public:
		Sound();							// Blank constructor
		Sound(string);						// Constructor taking path
		Sound(string,int,int);				// Constructor taking path & position
		~Sound();							// Destructor
		static void init();					// Initialize audio engine
		static void shutdown();				// Shutdown audio engine
		static void setListener(int,int);	// Set listener position
		bool operator==(Sound);				// Equality of sounds
		int getPosX();						// Get x-coord position
		int getPosY();						// Get y-coord position
		string getPath();					// Get path to sound file
		bool isSpatial();					// Spatial test
		void play(bool);					// Play sound
		bool playAndWait(bool);				// Play and wait until finish
		WPARAM playAndGetYorN();			// Play sound and get input
		WPARAM playAndGetYorNRun();			// Play sound and get input
		WPARAM playAndGet123();				// Play sound and get input
		void stop();						// Stop sound
		void fadeOut();						// Fade sound out
		bool isPlaying();					// Playing test
		void setVolume(int);				// Set sound volume (0-255)
};

// Static member definitions
int  Sound::listenerX   = 0;
int  Sound::listenerY   = 0;
ma_engine Sound::engine;
bool Sound::engineInited = false;

// Blank constructor
Sound::Sound() : loaded(false), spatial(false), posX(0), posY(0) {}

//////////////////////////////////////////////////////////////////////////////////////////
//         Constructor given path: Loads sample for traditional stereo playing          //
//////////////////////////////////////////////////////////////////////////////////////////
Sound::Sound(string path)																//
{																						//
	filepath = path;																	//
	loaded   = false;																	//
	spatial  = false;																	//
	posX = posY = 0;																	//
	if(!engineInited) return;															//
	ma_result result = ma_sound_init_from_file(&engine, path.c_str(),					//
	                                            0, NULL, NULL, &maSound);				//
	if(result != MA_SUCCESS) return;													//
	ma_sound_set_spatialization_enabled(&maSound, MA_FALSE);							//
	loaded = true;																		//
}																						//
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//         Constructor given path & position: Loads sample for spatial playing          //
//////////////////////////////////////////////////////////////////////////////////////////
Sound::Sound(string path, int x, int y)													//
{																						//
	filepath = path;																	//
	loaded   = false;																	//
	spatial  = true;																	//
	posX = x; posY = y;																//
	if(!engineInited) return;															//
	ma_result result = ma_sound_init_from_file(&engine, path.c_str(),					//
	                                            0, NULL, NULL, &maSound);				//
	if(result != MA_SUCCESS) return;													//
	ma_sound_set_spatialization_enabled(&maSound, MA_TRUE);								//
	ma_sound_set_position(&maSound, (float)x, 0.0f, (float)y);							//
	ma_sound_set_min_distance(&maSound, 0.0f);											//
	ma_sound_set_max_distance(&maSound, 10000.0f);										//
	loaded = true;																		//
}																						//
//////////////////////////////////////////////////////////////////////////////////////////

Sound::~Sound()
{
	if(loaded)
	{
		if(ma_sound_is_playing(&maSound)) ma_sound_stop(&maSound);
		ma_sound_uninit(&maSound);
	}
}

//////////////////////////////////////////////////
//        init & shutdown of audio engine       //
//////////////////////////////////////////////////
void Sound::init()																		//
{																						//
	if(!engineInited)																	//
	{																					//
		ma_engine_config cfg = ma_engine_config_init();									//
		ma_engine_init(&cfg, &engine);													//
		engineInited = true;															//
	}																					//
}																						//
void Sound::shutdown()																	//
{																						//
	if(engineInited)																	//
	{																					//
		ma_engine_uninit(&engine);														//
		engineInited = false;															//
	}																					//
}																						//
//////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                     setListener: Sets listener position                      //
//////////////////////////////////////////////////////////////////////////////////
void Sound::setListener(int x, int y)											//
{																				//
	listenerX = x; listenerY = y;												//
	if(engineInited)															//
		ma_engine_listener_set_position(&engine, 0,								//
		                                (float)x, 0.0f, (float)y);				//
}																				//
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                     operator==: Tests equality of sounds                     //
//////////////////////////////////////////////////////////////////////////////////
bool Sound::operator==(Sound s)													//
{																				//
	return (filepath == s.getPath() &&											//
		    posX == s.getPosX() && posY == s.getPosY() &&						//
		    spatial == s.isSpatial());											//
}																				//
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//             Sound querying functions             //
//////////////////////////////////////////////////////
int Sound::getPosX(){return posX;}					//
int Sound::getPosY(){return posY;}					//
string Sound::getPath(){return filepath;}			//
bool Sound::isSpatial(){return spatial;}			//
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//                  play: Plays sound & may be looped                   //
//////////////////////////////////////////////////////////////////////////
void Sound::play(bool loop)												//
{																		//
	if(!loaded || isPlaying()) return;									//
	ma_sound_set_looping(&maSound, loop ? MA_TRUE : MA_FALSE);			//
	ma_sound_seek_to_pcm_frame(&maSound, 0);							//
	if(!spatial)														//
	{																	//
		ma_sound_set_spatialization_enabled(&maSound, MA_FALSE);		//
		ma_sound_set_volume(&maSound, 1.0f);							//
	}																	//
	else																//
	{																	//
		ma_sound_set_spatialization_enabled(&maSound, MA_TRUE);			//
		ma_sound_set_position(&maSound, (float)posX, 0.0f, (float)posY);//
		ma_sound_set_volume(&maSound, 32.0f / 255.0f);					//
	}																	//
	ma_sound_start(&maSound);											//
}																		//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////
// playAndWait: Play & wait for finish	//
//////////////////////////////////////////
bool Sound::playAndWait(bool skip)		//
{										//
	bool ret = false;					//
	play(false);						//
	while(isPlaying())					//
	{									//
		SDL_Delay(1);					//
		WPARAM key = MyWin::getKey();	//
		if(skip && key=='F')			//
		{								//
			ret = true;					//
			break;						//
		}								//
	}									//
	if(isPlaying()) stop();				//
	return ret;							//
}										//
//////////////////////////////////////////

//////////////////////////////////////////
// playAndGet[KEY]: Play and get [KEY]  //
//////////////////////////////////////////
WPARAM Sound::playAndGetYorN()			//
{										//
	play(false);						//
	WPARAM key = MyWin::getYorN();		//
	if(isPlaying()) stop();				//
	return key;							//
}										//
//////////////////////////////////////////
WPARAM Sound::playAndGetYorNRun()		//
{										//
	play(false);						//
	WPARAM key = MyWin::getYorNRun();	//
	if(isPlaying()) stop();				//
	return key;							//
}										//
//////////////////////////////////////////
WPARAM Sound::playAndGet123()			//
{										//
	play(false);						//
	WPARAM key = MyWin::get123();		//
	if(isPlaying()) stop();				//
	return key;							//
}										//
//////////////////////////////////////////

//////////////////////////////////
//      stop: Stops sound       //
//////////////////////////////////
void Sound::stop()				//
{								//
	if(!loaded) return;			//
	ma_sound_stop(&maSound);	//
	ma_sound_seek_to_pcm_frame(&maSound, 0);	//
}								//
//////////////////////////////////

//////////////////////////////////////////////
//        fadeOut: Fades a sound out        //
//////////////////////////////////////////////
void Sound::fadeOut()						//
{											//
	if(!isPlaying()) return;				//
	float vol = ma_sound_get_volume(&maSound);	//
	float dec = vol / 10.0f;				//
	while(vol > 0.01f)						//
	{										//
		vol -= dec;							//
		if(vol < 0.0f) vol = 0.0f;			//
		ma_sound_set_volume(&maSound, vol);	//
		SDL_Delay(150);						//
		MyWin::DoEvents();					//
	}										//
	if(isPlaying()) stop();					//
}											//
//////////////////////////////////////////////

//////////////////////////////////////////
//     isPlaying: Is sound playing?     //
//////////////////////////////////////////
bool Sound::isPlaying()					//
{										//
	if(!loaded) return false;			//
	return ma_sound_is_playing(&maSound) == MA_TRUE;	//
}										//
//////////////////////////////////////////

//////////////////////////////////////////////////////
//           setVolume: Set sound volume            //
//////////////////////////////////////////////////////
void Sound::setVolume(int v)						//
{													//
	if(loaded)										//
		ma_sound_set_volume(&maSound, v / 255.0f);	//
}													//
//////////////////////////////////////////////////////
