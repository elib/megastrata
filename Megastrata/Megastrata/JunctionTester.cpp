#include "JunctionTester.h"

#define MIN_BOX_WIDTH 0.3f
#define CONNECTOR_WIDTH (GRID_SIZE * 0.1f)

JunctionTester::JunctionTester(int typeID) : Generator(typeID)
{
}


void JunctionTester::SetVariables()
{
	m_variableCount = 0;
}

void JunctionTester::BuildCollection(Entity3dCollection *collection, WindowMapping &mapping, float height)
{

	int x, y;

	mapping.GetPhysicalCoordinates(0,0, x, y);
	if((x >= 1) && (x < MAX_ENTITIES -2) && (y >= 1) && (y < MAX_ENTITIES -2))
	{
		if(collection->AddEntity(id, mapping, RENDERED_JUNCTIONBOXES, x,y, 0))
		{
			ConnectionStruct s;
			s.Zero();
			s.west = 1;
			collection->SetEntityVariable(x, y, 0, 0.3f);

			if(collection->AddEntity(id, mapping, RENDERED_JUNCTIONBOXES, x+1,y, s))
				collection->SetEntityVariable(x+1, y, 0, 0.3f);
			
		}
	}



	//testing a 3x3 grid of connecting pieces. See haow it looks and collides.
	//note that the grid is fixed at 0,0. You won't see it at first, when you add the layer.

	////row 1
	//mapping.GetPhysicalCoordinates(-GRID_SIZE,-GRID_SIZE, x, y);
	//if((x >= 0) && (x < MAX_ENTITIES -1) && (y >= 0) && (y < MAX_ENTITIES -1))
	//{
	//	ent.Initialize(id, 1, 1, 0.3f);
	//	//ent.Connectivity.Connections.north = 1;
	//	ent.Connectivity.ConnectionsValue = 0;
	//	collection->AddEntity(ent, x,y, mapping);
	//}

	//mapping.GetPhysicalCoordinates(0,-GRID_SIZE, x, y);
	//if((x >= 0) && (x < MAX_ENTITIES -1) && (y >= 0) && (y < MAX_ENTITIES -1))
	//{
	//	ent.Initialize(id, 1, 1, 0.3f);
	//	//ent.Connectivity.Connections.north = 1;
	//	ent.Connectivity.ConnectionsValue = 0;
	//	collection->AddEntity(ent, x,y, mapping);
	//}

	//mapping.GetPhysicalCoordinates(GRID_SIZE,-GRID_SIZE, x, y);
	//if((x >= 0) && (x < MAX_ENTITIES -1) && (y >= 0) && (y < MAX_ENTITIES -1))
	//{
	//	ent.Initialize(id, 1, 1, 0.3f);
	//	//ent.Connectivity.Connections.north = 1;
	//	ent.Connectivity.ConnectionsValue = 0;
	//	collection->AddEntity(ent, x,y, mapping);
	//}

	////row 2

	//mapping.GetPhysicalCoordinates(-GRID_SIZE,0, x, y);
	//if((x >= 0) && (x < MAX_ENTITIES -1) && (y >= 0) && (y < MAX_ENTITIES -1))
	//{
	//	ent.Initialize(id, 1, 1, 0.3f);
	//	//ent.Connectivity.Connections.north = 1;
	//	ent.Connectivity.ConnectionsValue = 0;
	//	collection->AddEntity(ent, x,y, mapping);
	//}

	//mapping.GetPhysicalCoordinates(0,0, x, y);
	//if((x >= 0) && (x < MAX_ENTITIES -1) && (y >= 0) && (y < MAX_ENTITIES -1))
	//{
	//	ent.Initialize(id, 1, 1, 0.3f);
	//	ent.Connectivity.Connections.north = 1;
	//	ent.Connectivity.Connections.south = 1;
	//	ent.Connectivity.Connections.east = 1;
	//	ent.Connectivity.Connections.west = 1;
	//	collection->AddEntity(ent, x,y, mapping);
	//}

	//mapping.GetPhysicalCoordinates(GRID_SIZE,0, x, y);
	//if((x >= 0) && (x < MAX_ENTITIES -1) && (y >= 0) && (y < MAX_ENTITIES -1))
	//{
	//	ent.Initialize(id, 1, 1, 0.3f);
	//	//ent.Connectivity.Connections.north = 1;
	//	ent.Connectivity.ConnectionsValue = 0;
	//	collection->AddEntity(ent, x,y, mapping);
	//}

	////row 3
	//	mapping.GetPhysicalCoordinates(-GRID_SIZE,GRID_SIZE, x, y);
	//if((x >= 0) && (x < MAX_ENTITIES -1) && (y >= 0) && (y < MAX_ENTITIES -1))
	//{
	//	ent.Initialize(id, 1, 1, 0.3f);
	//	//ent.Connectivity.Connections.north = 1;
	//	ent.Connectivity.ConnectionsValue = 0;
	//	collection->AddEntity(ent, x,y, mapping);
	//}

	//mapping.GetPhysicalCoordinates(0,GRID_SIZE, x, y);
	//if((x >= 0) && (x < MAX_ENTITIES -1) && (y >= 0) && (y < MAX_ENTITIES -1))
	//{
	//	ent.Initialize(id, 1, 1, 0.3f);
	//	//ent.Connectivity.Connections.north = 1;
	//	ent.Connectivity.ConnectionsValue = 0;
	//	collection->AddEntity(ent, x,y, mapping);
	//}

	//mapping.GetPhysicalCoordinates(GRID_SIZE,GRID_SIZE, x, y);
	//if((x >= 0) && (x < MAX_ENTITIES -1) && (y >= 0) && (y < MAX_ENTITIES -1))
	//{
	//	ent.Initialize(id, 1, 1, 0.3f);
	//	//ent.Connectivity.Connections.north = 1;
	//	ent.Connectivity.ConnectionsValue = 0;
	//	collection->AddEntity(ent, x,y, mapping);
	//}



	//ent.Initialize(1, 1, 0.3f);
	////ent.Connectivity.Connections.north = 1;
	//ent.Connectivity.ConnectionsValue = 0;
	//collection->AddEntity(ent, 0,0, mapping);

	//ent.Initialize(1, 1, 0.3f);
	////ent.Connectivity.Connections.north = 1;
	//ent.Connectivity.ConnectionsValue = 0;
	//collection->AddEntity(ent, 1,0, mapping);

	//ent.Initialize(1, 1, 0.3f);
	//ent.Connectivity.Connections.east = 1;
	//collection->AddEntity(ent, 2,0, mapping);

	//ent.Initialize(1, 1, 0.3f);
	//ent.Connectivity.ConnectionsValue = 0;
	//collection->AddEntity(ent, 4, 4, mapping);
	//	
	//ent.Initialize(1, 1, 0.3f);
	//ent.Connectivity.ConnectionsValue = 0;
	//collection->AddEntity(ent, 4, 5, mapping);

	//ent.Initialize(1, 1, 0.3f);
	//ent.Connectivity.ConnectionsValue = 0;
	//collection->AddEntity(ent, 5, 4, mapping);

	//ent.Initialize(1, 1, 0.3f);
	//ent.Connectivity.ConnectionsValue = 0;
	//collection->AddEntity(ent, 5, 5, mapping);

	//collection->EnsureAdjacentConnectivity(0);

}