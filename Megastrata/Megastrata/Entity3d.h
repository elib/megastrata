#pragma once
#include <utils.h>
#include "Vector3d.h"

#include "CollidableBatch.h"
#include "CollidableCollection.h"

#include <vector>

#include "ScriptableEntity.h"

#define ENTITY3D_VAR_COUNT 10

enum EntityRenderedType
{
	RENDERED_NONE,
	RENDERED_JUNCTIONBOXES,
	RENDERED_DUMMY,
	RENDERED_SHAFT,

	RENDERED_PADDING

};

enum Direction
{
	NORTH,EAST,SOUTH,WEST
};

typedef struct __BEVEL
{
	unsigned northeast	: 2;
	unsigned southeast	: 2;
	unsigned southwest	: 2;
	unsigned northwest	: 2;

	unsigned GetByIndex(int ind)
	{
		switch(ind % 4)
		{
		case 0:
			return northeast;
		case 1:
			return southeast;
		case 2:
			return southwest;
		case 3:
			return northwest;
		}
	}

} BevelInfo;

const int dir_iter[] = {0, 1, 0, -1}; //for x, y is +1 offset

typedef struct __CONNECTIONS
{
	unsigned north	: 1; //u =  1
	unsigned east	: 1; //v = -1
	unsigned south	: 1; //u =  1
	unsigned west	: 1; //v = -1

	unsigned _____padding : 4;

	void Zero()
	{
		north = east = south = west = 0;
	}

	unsigned GetByIndex(int ind)
	{
		switch(ind % 4)
		{
		case 0:
			return north;
		case 1:
			return east;
		case 2:
			return south;
		case 3:
			return west;
		}
	}

	void SetByIndex(int ind, unsigned value)
	{
		switch(ind % 4)
		{
		case 0:
			north = value;
			break;
		case 1:
			east = value;
			break;
		case 2:
			south = value;
			break;
		case 3:
			west = value;
			break;
		}
	}

} ConnectionStruct;

class Entity3d
{
public:

	union
	{
		ConnectionStruct Connections;
		unsigned char ConnectionsValue;
	} Connectivity;

	EntityRenderedType rendered_type;
	float xpos, ypos;

	//now in grid units
	int width, length, height;
	void Initialize(int generatorId, int w, int l, int h, EntityRenderedType = RENDERED_JUNCTIONBOXES);

	vector<ScriptableEntity*> Render(ConnectionStruct externalConnections, ConnectionStruct adjacentTunnels, 
				EntityRenderedType *adjTypes, int textureGroup, bool shouldBakeCollisions, float *heights);

	void GetCollidables(CollidableBatch *batch, vector<int> *exclusion_list);

	Entity3d(void);

	//make into list?
	float variables[ENTITY3D_VAR_COUNT];

	int GeneratedByID;
	Entity3d *m_entityOwner;
protected:
	
	//"standard" ways
	void DrawTopAndBottom(int textureGroup);
	void DrawSideFace(Direction dir, int textureGroup);

	/**
	* slightly more raw - make sure you are in the right mode
	* (check texturegroup beforehand
	*/
	void DrawFace(float yangle, float zangle = 0);
	
	/**
	* slightly more raw - make sure you are in the right mode
	* (check texturegroup beforehand
	*/
	void DoQuad();
	void CreateQuad(Vector3d *corners, int tessellation_u, int tessellation_v);

	void RenderJunctionBoxes(ConnectionStruct externalConnections, ConnectionStruct adjacentTunnels, int textureGroup, float *heights);
	vector<ScriptableEntity*> RenderShaft(ConnectionStruct externalConnections, ConnectionStruct adjacentTunnels, int textureGroup, float *heights, EntityRenderedType *adjTypes);
	void RenderDummy();

	CollidableCollection m_collidables;

	bool m_bakingCollisions;

	int id;
	static int ____last_id;
};