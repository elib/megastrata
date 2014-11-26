#include "Entity3dCollection.h"

#include "LayerManager.h"
#include "CollidableBatch.h"

#include "World.h"

Entity3dCollection::Entity3dCollection(void)
{
	Clear();
}


void Entity3dCollection::UnionTwoNodes(int x, int y, int deltax, int deltay)
{
	m_disjointSets.Union(
		m_disjointSets.FindSet(x + MAX_ENTITIES * y),
		m_disjointSets.FindSet(x + deltax + MAX_ENTITIES * (y + deltay)));
}

bool Entity3dCollection::AreDisjoint(int x, int y, int deltax, int deltay)
{
	return 
		m_disjointSets.FindSet(x + MAX_ENTITIES * y) !=
		m_disjointSets.FindSet(x + deltax + MAX_ENTITIES * (y + deltay));
}

//Run this after building the connection.
//Naturally-connected nodes will already exist in the disjoint sets structure (unioned together).
void Entity3dCollection::EnsureAdjacentConnectivity()
{
	for(int x = 0; x < MAX_ENTITIES; x++)
	{
		for(int y = 0; y < MAX_ENTITIES; y++)
		{
			if(m_entities[x][y].rendered_type == RENDERED_NONE)
				continue;

			if(m_entities[x][y].Connectivity.ConnectionsValue == 0)
				continue;

			if(
				(x > 0)
				&& (m_entities[x-1][y].rendered_type != RENDERED_NONE)  //other node exists
				&& (AreDisjoint(x, y, -1, 0))							  //other node disjoint
			  )
			{
				//connect.
				if(m_entities[x-1][y].Connectivity.ConnectionsValue != 0)
					m_entities[x-1][y].Connectivity.Connections.east = 1;
				m_entities[x][y].Connectivity.Connections.west = 1;
				UnionTwoNodes(x, y, -1, 0);
			}

			if(
				(x < MAX_ENTITIES -1)
				&& (m_entities[x+1][y].rendered_type != RENDERED_NONE)  //other node exists
				&& (AreDisjoint(x, y, 1, 0))							  //other node disjoint
			  )
			{
				//connect.
				if(m_entities[x+1][y].Connectivity.ConnectionsValue != 0)
					m_entities[x+1][y].Connectivity.Connections.west = 1;
				m_entities[x][y].Connectivity.Connections.east = 1;
				UnionTwoNodes(x, y, 1, 0);
			}

			if(
				(y > 0)
				&& (m_entities[x][y-1].rendered_type != RENDERED_NONE)  //other node exists
				&& (AreDisjoint(x, y, 0, -1))							  //other node disjoint
			  )
			{
				//connect.
				if(m_entities[x][y-1].Connectivity.ConnectionsValue != 0)
					m_entities[x][y-1].Connectivity.Connections.north = 1;
				m_entities[x][y].Connectivity.Connections.south = 1;
				UnionTwoNodes(x, y, 0, -1);
			}

			if(
				(y < MAX_ENTITIES - 1)
				&& (m_entities[x][y+1].rendered_type != RENDERED_NONE)  //other node exists
				&& (AreDisjoint(x, y, 0, 1))							  //other node disjoint
			  )
			{
				//connect.
				if(m_entities[x][y+1].Connectivity.ConnectionsValue != 0)
					m_entities[x][y+1].Connectivity.Connections.south = 1;
				m_entities[x][y].Connectivity.Connections.north = 1;
				UnionTwoNodes(x, y, 0, +1);
			}
		}
	}
}

void Entity3dCollection::Clear()
{
	m_disjointSets.Reset();

	m_scriptableCount = 0;
	
	active = false;

	for(int j = 0; j < MAX_ENTITIES; j++)
	{
		for(int k = 0; k < MAX_ENTITIES; k++)
		{
			m_entities[j][k].Initialize(-1, 1, 1, 1, RENDERED_NONE);
		}
	}
}

void Entity3dCollection::CompileDisplayLists(GLuint firstEntityList, float height)
{
	int texturecount = MIN(TEXTURE_COUNT, LayerManager::theLayerManager->GetArchitecturalTextureCount());

	ClearScriptables();

	//one more, baby (texturecount+1)! This time for the collisions.

	for(int tex = 0; tex < texturecount+1; tex++)
	{
		//build display list
		if(tex < texturecount)
			glNewList(firstEntityList + tex, GL_COMPILE);

		{
			AutoMatrixStack stack;

			glTranslatef(0, 0, height);

			for(int i = 0; i < MAX_ENTITIES; i++)
			{
				for(int j = 0; j < MAX_ENTITIES; j++)
				{
					RenderSingleNode(i, j, tex, tex == texturecount);
				}
			}
		}

		if(tex < texturecount)
			glEndList();
	}

	active = true;
}

void Entity3dCollection::Render(GLuint *textures, GLuint firstEntityList, GLuint scripttex)
{
	int texturecount = MIN(TEXTURE_COUNT, LayerManager::theLayerManager->GetArchitecturalTextureCount());

	{
		AutoMatrixStack stack;

		for(int i = 0; i < texturecount; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glCallList(firstEntityList + i);
		}
	}

	{
		AutoMatrixStack stack;

		//render scriptables
		glBindTexture(GL_TEXTURE_2D, scripttex);
		RenderScriptables();
	}
}


void Entity3dCollection::RenderSingleNode(int x, int y, int texture, bool shouldBakeCollisions)
{
	if(m_entities[x][y].rendered_type == RENDERED_NONE)
		return;

	int textureGroup = -1;

	if(!shouldBakeCollisions)
	{
		textureGroup = LayerManager::theLayerManager->GetTextureGroup(m_entities[x][y].GeneratedByID, texture);
		if(textureGroup == -1)
			return;
	}

	ConnectionStruct connections;
	DiscoverConnectedNodes(x, y, connections);

	ConnectionStruct adjacentTunnels;
	DiscoverConnectedNodes(x, y, adjacentTunnels, true);

	float heights[4];
	memset(heights, 0, sizeof(heights));
	GetAdjacentHeights(x, y, heights);

	EntityRenderedType types[4];
	memset(types, 0, sizeof(types));
	GetAdjacentTypes(x, y, types);

	if(!connections.east && !connections.west)
		int g = 0;

	vector<ScriptableEntity*> scriptables = m_entities[x][y].Render(connections, adjacentTunnels, types, textureGroup, shouldBakeCollisions, heights);
	for(int i = 0; i < scriptables.size(); i++)
		AddScriptable(scriptables[i]);
}

void Entity3dCollection::GetNodeExits(int x, int y, ConnectionStruct &exits)
{
	if(m_entities[x][y].rendered_type == RENDERED_NONE)
	{
		//for now allow traversal in empty space.
		exits.east = exits.north = exits.south = exits.west = 1;
		return;
	}

	ConnectionStruct extconns;
	DiscoverConnectedNodes(x, y, exits);

	//neighbor links only work if we're a "big box" or if we have an outgoing connection as well.
	if(m_entities[x][y].Connectivity.ConnectionsValue != 0)
	{
		exits.north &= m_entities[x][y].Connectivity.Connections.north;
		exits.east &= m_entities[x][y].Connectivity.Connections.east;
		exits.south &= m_entities[x][y].Connectivity.Connections.south;
		exits.west &= m_entities[x][y].Connectivity.Connections.west;
	}
}

//heights is a 4-element float array, to get the surrounding heights of the entities. 
void Entity3dCollection::GetAdjacentHeights(int x, int y, float *heights)
{
	int width  = m_entities[x][y].width;
	int length = m_entities[x][y].length;

	for(int i = 0; i < 4; i++)
	{
		int tar_x = x + ((width+1)/2) * dir_iter[i];
		int tar_y = y + ((length+1)/2) * dir_iter[(i+1)%4];

		if((tar_x >= 0) && (tar_y >= 0)
			&& (tar_x <= MAX_ENTITIES -1) && (tar_y <= MAX_ENTITIES -1))
		{
			Entity3d *point;
			if(m_entities[tar_x][tar_y].m_entityOwner != NULL)
				point = m_entities[tar_x][tar_y].m_entityOwner;
			else
				point = &m_entities[tar_x][tar_y];

			heights[i] = point->variables[0];
		}
	}

}

void Entity3dCollection::GetAdjacentTypes(int x, int y, EntityRenderedType *types)
{
	int width  = m_entities[x][y].width;
	int length = m_entities[x][y].length;

	for(int i = 0; i < 4; i++)
	{
		int tar_x = x + ((width+1)/2) * dir_iter[i];
		int tar_y = y + ((length+1)/2) * dir_iter[(i+1)%4];

		if((tar_x >= 0) && (tar_y >= 0)
			&& (tar_x <= MAX_ENTITIES -1) && (tar_y <= MAX_ENTITIES -1))
		{
			Entity3d *point;
			if(m_entities[tar_x][tar_y].m_entityOwner != NULL)
				point = m_entities[tar_x][tar_y].m_entityOwner;
			else
				point = &m_entities[tar_x][tar_y];

			types[i] = point->rendered_type;
		}
	}

}


void Entity3dCollection::DiscoverConnectedNodes(int x, int y, ConnectionStruct &connections, bool excludeBoxes)
{
	connections.Zero();
	int width  = m_entities[x][y].width;
	int length = m_entities[x][y].length;

	for(int i = 0; i < 4; i++)
	{
		int tar_x = x + ((width+1)/2) * dir_iter[i];
		int tar_y = y + ((length+1)/2) * dir_iter[(i+1)%4];

		if((tar_x >= 0) && (tar_y >= 0)
			&& (tar_x <= MAX_ENTITIES -1) && (tar_y <= MAX_ENTITIES -1))
		{
			Entity3d *point;
			if(m_entities[tar_x][tar_y].m_entityOwner != NULL)
				point = m_entities[tar_x][tar_y].m_entityOwner;
			else
				point = &m_entities[tar_x][tar_y];

			//false be default.
			connections.SetByIndex(i, 0);

			if(point->rendered_type != RENDERED_NONE)
			{
				if((point->Connectivity.Connections.GetByIndex((i+2) % 4)))
				{
					//exit is this way - adjacent cell is pointing in our direction.
					connections.SetByIndex(i, 1);
				}

				//if we just mean to find the corridors, remove shafts as well
				if(excludeBoxes && point->rendered_type == RENDERED_SHAFT)
					connections.SetByIndex(i, 0);

				if(!excludeBoxes)
				{
					//also, boxes are connected. If desired.
					if(point->Connectivity.ConnectionsValue == 0)
						connections.SetByIndex(i, 1);
				}
			}
		}
	}
}

bool Entity3dCollection::AddEntity(int generator_id, WindowMapping &mapping, EntityRenderedType m_targetType,
								   int x, int y, ConnectionStruct &connectivity, int spanX, int spanY, int spanZ)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < MAX_ENTITIES);
	assert(y < MAX_ENTITIES);

	//get physical coordinates
	float xpos = x, ypos = y;
	mapping.GetWorldCoordinates(xpos, ypos);

	//ensure we are on our subgrid.
	float xmod = (round(xpos) / (GRID_SIZE*spanX)) - floor(round(xpos) / (GRID_SIZE*spanX));
	float ymod = (round(ypos) / (GRID_SIZE*spanY)) - floor(round(ypos) / (GRID_SIZE*spanY));

	if(ABS(xmod) > EPSILON)
		return false;

	if(ABS(ymod) > EPSILON)
		return false;

	int xstart, ystart, xlim, ylim;
	xstart = MAX(0, x - (spanX - 1)/2);
	xlim = MIN(mapping.GetPhysicalWidth() - 1, x + (spanX - 1)/2);
	ystart = MAX(0, y - (spanY - 1)/2);
	ylim = MIN(mapping.GetPhysicalHeight() - 1, y + (spanY - 1)/2);

	bool empty = true;
	//checking if grid is empty
	for(int k = xstart; k <= xlim; k++)
	{
		for(int l = ystart; l <= ylim; l++)
		{
			empty &= (m_entities[k][l].rendered_type == RENDERED_NONE);
		}
	}

	if(!empty)
		return false;

	//empty.
	//filling in the grid
	for(int k = xstart; k <= xlim; k++)
	{
		for(int l = ystart; l <= ylim; l++)
		{
			if(!(k == x && l == y))
			{
				xpos = x, ypos = y;
				mapping.GetWorldCoordinates(xpos, ypos);
				m_entities[k][l].Initialize(generator_id, 1, 1, 1, RENDERED_PADDING);
				m_entities[k][l].xpos = xpos;
				m_entities[k][l].ypos = ypos;
				m_entities[k][l].m_entityOwner = &m_entities[x][y];

				//union these nodes, kthx
				UnionTwoNodes(x, y, k - x, l - y);
			}
		}
	}
	
	//and again.
	xpos = x;
	ypos = y;
	mapping.GetWorldCoordinates(xpos, ypos);
	m_entities[x][y].Initialize(generator_id, spanX, spanY, spanZ, m_targetType);
	m_entities[x][y].xpos = xpos;
	m_entities[x][y].ypos = ypos;
	m_entities[x][y].Connectivity.Connections = connectivity;

	ConnectionStruct connections;
	DiscoverConnectedNodes(x, y, connections);

	if(m_entities[x][y].Connectivity.ConnectionsValue)
	{
		//not a boxy structure - corridors.
		//(no need to worry about box connectivity, they never get changed anyway)
		//the strategy is to connect TO boxes.

		if((connections.east) && (m_entities[x][y].Connectivity.Connections.east))
			UnionTwoNodes(x, y, (spanX+1)/2, 0);
		if((connections.west) && (m_entities[x][y].Connectivity.Connections.west))
			UnionTwoNodes(x, y, -(spanX+1)/2, 0);
		if((connections.north) && (m_entities[x][y].Connectivity.Connections.north))
			UnionTwoNodes(x, y, 0, (spanY+1)/2);
		if((connections.south) && (m_entities[x][y].Connectivity.Connections.south))
			UnionTwoNodes(x, y, 0, -(spanY+1)/2);
	}

	return true;

}

bool Entity3dCollection::AddEntity(int generator_id, WindowMapping &mapping, EntityRenderedType m_targetType, int center_x, int center_y,
		unsigned char connectivityValue, int spanX , int spanY , int spanZ )
{
	ConnectionStruct conn;
	ConnectionStruct *thesame = &conn;
	*((unsigned char*)(thesame)) = connectivityValue;
	return AddEntity(generator_id, mapping, m_targetType, center_x, center_y, conn, spanX, spanY, spanZ);
}

void Entity3dCollection::SetEntityVariable(int x, int y, int varindex, float var)
{
	m_entities[x][y].variables[varindex] = var;
}

float Entity3dCollection::GetEntityVariable(int x, int y, int index)
{
	return m_entities[x][y].variables[0];
}

void Entity3dCollection::ClearScriptables()
{
	for(int i = 0; i < m_scriptableCount; i++)
		delete m_scriptables[i];

	m_scriptableCount = 0;
}

void Entity3dCollection::RenderScriptables()
{
	for(int i = 0; i < m_scriptableCount; i++)
	{
		m_scriptables[i]->Render();
	}
}

void Entity3dCollection::UpdateScriptables(ElapsedTime absolutetime)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for(int i = 0; i < m_scriptableCount; i++)
	{
		m_scriptables[i]->Update(absolutetime);
	}
}

void Entity3dCollection::AddScriptable(ScriptableEntity *entity)
{
	if(m_scriptableCount <= MAX_SCRIPTABLE - 1)
	{
		entity->collection_id = (int)this;
		m_scriptables[m_scriptableCount++] = entity;
	}
}

void Entity3dCollection::GetExtraCollidables(CollidableBatch *batch)
{
	for(int i = 0; i < m_scriptableCount; i++)
	{
		m_scriptables[i]->GetCollidables(batch);
	}
}

Entity3dCollection::~Entity3dCollection()
{
	ClearScriptables();
}

void Entity3dCollection::GetCollidables(CollidableBatch *batch, int gridx, int gridy, WindowMapping &mapping, bool onlyScriptables)
{
	assert(gridx >= 0);
	assert(gridx < MAX_ENTITIES);
	assert(gridy >= 0);
	assert(gridy < MAX_ENTITIES);

	assert(gridx < mapping.GetPhysicalWidth());
	assert(gridy < mapping.GetPhysicalHeight());

	int xstart, ystart, xlim, ylim;
	xstart = MAX(0, gridx - 1);
	xlim = MIN(mapping.GetPhysicalWidth() - 1, gridx + 1);
	ystart = MAX(0, gridy - 1);
	ylim = MIN(mapping.GetPhysicalHeight() - 1, gridy + 1);

	vector<int> exclusion_list;

	if(!onlyScriptables)
	{
		//collect 9 Entity3ds of collidables into a batch
		for(int k = xstart; k <= xlim; k++)
		{
			for(int l = ystart; l <= ylim; l++)
			{
				//int k = gridx, l = gridy;
				m_entities[k][l].GetCollidables(batch, &exclusion_list);
			}
		}
	}

	//also get collidables from the scriptables:
	GetExtraCollidables(batch);
}