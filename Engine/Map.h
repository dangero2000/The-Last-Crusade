/********************************************************
 *  Class: Map											*
 *     By: Peter S. VanLund								*
 *   Desc: Defines a Map object and its operations		*
 ********************************************************/

#include "Player.h"
#include "MapStruct.h"

class Map
{
	private:
		string mapName;						// Map name
		Node *start,*end,*cursor,*previous;	// Node pointers
		vector<Node*>  vecNode;				// Vector of nodes
		vector<Sound*> vecSound;			// Vector of sounds
		vector<Item*>  vecItem;				// Vector of items
		vector<NPC*>   vecNPC;				// Vector of NPCs
		Sound          *mapTitle;			// Map title sound
		int posX, posY;						// Current position
	public:
		// Constructor & Destructor
		Map(string);
		~Map();
		void loadMap();						// Load map into memory from disk
		void freeMap();						// Free map (deallocate memory)
		bool move(Direction);				// Move in a certain direction
		void movePrevious();				// Move to previous node
		void activate();					// Activate map
		void deactivate();					// Deactivate map
		int   getNodeCount();				// Get # of nodes
		Node* getNode(int);					// Get node
		Node* getCurrentNode();				// Get current node
		int   getCurrentNodeNumber();		// Get current node index
		void  setCurrentNodeNumber(int);	// Set current node by index
		NPC*  getNPC(int);					// Get NPC
		Item* getItem(int);					// Get Item
		int   getItemCount();				// Get # of items
		int getItemNumber(Item*);			// Get item index
		int getNPCNumber(NPC*);				// Get NPC index
		bool atEndOfMap();					// End of map test
		void sayMapName();					// Say map name
};

// Constructor & Destructor
Map::Map(string s)
{
	// Set defaults
	start = end = cursor = previous = NULL;
	mapName=s;
	mapName[mapName.find(".map")] = '\0';
	mapTitle = NULL;
}
Map::~Map()
{
	// Deallocate memory
	freeMap();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                               loadMap: Reads map file into memory                                //
//////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::loadMap()																					//
{																									//
	// Set filename																					//
	string filename = mapName.c_str();																//
	filename += ".map";																				//
																									//
	// Map Title sound																				//
	string titlePath = mapName.c_str();																//
	titlePath += ".sounds/";																		//
	titlePath += mapName.c_str();																	//
	titlePath += ".wav";																			//
	mapTitle = new Sound(titlePath);																//
																									//
	// Arbitrary variables																			//
	int i,j;																						//
																									//
	// Open file																					//
	FILE *file = fopen(filename.c_str(),"rb");														//
	if(file==NULL)																					//
	{																								//
		string error = filename.c_str();															//
		error += " not found!";																		//
		fprintf(stderr, "Map Reading Error: %s\n", error.c_str());									//
		return;																						//
	}																								//
																									//
	// Map header																					//
	MAPHEADER mh;																					//
	fread(&mh,sizeof(MAPHEADER),1,file);															//
																									//
	// Sound data																					//
	for(i = 0; i<mh.mSoundCount; i++)																//
	{																								//
		SOUNDDATA sd;																				//
		fread(&sd,sizeof(SOUNDDATA),1,file);														//
		string filename = sd.sName;																	//
		string path = mapName.c_str();																//
		path += ".sounds/";																		//
		path += filename;																			//
		Sound *newSound = (sd.sSpatial) ? new Sound(path,sd.sXCoord,sd.sYCoord) : new Sound(path);	//
		vecSound.push_back(newSound);																//
	}																								//
																									//
	// Item data																					//
	for(i = 0; i<mh.mItemCount; i++)																//
	{																								//		
		ITEMDATA id;																				//
		fread(&id,sizeof(ITEMDATA),1,file);															//
		Sound *nameSound   = (id.iNameSound  ==-1) ? NULL : vecSound[id.iNameSound]  ;				//
		Sound *actionSound = (id.iActionSound==-1) ? NULL : vecSound[id.iActionSound];				//
		Item *newItem = new Item(id.iName,id.iType,id.iValue,nameSound,actionSound);				//
		vecItem.push_back(newItem);																	//
	}																								//
																									//
	// NPC data																						//
	for(i = 0; i<mh.mNPCCount; i++)																	//
	{																								//
		NPCDATA cd;
		fread(&cd,sizeof(NPCDATA),1,file);															//
		Sound *nameSound   = (cd.cNameSound  ==-1) ? NULL : vecSound[cd.cNameSound]  ;				//
		Sound *actionSound = (cd.cActionSound==-1) ? NULL : vecSound[cd.cActionSound];				//
		NPC *newNPC = new NPC(cd.cName,cd.cType,cd.cStrMin,cd.cStrMax,cd.cDefMin,cd.cDefMax,		//
							  cd.cHPMin,cd.cHPMax,cd.cRunPerc,nameSound,actionSound);				//
		// NPC items																				//
		for(j = 0; j<cd.cItemCount; j++)															//
		{																							//
			NPCITEM ci;																				//
			fread(&ci,sizeof(NPCITEM),1,file);														//														//
			newNPC->addItem(vecItem[ci.cItem],ci.cPercent);
		}																							//
		vecNPC.push_back(newNPC);																	//
	}																								//
																									//
	// Node data																					//
	for(i = 0; i<mh.mNodeCount; i++)																//
		vecNode.push_back(new Node());																//
	for(i = 0; i<mh.mNodeCount; i++)																//
	{																								//
		NODEHEADER nh;																				//
		fread(&nh,sizeof(NODEHEADER),1,file);														//
		if(nh.nNorth!=-1) vecNode[i]->setNeighbor(vecNode[nh.nNorth],NORTH);						//
		if(nh.nSouth!=-1) vecNode[i]->setNeighbor(vecNode[nh.nSouth],SOUTH);						//
		if(nh.nEast !=-1) vecNode[i]->setNeighbor(vecNode[nh.nEast ],EAST );						//
		if(nh.nWest !=-1) vecNode[i]->setNeighbor(vecNode[nh.nWest ],WEST );						//
		// TODO: SET IMAGE																			//
		// Music																					//
		for(j = 0; j<nh.nMusicCount; j++)															//
		{																							//
			NODESOUND ns;																			//
			fread(&ns,sizeof(NODESOUND),1,file);													//
			vecNode[i]->addSound(vecSound[ns.nSound]);												//
		}																							//
		// Items																					//
		for(j = 0; j<nh.nItemCount; j++)															//
		{																							//
			NODEITEM ni;																			//
			fread(&ni,sizeof(NODEITEM),1,file);														//
			if(Random::checkOdds(ni.nPercent))														//
				vecNode[i]->addItem(vecItem[ni.nItem]);												//
		}																							//
		// NPCs																						//
		short odds = 0, r = Random::getShort(0,99);													//
		bool foundNPC = false;
		for(j = 0; j<nh.nNPCCount; j++)																//
		{																							//
			NODENPC nc;																				//
			fread(&nc,sizeof(NODENPC),1,file);														//
			if(foundNPC) continue;
			odds+=nc.nPercent;																		//
			if(r<odds)																				//
			{																						//
				vecNode[i]->addNPC(vecNPC[nc.nNPC]);												//
				foundNPC = true;
			}																						//
		}																							//
		// Required items																			//
		for(j = 0; j<nh.nReqItemCount; j++)															//
		{																							//
			NODEREQITEM nr;																			//
			fread(&nr,sizeof(NODEREQITEM),1,file);													//
			vecNode[i]->addRequiredItem(vecItem[nr.nReqItem]);										//
		}																							//
	}																								//
																									//
	// Set start and end																			//
	start = cursor = vecNode[mh.mStart];															//
	end   = vecNode[mh.mEnd];																		//
																									//
	// Close file																					//
	fclose(file);																					//
}																									//
//////////////////////////////////////////////////////////////////////////////////////////////////////

// freeMap: Deallocate memory
void Map::freeMap()
{
	while(vecNode.size() > 0)
	{
		delete vecNode[0];
		vecNode.erase(vecNode.begin());
	}
	while(vecSound.size() > 0)
	{
		delete vecSound[0];
		vecSound.erase(vecSound.begin());
	}
	while(vecItem.size() > 0)
	{
		delete vecItem[0];
		vecItem.erase(vecItem.begin());
	}
	while(vecNPC.size() > 0)
	{
		delete vecNPC[0];
		vecNPC.erase(vecNPC.begin());
	}
	if(mapTitle!=NULL)
	{
		delete mapTitle;
		mapTitle = NULL;
	}
}

// move: Move in a certain direction
bool Map::move(Direction d)
{
	Node *next = cursor->getNeighbor(d);
	// Return false if no node in that direction
	if(next==NULL) return false;
	// Change cursor
	previous = cursor;
	cursor   = next;
	// Change position
	switch(d)
	{
		case NORTH: posY++; break;
		case SOUTH: posY--; break;
		case EAST : posX++; break;
		case WEST : posX--; break;
	}
	// Change spatial sound listener
	Sound::setListener(posX,posY);
	// Move successful
	return true;
}

// movePrevious: Move to previous node
void Map::movePrevious()
{
	cursor = previous;
}

// activate: Activate current map (start spatial sounds)
void Map::activate()
{
	int i;
	posX = posY = 0;
	// Start spatial sounds
	Sound::setListener(posX,posY);
	for(i = 0; i<vecSound.size(); i++)
		if(vecSound[i]->isSpatial()) vecSound[i]->play(true);
}

// deactivate: Stop all sounds
void Map::deactivate()
{
	int i;
	for(i = 0; i<vecSound.size(); i++)
		if(vecSound[i]->isPlaying()) vecSound[i]->stop();
}

// getNodeCount: Get # of nodes
int Map::getNodeCount(){return vecNode.size();}

// getNode: Get node i
Node* Map::getNode(int i)
{
	if(i>vecNode.size()-1) return NULL;
	return vecNode[i];
}

// getCurrentNode: Get current node
Node* Map::getCurrentNode(){return cursor;}

// getCurrentNodeNumber: Returns index of current node
int Map::getCurrentNodeNumber()
{
	for(int i = 0; i<vecNode.size(); i++)
		if(vecNode[i]==cursor) return i;
	return -1;
}

// setCurrentNodeNumber: Set current node by index
void Map::setCurrentNodeNumber(int n)
{
	cursor = previous = vecNode[n];
}

// getItem: Get item i
Item* Map::getItem(int i)
{
	if(i>vecItem.size()-1) return NULL;
	return vecItem[i];
}

// getItemCount: Get # of items
int Map::getItemCount(){return vecItem.size();}

// getNPC: Get NPC
NPC* Map::getNPC(int i)
{
	if(i>vecNPC.size()-1) return NULL;
	return vecNPC[i];
}

// getItemNumber: Get index of item
int Map::getItemNumber(Item *item)
{
	for(int i = 0; i<vecItem.size(); i++)
		if(item==vecItem[i]) return i;
	return -1;
}

// getNPCNumber: Get index of npc
int Map::getNPCNumber(NPC *npc)
{
	for(int i = 0; i<vecNPC.size(); i++)
		if(npc->getName()==vecNPC[i]->getName()) return i;
	return -1;
}

// atEndOfMap: End of map test
bool Map::atEndOfMap(){return end==cursor;}

// sayMapName: Say map name
void Map::sayMapName(){mapTitle->playAndWait(false);}