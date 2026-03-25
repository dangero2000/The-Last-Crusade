/********************************************************
 *  Class: Player										*
 *     By: Peter S. VanLund								*
 *   Desc: Defines player for game and contains all		*
 *         necessary methods & variables				*
 ********************************************************/

#include "Node.h"

class Player
{
	private:
		int gold;						// Gold
		int hp, maxHP;					// Hitpoints & max hitpoints
		int str;						// Strength
		int def;						// Defense
		vector<Item*> vecPotion;		// Vector of potions
		vector<Item*> vecSpecial;		// Vector of special items
		Item *weapon;					// Weapon
		Item *armor;					// Armor
	public:
		Player();						// Player constructor
		~Player();						// Player desctructor
		void freeSounds();				// Free sounds used by player
		void addItem(Item*);			// Add item
		void addHP(int);				// Add hitpoints
		void subHP(int);				// Subtract hitpoints
		int getHP();					// Get hitpoints
		void setHP(int);				// Set hitpoints
		int getMaxHP();					// Get max hitpoints
		void setMaxHP(int);				// Set max hitpoints
		int getGold();					// Get gold
		void setGold(int);				// Set gold
		void addGold(int);				// Add gold
		void subGold(int);				// Substract gold
		int getStr();					// Get strength
		int getDef();					// Get defense
		bool isDead();					// Death test
		void playWeaponName();			// Player weapon name sound
		void playWeaponAction();		// Play weapon action sound
		Item* getWeapon();				// Get weapon
		void playArmorName();			// Play armor name sound
		void playArmorAction();			// Play armor action sound
		Item* getArmor();				// Get armor
		bool usePotion();				// Use potion
		bool useUndeadElixir();			// Use undead elixir
		void removeAllPotions();		// Remove all potions
		int getPotionCount();			// Get potion count
		Item* getPotion(int);			// Get potion i
		int getSpecialItemCount();		// Get special item count
		Item* getSpecial(int);			// Get special item i
		void playSpecialItemNames();	// Play all special item name sounds
		bool hasSpecialItem(Item*);		// Special item test
		void resetPlayer();				// Reset player
		void clearPlayer();
};

//////////////////////////////////
// Constructor: Default player  //
//////////////////////////////////
Player::Player()				//
{								//
	weapon = armor = NULL;		//
	resetPlayer();				//
}								//
//////////////////////////////////

void Player::resetPlayer()
{
	// Player defaults			//
	gold = hp = maxHP = 100;	//
	str = def = 0;				//
	weapon = new Item();		//
	armor  = new Item();		//
}

void Player::clearPlayer()
{
	while(vecPotion.size() > 0)
	{
		delete vecPotion[0];
		vecPotion.erase(vecPotion.begin());
	}
	while(vecSpecial.size() > 0)
	{
		delete vecSpecial[0];
		vecSpecial.erase(vecSpecial.begin());
	}
	delete weapon;
	delete armor;
}

//////////////////////////////////////////////
//      Destructor: Deallocate memory       //
//////////////////////////////////////////////
Player::~Player()							//
{											//
	clearPlayer();
}											//
//////////////////////////////////////////////

//////////////////////////////////////////////
//   freeSounds: Deallocate sound memory    //
//////////////////////////////////////////////
void Player::freeSounds()					//
{											//
	int i;									//
	for(i = 0; i<vecPotion.size(); i++)		//
		vecPotion[i]->freeSounds();			//
	for(i = 0; i<vecSpecial.size(); i++)	//
		vecSpecial[i]->freeSounds();		//
	weapon->freeSounds();					//
	armor->freeSounds();					//
}											//
//////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                     addItem: Add item to player                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::addItem(Item *i)																			//
{																										//
	// Add gold to total																				//
	if(i->getType()==GOLD) gold += i->getValue();														//
	// Add weapons and armor to correct variable														//
	// Make copy of i and allocate new sounds (for map to map travel)									//
	else if(i->getType()==WEAPON)																		//
	{																									//
		weapon->freeSounds();																			//
		delete weapon;																					//
		Sound *nSound = (i->getNameSound()==NULL) ? NULL : new Sound(i->getNameSound()->getPath());		//
		Sound *aSound = (i->getActionSound()==NULL) ? NULL : new Sound(i->getActionSound()->getPath());	//
		weapon = new Item(i->getName(),(short)i->getType(),i->getValue(),nSound,aSound);				//
		str = i->getValue();																			//
	}																									//
	else if(i->getType()==ARMOR)																		//
	{																									//
		armor->freeSounds();																			//
		delete armor;																					//
		Sound *nSound = (i->getNameSound()==NULL) ? NULL : new Sound(i->getNameSound()->getPath());		//
		Sound *aSound = (i->getActionSound()==NULL) ? NULL : new Sound(i->getActionSound()->getPath());	//
		armor = new Item(i->getName(),(short)i->getType(),i->getValue(),nSound,aSound);					//
		def = i->getValue();																			//
	}																									//
	// Add potions and special items similarly															//
	else																								//
	{																									//
		Sound *nSound = (i->getNameSound()==NULL) ? NULL : new Sound(i->getNameSound()->getPath());		//
		Sound *aSound = (i->getActionSound()==NULL) ? NULL : new Sound(i->getActionSound()->getPath());	//
		Item *newItem = new Item(i->getName(),(short)i->getType(),i->getValue(),nSound,aSound);			//
		if(i->getType()==POTION)																		//
			vecPotion.push_back(newItem);																//
		else if(i->getType()==SPECIAL)																	//
			vecSpecial.push_back(newItem);																//
	}																									//
}																										//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//             Player query & modification routines             //
//////////////////////////////////////////////////////////////////
void Player::addHP(int h){hp = (hp+h>maxHP) ? maxHP : hp+h;}	//
void Player::subHP(int h){hp-=h;}								//
int Player::getHP(){return hp;}									//
void Player::setHP(int h){hp=h;}								//
int Player::getMaxHP(){return maxHP;}							//
void Player::setMaxHP(int h){maxHP=h;}							//
int Player::getGold(){return gold;}								//
void Player::setGold(int g){gold=g;}							//
void Player::addGold(int g){gold+=g;}							//
void Player::subGold(int g){if(gold>=g) gold-=g;}				//
int Player::getStr(){return str;}								//
int Player::getDef(){return def;}								//
bool Player::isDead(){return hp<=0;}							//
void Player::playWeaponName(){weapon->playNameSound();}			//
void Player::playWeaponAction(){weapon->playActionSound();}		//
Item* Player::getWeapon(){return weapon;}						//
void Player::playArmorName(){armor->playNameSound();}			//
void Player::playArmorAction(){armor->playActionSound();}		//
Item* Player::getArmor(){return armor;}							//
int Player::getPotionCount(){return vecPotion.size();}			//
int Player::getSpecialItemCount(){return vecSpecial.size();}	//
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////
//      usePotion: Player uses potion       //
//////////////////////////////////////////////
bool Player::usePotion()					//
{											//
	if(vecPotion.size()==0) return false;	//
	vecPotion[0]->playActionSound();		//
	addHP(vecPotion[0]->getValue());		//
	vecPotion[0]->freeSounds();				//
	delete vecPotion[0];					//
	vecPotion.erase(vecPotion.begin());		//
	return true;							//
}											//
//////////////////////////////////////////////

//////////////////////////////////////////////
//  removeAllPotions: Removes all potions   //
//////////////////////////////////////////////
void Player::removeAllPotions()				//
{											//
	while(vecPotion.size()>0)				//
	{										//
		vecPotion[0]->freeSounds();			//
		delete vecPotion[0];				//
		vecPotion.erase(vecPotion.begin());	//
	}										//
}											//
//////////////////////////////////////////////


//////////////////////////////////////////////
//    get[ITEM]: Get item i from player     //
//////////////////////////////////////////////
Item* Player::getPotion(int i)				//
{											//
	if(i>vecPotion.size()-1) return NULL;	//
	return vecPotion[i];					//
}											//
//////////////////////////////////////////////
Item* Player::getSpecial(int i)				//
{											//
	if(i>vecSpecial.size()-1) return NULL;	//
	return vecSpecial[i];					//
}											//
//////////////////////////////////////////////

//////////////////////////////////////////////////
// playSpecialItemNames: Say all special items  //
//////////////////////////////////////////////////
void Player::playSpecialItemNames()				//
{												//
	for(int i = 0; i<vecSpecial.size(); i++)	//
	{											//
		vecSpecial[i]->playNameSound();			//
		Sound::resetSkip();						//
	}											//
}												//
//////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//         hasSpecialItem: Check to see if player has item          //
//////////////////////////////////////////////////////////////////////
bool Player::hasSpecialItem(Item *item)								//
{																	//
	for(int i = 0; i<vecSpecial.size(); i++)						//
		if(vecSpecial[i]->getName()==item->getName()) return true;	//
	return false;													//
}																	//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// useUndeadElixir: Player automatically uses elixir//
//////////////////////////////////////////////////////
bool Player::useUndeadElixir()						//
{													//
	for(int i = 0; i<vecSpecial.size(); i++)		//
	{												//
		if(vecSpecial[i]->getValue()==666)			//
		{											//
			hp = maxHP;								//
			vecSpecial[i]->playActionSound();		//
			delete vecSpecial[i]->getNameSound();	//
			delete vecSpecial[i]->getActionSound();	//
			delete vecSpecial[i];					//
			vecSpecial.erase(vecSpecial.begin()+i);	//
			return true;							//
		}											//
	}												//
	return false;									//
}													//
//////////////////////////////////////////////////////