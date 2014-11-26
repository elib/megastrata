#include "GeneratedEntities.h"
#include "World.h"
#include "LayerManager.h"

GLubyte SomeColors[TEXTURE_COUNT][3] = {
	{140,	151,	155},
	{202,	132,	0},
	{132,	74,		47},
	{140,	151,	155},
	{202,	132,	0},
	{132,	74,		47},
	{0x44, 0x44, 0},
	{0x44, 0, 0x44},
	{0x44, 0x44, 0},
	{0x44, 0, 0x44}
};

GLubyte ScriptColors[2][3] = {
	{0,	64,	64},
	{0,	64,	64}
};

GeneratedEntities::GeneratedEntities()
{
	m_EntityCollections[0] = new Entity3dCollection();
	m_EntityCollections[1] = new Entity3dCollection();

	m_WindowSizeX = m_WindowSizeY = 8;

	m_EntityCollections[0]->Clear();
	m_EntityCollections[1]->Clear();

	m_entityTextures[0] = -1;
}

void GeneratedEntities::InitializeTextures()
{
	if(m_entityTextures[0] != -1)
		return;

	m_firstEntityList[0] = glGenLists(TEXTURE_COUNT);
	m_firstEntityList[1] = glGenLists(TEXTURE_COUNT);

	//generate textures
	glGenTextures(TEXTURE_COUNT, m_entityTextures);
	glGenTextures(2, m_scriptableTextures);

	for(int i = 0; i < TEXTURE_COUNT; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_entityTextures[i]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Min Filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Mag Filter

		glTexImage2D(GL_TEXTURE_2D, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, SomeColors[i]);
	}

	for(int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_scriptableTextures[i]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Min Filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Mag Filter

		glTexImage2D(GL_TEXTURE_2D, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, ScriptColors[i]);
	}
}

GeneratedEntities::~GeneratedEntities(void)
{
	delete [] m_EntityCollections;
}


void GeneratedEntities::Render()
{
	{
		AutoMatrixStack stack;

		if(BetweenLevels())
			m_EntityCollections[1]->Render(m_entityTextures, m_firstEntityList[1], m_scriptableTextures[1]);

		m_EntityCollections[0]->Render(m_entityTextures, m_firstEntityList[0], m_scriptableTextures[0]);
	}
}

void GeneratedEntities::Update(bool force)
{
	InitializeTextures();

	if(FillCollection(force))
	{
		float height = floor(m_spaceWindow.GetLevelHeight());

		if(BetweenLevels())
			m_EntityCollections[1]->CompileDisplayLists(m_firstEntityList[1], World::theWorld->GetLevelPhysicalHeight(height + 1));

		m_EntityCollections[0]->CompileDisplayLists(m_firstEntityList[0], World::theWorld->GetLevelPhysicalHeight(height));

		UpdateScriptables();
	}
}

bool GeneratedEntities::ConstrainToIndoors(Vector3d &originalPoint, Vector3d &delta)
{
	//current grid
	float gridx, gridy;
	World::theWorld->GetGridContainingPoint(originalPoint, gridx, gridy);

	//transform to physical coordinates
	int x, y;
	m_spaceWindow.GetPhysicalCoordinates(gridx, gridy, x, y);

	//got it - we need to get the batches here
	CollidableBatch batch;
	
	if(m_EntityCollections[1]->active)
	{
		//just get the collidables from the scriptables. [ables]
		m_EntityCollections[1]->GetCollidables(&batch, x, y, m_spaceWindow, true);
	}

	m_EntityCollections[0]->GetCollidables(&batch, x, y, m_spaceWindow);

	return batch.Collide(originalPoint, delta);
}

bool GeneratedEntities::FillCollection(bool force)
{
	//get new space window
	RefreshEntitiesCommand changedWindow = SetSpaceWindow();

	//decide what needs to be refreshed
	if((changedWindow == NO_REFRESH) && !force) return false;

	float height;
	bool toprefresh = false, bottomrefresh = false;

	switch(changedWindow)
	{
	case REFRESH_ABOVE:
		{
			if(m_EntityCollections[1]->active)
			{
				//swap bottom and top, so we can continue upwards.
				LOG("Swapping top-bottom.");

				Entity3dCollection* temp;
				GLuint listtemp;

				temp = m_EntityCollections[1];
				m_EntityCollections[1] = m_EntityCollections[0];
				m_EntityCollections[0] = temp;

				listtemp = m_firstEntityList[1];
				m_firstEntityList[1] = m_firstEntityList[0];
				m_firstEntityList[0] = listtemp;
			}

			toprefresh = true;
		}
		break;
	case REFRESH_BELOW:
		{
			
			//swap top and bottom, so we can continue downwards.
			Entity3dCollection* temp;
			GLuint listtemp;

			temp = m_EntityCollections[1];
			m_EntityCollections[1] = m_EntityCollections[0];
			m_EntityCollections[0] = temp;

			listtemp = m_firstEntityList[1];
			m_firstEntityList[1] = m_firstEntityList[0];
			m_firstEntityList[0] = listtemp;

			bottomrefresh = true;
		}
		break;
	case NO_REFRESH:
	case REFRESH_ALL:
	default:
		//if we are here and it's "no refresh", force == true, so refresh all.
		toprefresh = bottomrefresh = true;
		break;
	}

	if(toprefresh && BetweenLevels())
	{
		LOG("Refreshing top");
		m_EntityCollections[1]->Clear();
		//get height in noise units:
		height = floor(m_spaceWindow.GetLevelHeight() + 1);

		LayerManager::theLayerManager->GenerateEntities(m_EntityCollections[1], m_spaceWindow, height);
		m_EntityCollections[1]->EnsureAdjacentConnectivity();
	}
	else
	{
		if(!BetweenLevels())
		{
			//"clear" sets as inactive
			m_EntityCollections[1]->Clear();
		}
	}

	if(bottomrefresh)
	{
		LOG("Refreshing bottom");
		m_EntityCollections[0]->Clear();
		//get height in noise units:
		height = floor(m_spaceWindow.GetLevelHeight());
		LayerManager::theLayerManager->GenerateEntities(m_EntityCollections[0], m_spaceWindow, height);
		m_EntityCollections[0]->EnsureAdjacentConnectivity();
	}

	return true;
}

RefreshEntitiesCommand GeneratedEntities::SetSpaceWindow()
{

	//when the point of interest moves more then 1/window_move_factor, we move the window.
	int window_move_factor = 4;

	//keep divisible by window_move_factor:

	while(m_WindowSizeX > 0
		&&
		(m_WindowSizeX % window_move_factor != 0
		|| m_WindowSizeX > MAX_ENTITIES))
	{
		m_WindowSizeX--;
	}

	while(m_WindowSizeY > 0
		&&
		(m_WindowSizeY % window_move_factor != 0
		|| m_WindowSizeY > MAX_ENTITIES))
	{
		m_WindowSizeY--;
	}

	if(m_WindowSizeX <= 0)
	{
		m_WindowSizeX = window_move_factor;
	}

	if(m_WindowSizeY <= 0)
	{
		m_WindowSizeY = window_move_factor;
	}

	//find size of space window
	float x_width = m_WindowSizeX * GRID_SIZE;
	float y_width = m_WindowSizeY * GRID_SIZE;

	float x, y;
	World::theWorld->GetWindowCenter(x, y);

	float x_center, y_center;

	float x_newGrid = GRID_SIZE * x_width*(1/(float)window_move_factor);
	float y_newGrid = GRID_SIZE * y_width*(1/(float)window_move_factor);

	x_center = x_newGrid * floor(x / x_newGrid);
	y_center = y_newGrid * floor(y / y_newGrid);

	WindowMapping tempWindow;
	tempWindow.SetPhysicalWindow(m_WindowSizeX, m_WindowSizeY);
	tempWindow.SetWorldWindow(-y_width/2 + y_center, x_width/2 + x_center, -x_width/2 + x_center, y_width/2 + y_center);
	tempWindow.SetLevelHeight(World::theWorld->GetCurrentHeight());

	//no changes - nothing should happen. Window is snapped to grid points.
	if(tempWindow == m_spaceWindow)
		return NO_REFRESH;

	LOG("New space window: (%.2f, %.2f), (%.2f, %.2f) height: %.2f", -x_width/2 + x_center, -y_width/2 + y_center, x_width/2 + x_center, y_width/2 + y_center, tempWindow.GetLevelHeight());

	//now we examine the difference. If we move in XY direction, we must render as usual.
	if(ABS(tempWindow.GetLevelHeight() - m_spaceWindow.GetLevelHeight()) < EPSILON)
	{
		m_spaceWindow = tempWindow;
		return REFRESH_ALL;
	}
	else
	{
		if(tempWindow.GetLevelHeight() - m_spaceWindow.GetLevelHeight() > EPSILON)
		{
			//new window is higher
			m_spaceWindow = tempWindow;
			return REFRESH_ABOVE;
		}
		else
		{
			//new window is lower
			m_spaceWindow = tempWindow;
			return REFRESH_BELOW;
		}
	}
}

void GeneratedEntities::PerformTests(WindowMapping &mapping)
{
#ifdef _DEBUG
	//performing some "unit testing"
	for(int i = 0; i < mapping.GetPhysicalWidth(); i++)
	{
		for(int j = 0; j < mapping.GetPhysicalHeight(); j++)
		{
			float xpos = i, ypos = j;
			float xtest = xpos, ytest = ypos;
			mapping.GetWorldCoordinates(xtest, ytest);
			int xres, yres;
			mapping.GetPhysicalCoordinates(xtest, ytest, xres, yres);

			if((i != xres) || (j != yres))
				LOG("BAD! (%d, %d) => (%f, %f) => (%d, %d)", i, j, xtest, ytest, xres, yres);
		}
	}
#endif
}

bool GeneratedEntities::SetWindowSize(int newWindowSize)
{
	if(newWindowSize >=2 && m_WindowSizeX != newWindowSize)
	{
		m_WindowSizeX = m_WindowSizeY = newWindowSize;
		return true;
	}

	return false;
}

bool GeneratedEntities::BetweenLevels()
{
	float height = m_spaceWindow.GetLevelHeight();
	return(height != floor(height));
}

float GeneratedEntities::GetEntityHeight(float gridx, float gridy)
{
	int x, y;
	m_spaceWindow.GetPhysicalCoordinates(gridx, gridy, x, y);
	if((x >= 0) && (y >= 0)	&& (x < MAX_ENTITIES) && (y < MAX_ENTITIES))
	{
		return m_EntityCollections[0]->GetEntityVariable(x, y, 0);
	}

	return 1.0f;
}

void GeneratedEntities::UpdateScriptables()
{
	World::theWorld->ClearDebugObjects();
	ElapsedTime absolutetime = World::theWorld->GetAbsoluteTime();

	if(m_EntityCollections[0]->active)
		m_EntityCollections[0]->UpdateScriptables(absolutetime);

	if(m_EntityCollections[1]->active)
		m_EntityCollections[1]->UpdateScriptables(absolutetime);
}