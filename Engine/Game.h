/********************************************************
 *  Class: Game											*
 *     By: Peter S. VanLund								*
 *   Desc: Engine for The Last Crusade. Engine is very	*
 *         similar to a	state machine, with special		*
 *         functions to handle different input. Quite a	*
 *         few sounds are included as well.				*
 ********************************************************/

#include "Map.h"
#include "GameStruct.h"

// Different states of game
enum GameState {OFF,MENU,MOVE,FIGHT};

class Game
{
	private:
		// Gameplay
		vector<Map*> vecMap;	// Vector of maps for game
		int          current;	// Current map
		GameState    state;		// Current state of game
		Player       player;	// Player
		bool pkey;				// Processing key boolean
		Sound *song;			// Current song
		NumberReader *nr;		// NumberReader
		// Intro
		Sound *theme;			// Theme song
		Sound *title;			// Title of game
		void exploreNode();		// Explores a node on arrival
		void searchNode();		// Searches a node for items
		// Story
		Sound *story;			// Game story
		Sound *ending;			// Game ending
		// Menu
		Sound *menu;			// Main menu
		Sound *nodata;			// No save file
		Sound *nocust;			// No saving on custom maps
		// Directions
		Sound *dirChar;			// Character directions
		Sound *dirEnem;			// Enemy directions
		Sound *dirItem;			// Item directions
		Sound *dirLoca;			// Location directions
		Sound *dirSave;			// Saving directions
		Sound *dirSkip;			// Skipping sound directions
		Sound *dirSpac;			// Spacebar directions
		Sound *dirQuit;			// Quitting game directions
		// Movement
		Sound *entering;		// "You are entering"
		Sound *heading;			// "You are heading"
		Sound *dNorth;			// "north"
		Sound *dSouth;			// "south"
		Sound *dEast;			// "east"
		Sound *dWest;			// "west"
		// Node query
		Sound *nPass;			// "The northern passage"
		Sound *sPass;			// "The southern passage"
		Sound *ePass;			// "The eastern passage"
		Sound *wPass;			// "The western passage"
		Sound *hasbeen;			// "has been visited"
		Sound *isopen;			// "is open"
		Sound *locked;			// "is locked"
		Sound *need;			// "you need"
		Sound *through;			// "to get through"
		Sound *leads;			// "leads to"
		Sound *vendor;			// "a vendor"
		void queryNode();		// Check node neighbors
		// Item search
		Sound *noitems;			// "There are no items here"
		Sound *nomore;			// "There are no more items here"
		Sound *found;			// "You have found"
		Sound *replace;			// "Would you like to replace your"
		Sound *with;			// "with"
		Sound *equipped;		// "You have equipped"
		// Battle
		Sound *fightsong;		// Fight music
		NPC   *enemy;			// Current enemy
		Sound *encounter;		// "You have encountered"
		Sound *attack;			// "His hit attack is"
		Sound *defense;			// "His defense is"
		Sound *hehas;			// "He has"
		Sound *nowhas;			// "Enemy now has"
		Sound *hitpoints;		// "hit points"
		Sound *hitpoint;		// "hit point"
		Sound *hitpointsr;		// "hit points remaining"
		Sound *hitpointr;		// "hit point  remaining"
		Sound *runquestion;		// "Do you wish to run away?"
		Sound *youlost;			// "You have lost"
		Sound *younow;			// "You now have"
		Sound *youattack;		// "You attacked for"
		Sound *collected;		// "You have collected"
		Sound *victory;			// "Victory"
		Sound *easy;			// Easy battle
		Sound *fair;			// Fair battle
		Sound *risky;			// Risky battle
		Sound *enemyMiss;		// "Enemy has missed"
		Sound *playerMiss;		// "You have missed"
		Sound *playerDeath;		// "You have died"
		void enemyAttack();		// Enemy attacks player
		void playerAttack();	// Player attacks enemy
		void lootEnemy();		// Loot enemy of items
		void queryEnemy();		// Determine fight difficulty
		// Vendor
		Sound *welcome;			// "Welcome to"
		Sound *today;			// "Would you like to purchase some items today?"
		Sound *purchase;		// "Would you like to purchase"
		Sound *vfor;			// "for"
		Sound *vgold;			// "gold pieces"
		Sound *nowhave;			// "You now have"
		Sound *vgoldr;			// "gold pieces remaining"
		Sound *afford;			// "You cannot afford this item"
		Sound *goodbye;			// Vendor goodbye
		// Friend
		Sound *talkto;			// "Would you like to talk to"
		// Leprechaun
		Sound *leprechaun;		// "a leprechaun"
		Sound *lepplay;			// Leprechaun game
		Sound *lepmore;			// Leprechaun more
		Sound *lepgone;			// Leprechaun has left
		Sound *leprules;		// Rules of game
		Sound *lep123;			// "Pick 1, 2, or 3"
		Sound *lepwin;			// Lep wins game
		Sound *leplose;			// Lep lost game
		Sound *lepagain;		// "Would you like to play again?"
		Sound *lepbye;			// Lep says bye
		Sound *lepgood;			// Lep not mad you won't play
		Sound *lepmad;			// Lep is  mad you won't play
		Sound *leptook;			// Lep takes gold
		Sound *leplive;			// Lep did not die
		Sound *lepdie;			// Lep dies
		// Stat reading
		Sound *sndStatStr;		// Strength
		Sound *sndStatDef;		// Defense
		// Misc
		Sound *level;			// "level"
		Sound *sndPotion;		// "potion"
		Sound *sndPotions;		// "potions"
		Sound *instruct;		// Spacebar instructions
		void readStats();		// Read player stats
		void readGold();		// Read gold amount
		Sound *coins;			// "gold coins"
		bool first;				// Talk automatically to first friend
		Sound *runyes;			// Run successful
		Sound *runno;			// Run failed
		Sound *notstrong;		// Not strong enough
		bool checkMove(Direction);	// Validate move
		bool custom;			// Using custom map
		void returnToMenu();	// Return to main menu
		void equipWeakItems();	// Equip weakest weapon & armor in map
		// Save & Load game
		void saveGame();		// Save game
		void saveItem(Item*,PLAYERITEM&);	// Save individual item
		void loadGame();		// Load game
		bool savedGameExists();	// Check for saved game
		Item* loadItem(PLAYERITEM);			// Load individual item
		Sound *load;			// "Loading..."
		Sound *save;			// "Game saved"
	public:
		Game();					// Game constructor
		~Game();				// Game destructor
		void addMap(string);	// Add map to game
		void play();			// Play game
		void processKey(WPARAM);// Process key
		void useCustom();		// Use a custom map
};

//////////////////////////////////////////////////////
//  Constructor: Insantiate sounds & set defaults   //
//////////////////////////////////////////////////////
Game::Game()										//
{													//
	// Initialize random number generator & FMOD	//
	Random::init();									//
	Sound::init();									//
	// Create number reader							//
	nr = new NumberReader();						//
	// Set variable defaults						//
	state   = OFF;									//
	current = 0;									//
	song = NULL;									//
	pkey = false;									//
	first = true;									//
	custom = false;									//
	// Load main game sounds						//
	string s = "./sounds/";							//
	// Intro										//
	theme = new Sound(s+"main_theme.mp3");			//
	title = new Sound(s+"title.wav");				//
	// Story										//
	story = new Sound(s+"introduction.mp3");		//
	ending = new Sound(s+"end_king.mp3");			//
	// Menu											//
	menu = new Sound(s+"menu.wav");					//
	nodata = new Sound(s+"no_save_data.wav");		//
	nocust = new Sound(s+"no_save_custom.wav");		//
	// Directions									//
	dirChar = new Sound(s+"dir_characters.wav");	//
	dirEnem = new Sound(s+"dir_enemy.wav");			//
	dirItem = new Sound(s+"dir_items.wav");			//
	dirLoca = new Sound(s+"dir_location.wav");		//
	dirSave = new Sound(s+"dir_save.wav");			//
	dirSkip = new Sound(s+"dir_skip.wav");			//
	dirSpac = new Sound(s+"dir_spacebar.wav");		//
	dirQuit = new Sound(s+"dir_quit.wav");			//
	// Movement										//
	entering = new Sound(s+"entering.wav");			//
	heading = new Sound(s+"g_heading.wav");			//
	dNorth = new Sound(s+"g_north.wav");			//
	dSouth = new Sound(s+"g_south.wav");			//
	dEast = new Sound(s+"g_east.wav");				//
	dWest = new Sound(s+"g_west.wav");				//
	// Node query									//
	nPass = new Sound(s+"g_northern.wav");			//
	sPass = new Sound(s+"g_southern.wav");			//
	ePass = new Sound(s+"g_eastern.wav");			//
	wPass = new Sound(s+"g_western.wav");			//
	hasbeen = new Sound(s+"g_visited.wav");			//
	isopen = new Sound(s+"g_open.wav");				//
	locked = new Sound(s+"g_locked.wav");			//
	need = new Sound(s+"g_need.wav");				//
	through = new Sound(s+"g_through.wav");			//
	leads = new Sound(s+"g_leadto.wav");			//
	vendor = new Sound(s+"g_vendor.wav");			//
	// Item search									//
	noitems = new Sound(s+"g_noitems.wav");			//
	nomore = new Sound(s+"g_nomore.wav");			//
	found = new Sound(s+"g_found.wav");				//
	replace = new Sound(s+"g_replace.wav");			//
	with = new Sound(s+"g_with.wav");				//
	equipped = new Sound(s+"g_equipped.wav");		//
	// Battle										//
	fightsong = new Sound(s+"fight.mp3");			//
	encounter = new Sound(s+"encountered.wav");		//
	attack = new Sound(s+"enemy_a.wav");			//
	defense = new Sound(s+"enemy_d.wav");			//
	hehas = new Sound(s+"enemy_h.wav");				//
	nowhas = new Sound(s+"enemy_nh.wav");			//
	hitpoints = new Sound(s+"hp.wav");				//
	hitpoint = new Sound(s+"hp_1.wav");				//
	hitpointsr = new Sound(s+"hp_remain.wav");		//
	hitpointr = new Sound(s+"hp_remain_1.wav");		//
	runquestion = new Sound(s+"run_away.wav");		//
	youlost = new Sound(s+"player_l.wav");			//
	younow = new Sound(s+"player_nh.wav");			//
	youattack = new Sound(s+"player_a.wav");		//
	collected = new Sound(s+"collected.wav");		//
	victory = new Sound(s+"victory.wav");			//
	easy = new Sound(s+"easy_q.wav");				//
	fair = new Sound(s+"fair_q.wav");				//
	risky = new Sound(s+"risky_q.wav");				//
	enemyMiss = new Sound(s+"enemy_m.wav");			//
	playerMiss = new Sound(s+"player_m.wav");		//
	playerDeath = new Sound(s+"player_x.wav");		//
	// Vendor										//
	welcome = new Sound(s+"v_welcome.wav");			//
	today = new Sound(s+"v_today.wav");				//
	purchase = new Sound(s+"v_purchase.wav");		//
	vfor = new Sound(s+"v_for.wav");				//
	vgold = new Sound(s+"v_gold.wav");				//
	nowhave = new Sound(s+"v_nowhave.wav");			//
	vgoldr = new Sound(s+"v_goldremain.wav");		//
	afford = new Sound(s+"v_cannot.wav");			//
	goodbye = new Sound(s+"v_goodbye.wav");			//
	// Friend										//
	talkto = new Sound(s+"talkto.wav");				//
	// Leprechaun									//
	leprechaun = new Sound(s+"leprechaun.wav");		//
	lepplay = new Sound(s+"play_lep.wav");			//
	lepmore = new Sound(s+"more_lep.wav");			//
	lepgone = new Sound(s+"lepgone.wav");			//
	leprules = new Sound(s+"rules_lep.wav");		//
	lep123 = new Sound(s+"123_lep.wav");			//
	lepwin = new Sound(s+"pwin_lep.wav");			//
	leplose = new Sound(s+"plose_lep.wav");			//
	lepagain = new Sound(s+"again_lep.wav");		//
	lepbye = new Sound(s+"goodbye_lep.wav");		//
	lepgood = new Sound(s+"noplaygood_lep.wav");	//
	lepmad = new Sound(s+"noplaymad_lep.wav");		//
	leptook = new Sound(s+"leptook.wav");			//
	leplive = new Sound(s+"survive_lep.wav");		//
	lepdie = new Sound(s+"death_lep.wav");			//
	// Stat											//
	sndStatStr = new Sound(s+"player_s.wav");		//
	sndStatDef = new Sound(s+"player_d.wav");		//
	// Misc											//
	level = new Sound(s+"level.wav");				//
	coins = new Sound(s+"gold.wav");				//
	sndPotion = new Sound(s+"player_p_1.wav");		//
	sndPotions = new Sound(s+"player_p.wav");		//
	instruct = new Sound(s+"g_instruc.wav");		//
	runyes = new Sound(s+"run_yes.wav");			//
	runno = new Sound(s+"run_no.wav");				//
	notstrong = new Sound(s+"g_notstrong.wav");		//
	// Save & Load game								//
	load = new Sound(s+"loading.wav");				//
	save = new Sound(s+"saved.wav");				//
}													//
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Destructor: Deallocate memory (sounds must be gone before shutdown)  //
//////////////////////////////////////////////////////////////////////////
Game::~Game()															//
{																		//
	for(int i = 0; i<vecMap.size(); i++)								//
		if(vecMap[i]!=NULL) delete vecMap[i];							//
	delete theme;	delete title;	delete story;	delete menu;		//
	delete ending;	delete nodata;	delete nocust;	delete dirChar;		//
	delete dirEnem;	delete dirItem;	delete dirLoca;	delete dirSave;		//
	delete dirSpac;	delete dirQuit;	delete entering;delete heading;		//
	delete dNorth;	delete dSouth;	delete dEast;	delete dWest;		//
	delete nPass;	delete sPass;	delete ePass;	delete wPass;		//
	delete hasbeen;	delete isopen;	delete locked;	delete need;		//
	delete through;	delete leads;	delete vendor;	delete noitems;		//
	delete nomore;	delete found;	delete replace;	delete encounter;	//
	delete equipped;delete with;	delete attack;	delete hitpoints;	//
	delete hehas;	delete nowhas;	delete defense;	delete hitpointsr;	//
	delete hitpoint;delete youlost;	delete younow;	delete hitpointr;	//
	delete easy;	delete fair;	delete risky;	delete runquestion;	//
	delete victory;	delete welcome;	delete today;	delete youattack;	//
	delete purchase;delete vfor;	delete vgold;	delete collected;	//
	delete nowhave;	delete vgoldr;	delete afford;	delete fightsong;	//
	delete goodbye;	delete level;	delete coins;	delete enemyMiss;	//
	delete lepplay;	delete lepmore;	delete lepgone;	delete playerMiss;	//
	delete leprules;delete lep123;	delete lepwin;	delete playerDeath;	//
	delete leplose;	delete lepagain;delete lepbye;	delete leprechaun;	//
	delete lepgood;	delete lepmad;	delete leptook;	delete sndStatStr;	//
	delete lepdie;	delete leplive; delete instruct;delete sndStatDef;	//
	delete runyes;	delete runno;	delete load;	delete sndPotions;	//
	delete save;	delete sndPotion;   			delete notstrong;	//
	delete nr;															//
	player.freeSounds();												//
	// Shutdown FMOD now that all sounds have been freed				//
	Sound::shutdown();													//
}																		//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//   addMap: Adds map to game   //
//////////////////////////////////
void Game::addMap(string s)		//
{vecMap.push_back(new Map(s));}	//
//////////////////////////////////

//////////////////////////////////////
// play: Begin game & set defaults  //
//////////////////////////////////////
void Game::play()					//
{									//
	// Custom map					//
	if(custom)						//
	{								//
		first = true;				//
		current = 0;				//
		Map *map = vecMap[current];	//
		load->play(true);			//
		map->loadMap();				//
		equipWeakItems();			//
		load->fadeOut();			//
		map->activate();			//
		exploreNode();				//
	}								//
	// Normal game					//
	else							//
	{								//
		theme->play(true);			//
		title->playAndWait(false);	//
		menu->play(false);			//
		state = MENU;				//
	}								//
}									//
//////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//     processKey: State machine part of engine, processes all major keystrokes     //
//////////////////////////////////////////////////////////////////////////////////////
void Game::processKey(WPARAM key)													//
{																					//
	Map *map;																		//
	WPARAM skey = 0;																//
	if(pkey) return;																//
	pkey = true;																	//
	// keys:																		//
	// VK_A-VK_Z for letters														//
	// VK_RETURN, VK_SPACE, VK_UP etc												//
	switch(state)																	//
	{																				//
		// Game is off																//
		case OFF: break;															//
		// Main menu																//
		// -> 1 starts new game														//
		// -> 2 resumes saved game													//
		// -> 3 reads instructions													//
		// -> ESC exits game														//
		case MENU:																	//
			switch(key)																//
			{																		//
				// Start new game													//
				case '1':															//
					menu->fadeOut();												//
					theme->fadeOut();												//
					// Read story													//
					story->play(false);												//
					first = true;													//
					while(story->isPlaying() && skey!='F' && skey!=VK_SPACE)		//
					{																//
						skey = MyWin::getKey();										//
					}																//
					if(story->isPlaying()) story->fadeOut();						//
					// Set start of game & load map									//
					current = 0;													//
					map = vecMap[current];											//
					load->play(true);												//
					map->loadMap();													//
					equipWeakItems();												//
					load->fadeOut();												//
					entering->playAndWait(false);									//
					map->sayMapName();												//
					map->activate();												//
					exploreNode();													//
					break;															//
				// Load saved game													//
				case '2':															//
					menu->fadeOut();												//
					if(!savedGameExists())											//
					{																//
						nodata->playAndWait(false);									//
						break;														//
					}																//
					theme->fadeOut();												//
					load->play(true);												//
					loadGame();														//
					load->fadeOut();												//
					entering->playAndWait(false);									//
					vecMap[current]->sayMapName();									//
					first = false;													//
					vecMap[current]->activate();									//
					exploreNode();													//
					break;															//
				// Read instructions												//
				case '3':															//
					menu->fadeOut();												//
					dirSkip->playAndWait(true);										//
					dirSpac->playAndWait(true);										//
					dirChar->playAndWait(true);										//
					dirItem->playAndWait(true);										//
					dirLoca->playAndWait(true);										//
					dirEnem->playAndWait(true);										//
					dirSave->playAndWait(true);										//
					dirQuit->playAndWait(true);										//
					instruct->playAndWait(true);									//
					menu->play(false);												//
					break;															//
				case VK_ESCAPE:														//
					exit(0);														//
					break;															//
			}																		//
			break;																	//
		// Free																		//
		// -> UP, DOWN, LEFT, RIGHT attempt movements								//
		// -> S searches a node for items											//
		// -> D queries possible directions											//
		// -> P uses a potion														//
		case MOVE:																	//
			switch(key)																//
			{																		//
				case VK_UP:															//
					if(vecMap[current]->move(NORTH) && checkMove(NORTH))			//
					{																//
						if(!heading->playAndWait(true)) dNorth->playAndWait(true);	//
						exploreNode();												//
					}																//
					break;															//
				case VK_DOWN:														//
					if(vecMap[current]->move(SOUTH) && checkMove(SOUTH))			//
					{																//
						if(!heading->playAndWait(true)) dSouth->playAndWait(true);	//
						exploreNode();												//
					}																//
					break;															//
				case VK_RIGHT:														//
					if(vecMap[current]->move(EAST) && checkMove(EAST))				//
					{																//
						if(!heading->playAndWait(true)) dEast->playAndWait(true);	//
						exploreNode();												//
					}																//
					break;															//
				case VK_LEFT:														//
					if(vecMap[current]->move(WEST) && checkMove(WEST))				//
					{																//
						if(!heading->playAndWait(true)) dWest->playAndWait(true);	//
						exploreNode();												//
					}																//
					break;															//
				case 'S':															//
					searchNode();													//
					break;															//
				case 'D':															//
					queryNode();													//
					break;															//
				case 'P':															//
					if(player.usePotion())											//
					{																//
						// "You now have [HIT POINTS] hit points"					//
						younow->playAndWait(false); nr->say(player.getHP());		//
						if(player.getHP()==1) hitpoint->playAndWait(false);			//
						else hitpoints->playAndWait(false);							//
					}																//
					break;															//
				case VK_SPACE:														//
					readStats();													//
					break;															//
				case 'A':															//
					// Time for leprechaun fight									//
					if(vecMap[current]->getCurrentNode()->hasLeprechaun())			//
					{																//
						// Leprechaun dies											//
						if(Random::getShort(0,1)==0)								//
						{															//
							lepdie->playAndWait(false);								//
							enemy = vecMap[current]->getCurrentNode()->getNPC();	//
							lootEnemy();											//
						}															//
						// Leprechaun robs you of energy & potions					//
						else														//
						{															//
							leplive->playAndWait(false);							//
							player.setHP(player.getHP()/2);							//
							player.removeAllPotions();								//
							younow->playAndWait(false); nr->say(player.getHP());	//
							if(player.getHP()==1) hitpointr->playAndWait(false);	//
							else hitpointsr->playAndWait(false);					//
							younow->playAndWait(false); nr->say(0);					//
							sndPotions->playAndWait(false);							//
							vecMap[current]->getCurrentNode()->removeNPC();			//
							lepgone->playAndWait(false);							//
						}															//
					}																//
					break;															//
				case 'V':															//
					if(custom)														//
					{																//
						nocust->playAndWait(false);									//
						break;														//
					}																//
					saveGame();														//
					save->playAndWait(false);										//
					break;															//
				case VK_ESCAPE:														//
					returnToMenu();													//
					break;															//
			}																		//
			break;																	//
		// Battle																	//
		// -> A attacks																//
		// -> R runs																//
		// -> P uses potion															//
		case FIGHT:																	//
			switch(key)																//
			{																		//
				case 'A':															//
					playerAttack();													//
					break;															//
				case 'R':															//
					if(Random::checkOdds(enemy->getRun()))							//
					{																//
						runyes->playAndWait(false);									//
						fightsong->fadeOut();										//
						enemy->heal();												//
						vecMap[current]->movePrevious();							//
						exploreNode();												//
					}																//
					else															//
					{																//
						runno->playAndWait(false);									//
						enemyAttack();												//
					}																//
					break;															//
				case 'P':															//
					if(player.usePotion())											//
					{																//
						// "You now have [HIT POINTS] hit points"					//
						younow->playAndWait(false); nr->say(player.getHP());		//
						if(player.getHP()==1) hitpoint->playAndWait(false);			//
						else hitpoints->playAndWait(false);							//
					}																//
					break;															//
				case 'Q':															//
					queryEnemy();													//
					break;															//
			}																		//
			break;																	//
	}																				//
	pkey = false;																	//
}																					//
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//       exploreNode: Explores a node upon arrival for NPCs & also checks for the end of the map        //
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void Game::exploreNode()																				//
{																										//
	// End of map																						//
	if(vecMap[current]->atEndOfMap())																	//
	{																									//
		vecMap[current]->deactivate();																	//
		if(custom) exit(0);																				//
		vecMap[current]->freeMap();																		//
		// Proceed to next map																			//
		current++;																						//
		if(current>vecMap.size()-1) exit(0);															//
		load->play(true);																				//
		vecMap[current]->loadMap();																		//
		load->fadeOut();																				//
		entering->playAndWait(false);																	//
		vecMap[current]->sayMapName();																	//
		vecMap[current]->activate();																	//
		first = true;																					//
		exploreNode();																					//
		return;																							//
	}																									//
	Node *node = vecMap[current]->getCurrentNode();														//
	// Visit node																						//
	node->visit();																						//
	if(!node->hasSong(song))																			//
	{																									//
		if(song!=NULL) song->fadeOut();																	//
		song = node->getRandomSong();																	//
	}																									//
	// Enemy																							//
	if(node->hasEnemy())																				//
	{																									//
		if(song!=NULL)																					//
			song->fadeOut();																			//
		fightsong->play(true);																			//
		fightsong->setVolume(150);																		//
		// Set current enemy and read stats																//
		enemy = node->getNPC();																			//
		// "You have encountered [ENEMY]"																//
		encounter->playAndWait(false); enemy->playNameSound();											//
		// "His attack is [STRENGTH]"																	//
		attack->playAndWait(false); nr->say(enemy->getStr());											//
		// "His defense is [DEFENSE]"																	//
		defense->playAndWait(false); nr->say(enemy->getDef());											//
		// "He has [HIT POINTS] hit points"																//
		hehas->playAndWait(false); nr->say(enemy->getHP());												//
		if(enemy->getHP()==1) hitpoint->playAndWait(false);												//
		else hitpoints->playAndWait(false);																//
		// "Do you wish to run away?"																	//
		WPARAM key = runquestion->playAndGetYorNRun();													//
		if(key=='Y' && Random::checkOdds(enemy->getRun()))												//
		{																								//
			runyes->playAndWait(false);																	//
			fightsong->fadeOut();																		//
			enemy->heal();																				//
			vecMap[current]->movePrevious();															//
			exploreNode();																				//
			return;																						//
		}																								//
		else if(key=='Y')																				//
			runno->playAndWait(false);																	//
		enemyAttack();																					//
	}																									//
	// Vendor																							//
	else if(node->hasVendor())																			//
	{																									//
		WPARAM key;																						//
		if(song!=NULL && !song->isPlaying())															//
		{																								//
			song->play(true);																			//
			song->setVolume(150);																		//
		}																								//
		NPC *vendor = node->getNPC();																	//
		// "Welcome to [VENDOR]"																		//
		welcome->playAndWait(false); vendor->playNameSound();											//
		// "Would you like to purchase some items today?"												//
		key = today->playAndGetYorN();																	//
		if(key=='Y')																					//
		{																								//
			readGold();																					//
			int i;																						//
			// Go through list of items																	//
			int n = vendor->getItemCount();																//
			for(i = 0; i<n; i++)																		//
			{																							//
				Item *newItem = vendor->getItem(i);														//
				if((newItem->getType()==WEAPON && newItem->getValue()<=player.getStr()) ||				//
				   (newItem->getType()==ARMOR  && newItem->getValue()<=player.getDef())) continue;		//
				int cost = (newItem->getType()==POTION) ? vendor->getDef()*newItem->getValue()			//
					                                    : vendor->getStr()*newItem->getValue();			//
				// "Would you like to purchase [ITEM] for [GOLD] gold pieces"							//
				purchase->playAndWait(false); newItem->playNameSound();									//
				if(newItem->getType()==WEAPON || newItem->getType()==ARMOR)								//
				{																						//
					// "level [VALUE]"																	//
					level->playAndWait(false); nr->say(newItem->getValue());							//
				}																						//
				vfor->playAndWait(false); nr->say(cost); vgold->playAndWait(false);						//
				key = MyWin::getYorN();																	//
				if(key=='Y')																			//
				{																						//
					if(player.getGold()>=cost)															//
					{																					//
						player.addItem(newItem);														//
						player.subGold(cost);															//
						// "You have now have [GOLD] gold pieces remaining"								//
						nowhave->playAndWait(false); nr->say(player.getGold());							//
						vgoldr->playAndWait(false);														//
						if(newItem->getType()==POTION) i--;												//
					}																					//
					else																				//
					{																					//
						afford->playAndWait(false);														//
					}																					//
				}																						//
			}																							//
		}																								//
		goodbye->playAndWait(false);																	//
		state = MOVE;																					//
	}																									//
	// Friend																							//
	else if(node->hasFriend())																			//
	{																									//
		if(song!=NULL && !song->isPlaying())															//
		{																								//
			song->play(true);																			//
			song->setVolume(150);																		//
		}																								//
		// If first friend, he will talk automatically													//
		NPC *frnd = node->getNPC();																		//
		if(!first) talkto->playAndWait(false);															//
		if(!first) frnd->playNameSound();																//
		if(first || MyWin::getYorN()=='Y')																//
		{																								//
			first = false;																				//
			frnd->playActionSound();																	//
			// Give item list to player																	//
			int i;																						//
			for(i = 0; i<frnd->getItemCount(); i++)														//
			{																							//
				Item *newItem = frnd->getItem(i);														//
				if((newItem->getType()==WEAPON && newItem->getValue()<=player.getStr()) ||				//
			       (newItem->getType()==ARMOR  && newItem->getValue()<=player.getDef())) continue;		//
				// "You have collected [ITEM]"															//
				collected->playAndWait(false); newItem->playNameSound();								//
				if(newItem->getType()==WEAPON || newItem->getType()==ARMOR)								//
				{																						//
					Item *item = (newItem->getType()==WEAPON) ? player.getWeapon() : player.getArmor();	//
					// "Would you like to replace your [ITEM] level [LEVEL] with [ITEM] level [LEVEL]"	//
					replace->playAndWait(false); item->playNameSound();									//
					level->playAndWait(false); nr->say(item->getValue());								//
					with->playAndWait(false); newItem->playNameSound();									//
					level->playAndWait(false); nr->say(newItem->getValue());							//
					if(MyWin::getYorN()=='Y')															//
					{																					//
						player.addItem(newItem);														//
						// "You have equipped [ITEM]"													//
						equipped->playAndWait(false); newItem->playNameSound();							//
						frnd->removeItem(i);															//
						i--;																			//
					}																					//
				}																						//
				else																					//
				{																						//
					player.addItem(newItem);															//
					frnd->removeItem(i);																//
					i--;																				//
				}																						//
			}																							//
		}																								//
		state = MOVE;																					//
	}																									//
	// Leprechaun																						//
	else if(node->hasLeprechaun())																		//
	{																									//
		bool rules = true;																				//
		if(song!=NULL && !song->isPlaying())															//
		{																								//
			song->play(true);																			//
			song->setVolume(150);																		//
		}																								//
		NPC *lep = node->getNPC();																		//
		// "You have encountered [a leprechaun]"														//
		encounter->playAndWait(false); leprechaun->playAndWait(false);									//
		// Player agrees to play game																	//
		if(lepplay->playAndGetYorN()=='Y')																//
		{																								//
			while(true)																					//
			{																							//
				if(player.getGold()<50)																	//
				{																						//
					lepmore->playAndWait(false);														//
					break;																				//
				}																						//
				else																					//
				{																						//
					// Set up game with three pots														//
					vector<int> pot;																	//
					vector<int> potSet;																	//
					// Randomize set of pots															//
					switch(Random::getShort(0,2))														//
					{																					//
						case 0:																			//
							potSet.push_back(-50);														//
							potSet.push_back(25);														//
							potSet.push_back(25);														//
							break;																		//
						case 1:																			//
							potSet.push_back(-50);														//
							potSet.push_back(35);														//
							potSet.push_back(35);														//
							break;																		//
						case 2:																			//
							potSet.push_back(-50);														//
							potSet.push_back(-50);														//
							potSet.push_back(75);														//
							break;																		//
					}																					//
					for(int i = 2; i>=0; i--)															//
					{																					//
						int j = Random::getShort(0,i);													//
						pot.push_back(potSet[j]);														//
						potSet.erase(potSet.begin()+j);													//
					}																					//
					if(rules) leprules->playAndWait(true);												//
					rules = false;																		//
					string input = "";																	//
					input+=(char)lep123->playAndGet123();												//
					int result = pot[atoi(input.c_str())-1];											//
					if(result>0) lepwin->playAndWait(false);											//
					else         leplose->playAndWait(false);											//
					player.addGold(result);																//
					readGold();																			//
					if(lepagain->playAndGetYorN()=='N')													//
					{																					//
						lepbye->playAndWait(false);														//
						break;																			//
					}																					//
				}																						//
			}																							//
		}																								//
		// Player chooses not to play game																//
		else																							//
		{																								//
			// Leprechaun gets mad																		//
			if(Random::getShort(0,2)==0)																//
			{																							//
				lepmad->playAndWait(false);																//
				lepgone->playAndWait(false);															//
				player.subGold(player.getGold()/2);														//
				readGold();																				//
				node->removeNPC();																		//
			}																							//
			// Leprechaun is fine																		//
			else																						//
			{																							//
				lepgood->playAndWait(false);															//
			}																							//
		}																								//
		state = MOVE;																					//
	}																									//
	else																								//
	{																									//
		if(song!=NULL && !song->isPlaying())															//
		{																								//
			song->play(true);																			//
			song->setVolume(150);																		//
		}																								//
		state = MOVE;																					//
	}																									//
}																										//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// I'm going to stop drawing boxes now, it is way too time consuming :)

// searchNode: Searches the current node for items
void Game::searchNode()
{
	Node *node = vecMap[current]->getCurrentNode();
	int f = 0, n = node->getItemCount();
	int g = player.getGold();
	// No items found
	if(n==0)
	{
		noitems->playAndWait(false);
	}
	// Items found (but only list those useful to player)
	else
	{
		int i;
		for(i = 0; i<node->getItemCount(); i++)
		{
			Item *newItem = node->getItem(i);
			// Ignore weaker weapon or armor
			if((newItem->getType()==WEAPON && newItem->getValue()<=player.getStr()) ||
			   (newItem->getType()==ARMOR  && newItem->getValue()<=player.getDef())) continue;
			f++;
			// "You have found [ITEM]"
			found->playAndWait(false); newItem->playNameSound();
			if(newItem->getType()==WEAPON || newItem->getType()==ARMOR)
			{
				Item *item = (newItem->getType()==WEAPON) ? player.getWeapon() : player.getArmor();
				// "Would you like to replace your [ITEM] level [LEVEL] with [ITEM] level [LEVEL]"
				replace->playAndWait(false); item->playNameSound(); level->playAndWait(false); nr->say(item->getValue());
				with->playAndWait(false); newItem->playNameSound(); level->playAndWait(false); nr->say(newItem->getValue());
				if(MyWin::getYorN()=='Y')
				{
					player.addItem(newItem);
					// "You have equipped [ITEM]"
					equipped->playAndWait(false); newItem->playNameSound();
					node->removeItem(i);
					i--;
				}
			}
			else
			{
				player.addItem(newItem);
				node->removeItem(i);
				i--;
			}
		}
		if(f>0) nomore->playAndWait(false);
		else    noitems->playAndWait(false);
	}
	// Read gold if it has increased
	if(player.getGold()>g)
		readGold();
}

// enemyAttack: Enemy attacks player
void Game::enemyAttack()
{
	enemy->playActionSound();
	// Calculate total for attack value
	int total = Random::getShort(0,enemy->getStr())+enemy->getStr()-player.getDef();
	// Enemy hits
	if(total>0)
	{
		player.subHP(total);
		// "You have lost [TOTAL] hit points"
		youlost->playAndWait(false); nr->say(total);
		if(total==1) hitpoint->playAndWait(false);
		else hitpoints->playAndWait(false);
		// If player is dead, try to revive with undead elixir
		if(player.isDead() && !player.useUndeadElixir())
		{
			// Player dies and game returns to main menu
			playerDeath->playAndWait(false);
			fightsong->fadeOut();
			returnToMenu();
			return;
		}
		// "You now have [HIT POINTS] hit points remaining"
		younow->playAndWait(false); nr->say(player.getHP());
		if(player.getHP()==1) hitpointr->playAndWait(false);
		else hitpointsr->playAndWait(false);
	}
	// Enemy missed
	else
	{
		enemyMiss->playAndWait(false);
	}
	state = FIGHT;
}

// playerAttack: Player attacks enemy
void Game::playerAttack()
{
	player.playWeaponAction();
	// Calculate attack total
	int total = Random::getShort(0,player.getStr())+player.getStr()-enemy->getDef();
	// "death blade" & "black talon" have chances of killing enemy in one hit
	if((player.getWeapon()->getName()=="death blade" && Random::checkOdds(25))||
	   (player.getWeapon()->getName()=="black talon" && Random::checkOdds(15)))
	{
		total = enemy->getHP();
	}
	// Player hits
	if(total>0)
	{
		enemy->subHP(total);
		// "You attacked for [TOTAL]"
		youattack->playAndWait(false); nr->say(total);
		if(enemy->isDead())
		{
			// "Victory"
			victory->playAndWait(false);
			// Get enemy items
			lootEnemy();
			return;
			enemy = NULL;
			vecMap[current]->getCurrentNode()->removeNPC();
			fightsong->fadeOut();
			if(song!=NULL)
			{
				song->play(true);
				song->setVolume(150);
			}
			state = MOVE;
			return;
		}
		// "Enemy now has [HIT POINTS] hit points remaining"
		nowhas->playAndWait(false); nr->say(enemy->getHP());
		if(enemy->getHP()==1) hitpointr->playAndWait(false);
		else hitpointsr->playAndWait(false);
	}
	// Player misses
	else
	{
		playerMiss->playAndWait(false);
	}
	// Enemy's turn
	enemyAttack();
}

// lootEnemy: Take all items from enemy
void Game::lootEnemy()
{
	int n = enemy->getItemCount();
	int i;
	int g = player.getGold();
	// Game may be over if "crown" found
	bool gameEnd = false;
	// Go through item list
	for(i = 0; i<n; i++)
	{
		Item *newItem = enemy->getItem();
		if(newItem->getName()=="crown") gameEnd = true;
		// Ignore weaker weapon or armor
		if((newItem->getType()==WEAPON && newItem->getValue()<=player.getStr()) ||
		   (newItem->getType()==ARMOR  && newItem->getValue()<=player.getDef())) continue;
		// "You have collected [ITEM]"
		collected->playAndWait(false); newItem->playNameSound();
		if(newItem->getType()==WEAPON || newItem->getType()==ARMOR)
		{
			Item *item = (newItem->getType()==WEAPON) ? player.getWeapon() : player.getArmor();
			// "Would you like to replace your [ITEM] level [LEVEL] with [ITEM] level [LEVEL]"
			replace->playAndWait(false); item->playNameSound(); level->playAndWait(false); nr->say(item->getValue());
			with->playAndWait(false); newItem->playNameSound(); level->playAndWait(false); nr->say(newItem->getValue());
			if(MyWin::getYorN()=='Y')
			{
				player.addItem(newItem);
				// "You have equipped [ITEM]"
				equipped->playAndWait(false); newItem->playNameSound();
			}
		}
		else
		{
			player.addItem(newItem);
		}
	}
	// If gold has changed, read out new total
	if(player.getGold()>g)
		readGold();
	enemy = NULL;
	// Remove enemy from node
	vecMap[current]->getCurrentNode()->removeNPC();
	// Player has won game
	if(gameEnd)
	{
		fightsong->fadeOut();
		ending->playAndWait(false);
		returnToMenu();
		return;
	}
	// End fight song if necessary
	if(fightsong->isPlaying())
	{
		fightsong->fadeOut();
		if(song!=NULL)
		{
			song->play(true);
			song->setVolume(150);
		}
	}
	state = MOVE;
}

// queryNode: Checks open directions
void Game::queryNode()
{
	Node *node = vecMap[current]->getCurrentNode();
	Node *temp = node->getNeighbor(NORTH);
	bool skip;
	// Look in all four directions
	for(int i = 0; i<4; i++)
	{
		Node *temp = node->getNeighbor((Direction)i);
		// Skip if no neighbor in that direction
		if(temp==NULL) continue;
		// Skip variable used to capture if 'F' was pressed during sounds
		skip = (i==0) ? nPass->playAndWait(true) : (i==1) ? sPass->playAndWait(true) :
		       (i==2) ? ePass->playAndWait(true) : wPass->playAndWait(true);
		// Node has been visited
		if(temp->hasBeenVisited() && !skip)
		{
			// No NPC
			if(!temp->hasNPC())
				hasbeen->playAndWait(true);
			// Leads to NPC
			else
			{
				skip = leads->playAndWait(true);
				if(temp->hasVendor() && !skip)
					vendor->playAndWait(true);
				else if(temp->hasLeprechaun() && !skip)
					leprechaun->playAndWait(true);
				else if(!skip)
					temp->getNPC()->playNameSound();
			}
		}
		// Node is not locked
		else if(temp->getRequiredItemCount()==0 && !skip)
			isopen->playAndWait(true);
		// Node may be locked if player does not have items
		else if(!skip)
		{
			bool missingItem = false;
			// Check player for required items
			for(int i = 0; i<temp->getRequiredItemCount(); i++)
			{
				if(!player.hasSpecialItem(temp->getRequiredItem(i)))
				{
					// Missing an item
					if(!missingItem)
					{
						locked->playAndWait(false);
						need->playAndWait(false);
						missingItem = true;
					}
					temp->getRequiredItem(i)->playNameSound();

				}
			}
			if(missingItem) through->playAndWait(false);
			else isopen->playAndWait(true);
		}
	}
}

// readStats: Read player stats & instructions
void Game::readStats()
{
	if(sndStatStr->playAndWait(true)) return;
	nr->say(player.getStr());
	if(sndStatDef->playAndWait(true)) return;
	nr->say(player.getDef());
	if(younow->playAndWait(true)) return;
	nr->say(player.getHP());
	if(player.getHP()==1) hitpoint->playAndWait(false);
	else hitpoints->playAndWait(false);
	if(younow->playAndWait(true)) return;
	nr->say(player.getGold());
	if(coins->playAndWait(true)) return;
	if(younow->playAndWait(true)) return;
	nr->say(player.getPotionCount());
	if(player.getPotionCount()==1) sndPotion->playAndWait(false);
	else sndPotions->playAndWait(false);
	if(player.getSpecialItemCount()>0)
	{
		if(younow->playAndWait(true)) return;
		player.playSpecialItemNames();
	}
	if(instruct->playAndWait(true)) return;
}

// readGold: Read gold amount of player
void Game::readGold()
{
	younow->playAndWait(false); nr->say(player.getGold()); coins->playAndWait(false);
}

// checkMove: Check to see if player can move
bool Game::checkMove(Direction d)
{
	Node *node = vecMap[current]->getCurrentNode();
	bool missingItem = false;
	// Look for required items
	for(int i = 0; i<node->getRequiredItemCount(); i++)
	{
		if(!player.hasSpecialItem(node->getRequiredItem(i)))
		{
			if(!missingItem)
			{
				missingItem = true;
				need->playAndWait(false);
			}
			node->getRequiredItem(i)->playNameSound();
		}
	}
	if(missingItem)
	{
		through->playAndWait(false);
		vecMap[current]->movePrevious();
		return false;
	}
	// If there is an enemy, make sure player can cause damage to him
	if(node->hasEnemy() && player.getStr()*2 <= node->getNPC()->getDef() && !(player.getWeapon()->getName()=="death blade"))
	{
		notstrong->playAndWait(false);
		switch(d)
		{
			case NORTH: dNorth->playAndWait(false); break;
			case SOUTH: dSouth->playAndWait(false); break;
			case EAST : dEast->playAndWait(false); break;
			case WEST : dWest->playAndWait(false); break;
		}
		vecMap[current]->movePrevious();
		return false;
	}
	return true;
}

// queryEnemy: Check difficulty of current battle
void Game::queryEnemy()
{
	if(enemy==NULL) return;
	double factor = enemy->getDef()/(player.getStr()*2.0);
	if(factor >= .75)
		risky->playAndWait(false);
	else if(factor >= .35)
		fair->playAndWait(false);
	else
		easy->playAndWait(false);
}

// returnToMenu: Return to main menu
void Game::returnToMenu()
{
	// Deactivate map, reset player, etc.
	vecMap[current]->deactivate();
	if(custom) exit(0);
	vecMap[current]->freeMap();
	player.freeSounds();
	player.clearPlayer();
	player.resetPlayer();
	theme->play(true);
	menu->play(false);
	state = MENU;
}

// saveGame: Save game
void Game::saveGame()
{
	int i,j;
	// Save player information
	FILE *file = fopen("player.sav","wb");
	PLAYERHEADER ph;
	ph.pMap = current;
	ph.pNode = vecMap[current]->getCurrentNodeNumber();
	ph.pGold = player.getGold();
	ph.pMaxHP = player.getMaxHP();
	ph.pHP = player.getHP();
	ph.pStr = player.getStr();
	ph.pDef = player.getDef();
	// Weapon
	saveItem(player.getWeapon(),ph.pWeapon);
	// Armor
	saveItem(player.getArmor(),ph.pArmor);
	// Get Potion & Special item count
	ph.pPotionCount = player.getPotionCount();
	ph.pSpecialCount = player.getSpecialItemCount();
	// Write PLAYERHEADER
	fwrite(&ph,sizeof(PLAYERHEADER),1,file);
	// Save player potions
	for(i = 0; i<ph.pPotionCount; i++)
	{
		PLAYERITEM pi;
		saveItem(player.getPotion(i),pi);
		fwrite(&pi,sizeof(PLAYERITEM),1,file);
	}
	// Save player special items
	for(i = 0; i<ph.pSpecialCount; i++)
	{
		PLAYERITEM pi;
		saveItem(player.getSpecial(i),pi);
		fwrite(&pi,sizeof(PLAYERITEM),1,file);
	}
	// Save state of all nodes for current map (Visited & NPCs & Items)
	Map *map = vecMap[current];
	for(i = 0; i<map->getNodeCount(); i++)
	{
		// Save node information
		Node *node = map->getNode(i);
		PLAYERNODEHEADER pn;
		pn.pnVisited = node->hasBeenVisited();
		pn.pnItemCount = node->getItemCount();
		pn.pnNPCCount = (node->hasNPC()) ? 1 : 0;
		fwrite(&pn,sizeof(PLAYERNODEHEADER),1,file);
		// Save items at node
		for(j = 0; j<pn.pnItemCount; j++)
		{
			int itemNum = map->getItemNumber(node->getItem(j));
			fwrite(&itemNum,sizeof(int),1,file);
		}
		// Save NPC data
		if(pn.pnNPCCount>0)
		{
			NPC *npc = node->getNPC();
			PLAYERNPCHEADER pnpc;
			pnpc.npcIndex = map->getNPCNumber(npc);
			pnpc.npcStr = npc->getStr();
			pnpc.npcDef = npc->getDef();
			pnpc.npcHP = npc->getHP();
			pnpc.npcItemCount = npc->getItemCount();
			fwrite(&pnpc,sizeof(PLAYERNPCHEADER),1,file);
			// Save NPC items
			for(j = 0; j<pnpc.npcItemCount; j++)
			{
				int itemNum = map->getItemNumber(npc->getItem(j));
				fwrite(&itemNum,sizeof(int),1,file);
			}
		}
	}
	fclose(file);
}

// saveItem: Copy individual item into PLAYERITEM form
void Game::saveItem(Item *item,PLAYERITEM &pi)
{
	int i;
	// Set name
	string strItem = item->getName();
	for(i = 0; i<strItem.length(); i++)
		pi.piName[i] = strItem[i];
	pi.piName[i] = '\0';
	// Set type
	pi.piType = (short)item->getType();
	// Set value
	pi.piValue = item->getValue();
	// Set path to name sound
	strItem = (item->getNameSound()==NULL) ? "" : item->getNameSound()->getPath();
	for(i = 0; i<strItem.length(); i++)
		pi.piNameSound[i] = strItem[i];
	pi.piNameSound[i] = '\0';
	// Set path to action sound
	strItem = (item->getActionSound()==NULL) ? "" : item->getActionSound()->getPath();
	for(i = 0; i<strItem.length(); i++)
		pi.piActionSound[i] = strItem[i];
	pi.piActionSound[i] = '\0';
}

// loadGame: Load saved game
void Game::loadGame()
{
	int i,j;
	// Load player information
	FILE *file = fopen("player.sav","rb");
	PLAYERHEADER ph;
	fread(&ph,sizeof(PLAYERHEADER),1,file);
	current = ph.pMap;
	vecMap[current]->loadMap();
	Map *map = vecMap[current];
	map->setCurrentNodeNumber(ph.pNode);
	player.setGold(ph.pGold);
	player.setMaxHP(ph.pMaxHP);
	player.setHP(ph.pHP);
	Item *item = loadItem(ph.pWeapon);
	player.addItem(item);
	item->freeSounds();
	delete item;
	item = loadItem(ph.pArmor);
	player.addItem(item);
	item->freeSounds();
	delete item;
	// Load potions & special items
	for(i = 0; i<ph.pPotionCount+ph.pSpecialCount; i++)
	{
		PLAYERITEM pi;
		fread(&pi,sizeof(PLAYERITEM),1,file);
		item = loadItem(pi);
		player.addItem(item);
		item->freeSounds();
		delete item;
	}
	// Load state of all nodes for current map (Visisted & NPCs & Items)
	for(i = 0; i<map->getNodeCount(); i++)
	{
		// Load node information
		Node *node = map->getNode(i);
		node->removeAllItems();
		node->removeNPC();
		PLAYERNODEHEADER pn;
		fread(&pn,sizeof(PLAYERNODEHEADER),1,file);
		if(pn.pnVisited) node->visit();
		// Add items to node
		for(j = 0; j<pn.pnItemCount; j++)
		{
			int itemNum;
			fread(&itemNum,sizeof(int),1,file);
			node->addItem(map->getItem(itemNum));
		}
		// Add NPC to node
		if(pn.pnNPCCount>0)
		{
			PLAYERNPCHEADER pnpc;
			fread(&pnpc,sizeof(PLAYERNPCHEADER),1,file);
			NPC *npc = map->getNPC(pnpc.npcIndex);
			node->addNPC(npc);
			npc = node->getNPC();
			npc->removeAllItems();
			npc->setStr(pnpc.npcStr);
			npc->setDef(pnpc.npcDef);
			npc->setHP(pnpc.npcHP);
			// Add items to NPC
			for(j = 0; j<pnpc.npcItemCount; j++)
			{
				int itemNum;
				fread(&itemNum,sizeof(int),1,file);
				npc->addItem(map->getItem(itemNum),100);
			}
		}
	}
	fclose(file);
}

// loadItem: Copies PLAYERITEM data into Item*
Item* Game::loadItem(PLAYERITEM pi)
{
	Sound *nSound = (strlen(pi.piNameSound)==0) ? NULL : new Sound(pi.piNameSound);
	Sound *aSound = (strlen(pi.piActionSound)==0) ? NULL : new Sound(pi.piActionSound);
	return new Item(pi.piName,pi.piType,pi.piValue,nSound,aSound);
}

// savedGameExists: Check to see if saved game exists
bool Game::savedGameExists()
{
	FILE *file = fopen("player.sav","rb");
	return (file) ? true : false;
}

// useCustom: Use custom map (loaded from Map Maker)
void Game::useCustom(){custom = true;}

// equipWeakItems: Find weakest weapon & armor in map and add to player
void Game::equipWeakItems()
{
	// Variables for tracking weak items
	int i, minW = -1, minWV = 0, minA = -1, minAV = 0;
	Map *map = vecMap[current];
	// Look through items in map
	for(i = 0; i<map->getItemCount(); i++)
	{
		Item *item = map->getItem(i);
		if(item->getType()==WEAPON)
		{
			// Weaker weapon found
			if(minW==-1 || (minW!=-1 && item->getValue()<minWV))
			{
				minWV = item->getValue();
				minW = i;
			}
		}
		else if(item->getType()==ARMOR)
		{
			// Weaker armor found
			if(minA==-1 || (minA!=-1 && item->getValue()<minAV))
			{
				minAV = item->getValue();
				minA = i;
			}
		}
	}
	// Add item to player
	player.addItem(map->getItem(minW));
	player.addItem(map->getItem(minA));
}