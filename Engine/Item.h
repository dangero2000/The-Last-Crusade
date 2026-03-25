/********************************************************
 *  Class: Items										*
 *     By: Peter S. VanLund								*
 *   Desc: Used to create Item objects					*
 ********************************************************/

#include "NumberReader.h"
#include "Random.h"
#include <string>

using namespace std;

// Different types of items
enum ItemType {WEAPON, ARMOR, POTION, GOLD, SPECIAL};

class Item
{
	private:
		string   name;						// Name
		ItemType type;						// Type
		short    value;						// Value
		Sound    *nameSound, *actionSound;	// Sounds
	public:
		// Constructors & Deconstructor
		Item(string,short,short,Sound*,Sound*);
		Item();
		~Item();
		bool operator==(Item);		// Test equality
		string   getName();			// Get name
		ItemType getType();			// Get type
		short    getValue();		// Get value
		Sound*   getNameSound();	// Get name sound
		Sound*   getActionSound();	// Get action sound
		void     playNameSound();	// Play name sound
		void     playActionSound();	// Play action sound
		void     freeNameSound();	// Free name sound
		void     freeActionSound();	// Free action sound
		void     freeSounds();		// Free all sounds
};

// Constructor with data
Item::Item(string n, short t, short v, Sound *nam, Sound *act)
{
	name        = n;
	type        = (ItemType)t;
	value       = v;
	nameSound   = nam;
	actionSound = act;
}

// Blank constructor
Item::Item(){nameSound = NULL; actionSound = NULL;}
// Empty destructor
Item::~Item(){}

bool Item::operator==(Item i)
{
	return (name==i.getName() && type==i.getType() && value==i.getValue() &&
		    nameSound==i.getNameSound() && actionSound==i.getActionSound());
}

// All explained above
string Item::getName(){return name;}
ItemType Item::getType(){return type;}
short Item::getValue(){return value;}
Sound* Item::getNameSound(){return nameSound;}
Sound* Item::getActionSound(){return actionSound;}
void Item::playNameSound(){if(nameSound!=NULL) nameSound->playAndWait(false);}
void Item::playActionSound(){if(actionSound!=NULL) actionSound->playAndWait(false);}

// free[]Sound: Needed for special deallocation of sounds by other classes
void Item::freeNameSound(){if(nameSound!=NULL) delete nameSound;}
void Item::freeActionSound(){if(actionSound!=NULL) delete actionSound;}
void Item::freeSounds(){freeNameSound();freeActionSound();}