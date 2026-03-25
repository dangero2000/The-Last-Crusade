/********************************************************
 *  Class: NumberReader									*
 *     By: Peter S. VanLund								*
 *   Desc: Class for reading numbers					*
 ********************************************************/

#include "Sound.h"

class NumberReader
{
	private:
		vector<Sound*> low, tens;	// low=0 to 19, tens=20-90
		Sound *hundred;				// 100
	public:
		NumberReader();				// Constructor
		~NumberReader();			// Destructor
		void say(int);				// Says a number
};

// Constructor
NumberReader::NumberReader()
{
	// Path to sounds
	string path = "./numbers/";
	int i;
	// Load numbers 0 to 19
	for(i = 0; i<20; i++)
	{
		string file = path;
		file += to_string(i);
		file += ".wav";
		low.push_back(new Sound(file));
	}
	// Load numbers 20-90 (tens)
	for(i = 0; i<8; i++)
	{
		string file = path;
		file += to_string(i+2);
		file += "0.wav";
		tens.push_back(new Sound(file));
	}
	// Load "hundred"
	hundred = new Sound("./numbers/hundred.wav");
}

// Destructor: Deallocate sounds
NumberReader::~NumberReader()
{
	int i;
	for(i = 0; i<low.size(); i++)
		delete low[i];
	for(i = 0; i<tens.size(); i++)
		delete tens[i];
	delete hundred;
}

// say: Says the number i
void NumberReader::say(int i)
{
	// Will not say negative numbers
	if(i<0) return;
	// Handle portion above 100
	if(i>=100)
	{
		say(i/100);
		hundred->playAndWait(false);
		if(i%100>0) say(i%100);
	}
	// Handle tens digit (if above 19)
	else if(i>19)
	{
		tens[i/10-2]->playAndWait(false);
		if(i%10>0) say(i%10);
	}
	// Handle numbers < 20
	else if(i<20)
	{
		low[i]->playAndWait(false);
	}
}