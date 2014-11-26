#include "World.h"
#include "SubGenerator.h"

#include "Timer.h"

World* World::theWorld = 0;

World::World(void)
{
//	m_currentHeight = 0;

	m_visualmode = VISUAL_TOPDOWN;
	m_rendermode = RENDER_SOLID;

	assert(theWorld == 0);
	theWorld = this;

	ClearDebugObjects();

	m_VehicleEventRegistry.AddObserver(&character);
	m_CharacterEventRegistry.AddObserver(this);
	m_CharacterEventRegistry.AddObserver(&camera);
	

}

World::~World(void)
{
}

void World::ResetCameras()
{
	camera.Reset();
	character.Reset();
}

VisualMode World::GetVisualMode()
{
	return m_visualmode;
}

void World::SetVisualMode(VisualMode selected)
{
	m_visualmode = selected;
}

void World::CycleVisualMode()
{
	m_visualmode = (VisualMode) ((m_visualmode + 1) % 2);
}

RenderMode World::GetRenderMode()
{
	return m_rendermode;
}

void World::SetRenderMode(RenderMode selected)
{
	m_rendermode = selected;
}

void World::CreateView()
{
	switch(m_visualmode)
	{
	case VISUAL_TOPDOWN:
		camera.CreateCamera();
		break;
	case VISUAL_CHARACTER:
		character.CreateCamera();
		break;
	}

	switch(m_rendermode)
	{
	case RENDER_SOLID:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case RENDER_WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	}
}

void World::GetGridContainingPoint(const Vector3d &point, float &x, float &y)
{
	GetGridContainingPoint(point.x, point.y, x, y);
}

void World::GetGridContainingPoint(float xpos, float ypos, float &x, float &y)
{
	x = GRID_SIZE * floor((xpos + GRID_SIZE/2) / GRID_SIZE);
	y = GRID_SIZE * floor((ypos + GRID_SIZE/2) / GRID_SIZE);
}

void World::GetCenterPosition(float &x, float &y)
{
	switch(m_visualmode)
	{
	case VISUAL_TOPDOWN:
		{
			x = camera.Lookat.x;
			y = camera.Lookat.y;
			return;
		}
		break;
	case VISUAL_CHARACTER:
	default:
		{
			x = character.Position.x;
			y = character.Position.y;
			return;
		}
		break;
	}
}

void World::GetWindowCenter(float &x, float &y)
{
	float xpos, ypos;
	GetCenterPosition(xpos, ypos);

	GetGridContainingPoint(xpos, ypos, x, y);
}

void World::PanMouse(float dx, float dy)
{
	switch(m_visualmode)
	{
	case VISUAL_TOPDOWN:
		{
			dx *= 10;
			dy *= 10;
			Vector3d thevector = ( camera.Lookat - camera.Position).Normalize();
			if(dx != 0)
			{
				camera.Lookat.x += thevector.y * dx;
				camera.Lookat.y -= thevector.x * dx;

				camera.Position.x += thevector.y * dx;
				camera.Position.y -= thevector.x * dx;
			}
			if(dy != 0)
			{
				camera.Lookat.x -= thevector.x * dy;
				camera.Lookat.y -= thevector.y * dy;

				camera.Position.x -= thevector.x * dy;
				camera.Position.y -= thevector.y * dy;
			}
		}
		break;
	case VISUAL_CHARACTER:
		{
			dx *= 4;
			dy *= 1;

			character.ChangeLook(dx, dy);
		}
		break;
	}

}

void World::ZoomMouse(float dx, float dy)
{
	switch(m_visualmode)
	{
	case VISUAL_TOPDOWN:
		{
			Vector3d outdirection = (camera.Position - camera.Lookat);
			float currentmag = outdirection.Magnitude();
			float newmag = currentmag + dy*10;
			newmag = MAX(newmag, MINIMUM_MAGNITUDE);

			camera.Position = (outdirection.Normalize() * newmag) + camera.Lookat;
		}
		break;
	case VISUAL_CHARACTER:
		{
			//nothing for now, maybe
			break;
		}
	}
}

void World::BuildLighting()
{
	glEnable(GL_LIGHTING);
	float noAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
	float noParam[] = {0,0,0, 1.0f};
	float whiteDiffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
	float bigDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};

	///////////////////////////////////////
	glEnable(GL_LIGHT0);
	//first top-down directional
	GLfloat paramsPosition0[] = {-1.0f, -4.0f, -10.0f, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, paramsPosition0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, noParam);
	
	if(m_visualmode == VISUAL_CHARACTER)
		glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuse);
	else
		glLightfv(GL_LIGHT0, GL_DIFFUSE, bigDiffuse);

	glLightfv(GL_LIGHT0, GL_SPECULAR, noParam);

	///////////////////////////////////////
	glEnable(GL_LIGHT1);
	//alternate top-down directional
	GLfloat paramsPosition1[] = {10.0f, 5.0f, 3.0f, 0};
	glLightfv(GL_LIGHT1, GL_POSITION, paramsPosition1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, noParam);

	if(m_visualmode == VISUAL_CHARACTER)
		glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteDiffuse);
	else
		glLightfv(GL_LIGHT1, GL_DIFFUSE, bigDiffuse);

	glLightfv(GL_LIGHT1, GL_SPECULAR, noParam);

	if(m_visualmode == VISUAL_CHARACTER)
	{
		///////////////////////////////////////
		glEnable(GL_LIGHT2);
		//"flashlight" on LookAt point (positional)
		{
			AutoMatrixStack stack;
			//float lightDis = 0.5f;
			Vector3d lightpos = character.Position; //((camera.Lookat - camera.Position).Normalize() * lightDis) + camera.Position;
			//Vector3d lightpos = ((character.Lookat - character.Position).Normalize() * lightDis) + character.Position;
			//lightpos.z = character.Position.z;
			lightpos.z += CHARACTER_HEIGHT;
			glTranslatef(lightpos.x, lightpos.y, lightpos.z);
			GLfloat paramsPosition2[] = {0,0,0,1.0f};//{camera.Lookat.x, camera.Lookat.y , camera.Lookat.z, 1.0f};
			glLightfv(GL_LIGHT2, GL_POSITION, paramsPosition2);
			glLightfv(GL_LIGHT2, GL_AMBIENT, whiteDiffuse);
			glLightfv(GL_LIGHT2, GL_DIFFUSE, bigDiffuse);
			glLightfv(GL_LIGHT2, GL_SPECULAR, bigDiffuse);

			//glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.5f);
			glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.4f);
		}
	}
	else
	{
		glDisable(GL_LIGHT2);
	}

	//experimental stuff ..?

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, noParam);
	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteDiffuse);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noParam);
}

void World::DoFrame(int timeElapsed, ElapsedTime absolutetime, int *keysDown)
{
	m_absoluteTime = absolutetime;

	m_architecture.UpdateScriptables();

	if(m_visualmode == VISUAL_CHARACTER)
	{
		//check keyboard input

		int forward = 0, right = 0;
		if(keysDown['w'])
		{
			//forward
			forward = 1;
		}
		else if(keysDown['s'])
		{
			//backward
			forward = -1;
		}

		//separately
		if(keysDown['d'])
		{
			//right
			right = 1;
		}
		else if(keysDown['a'])
		{
			//left
			right = -1;
		}

		int up = 0;
		bool snap = false;

		if(keysDown['p'])
		{
			//snap to nearest level
			snap = true;
		}
		else
		{
			if(keysDown[' '])
				up = 1;
			else if(keysDown['c'])
				up = -1;
		}

		character.Go(forward, right, up, snap, timeElapsed);
	}
}

bool World::ConstrainToIndoors(Vector3d &originalPoint, Vector3d &delta)
{
	bool was_already_inside = m_architecture.ConstrainToIndoors(originalPoint, delta);

	return was_already_inside;
}

void World::SetLightingAndCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)m_width / (GLfloat)m_height, 0.01, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	CreateView();
	BuildLighting();
}


void World::SetWindowSize(int newWindowSize)
{
	m_architecture.SetWindowSize(newWindowSize);
}

int World::GetWindowSize()
{
	return m_architecture.m_WindowSizeX;
}

void World::Render()
{
	//3D mode - show everything
	glClearColor (0.35f,0.35f,0.5f,1.0f);
	glClear (GL_COLOR_BUFFER_BIT);

	
	{
		AutoMatrixStack stack;
		m_architecture.Render();
	}

	float x, y;
	GetWindowCenter(x, y);


	if(m_visualmode == VISUAL_TOPDOWN)
	{
		float gridx, gridy;
		GetGridContainingPoint(character.Position.x, character.Position.y, gridx, gridy);
		float h = m_architecture.GetEntityHeight(gridx, gridy);
		character.RenderDirectionIndicator(h);
	}

#ifdef _DEBUG
	RenderDebugObjects();
#endif

}

void World::OnCharacterMove(Vector3d position)
{
	Update();
}

void World::Update(bool force)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ClearDebugObjects();

	{
		AutoMatrixStack stack;
		m_architecture.Update(force);
	}
}

string World::GetLookDirection()
{
	return character.GetLookDirection();
}

float World::GetCurrentHeight()
{
	float fracheight = ((character.Position.z - CHARACTER_HEIGHT) / PHYSICAL_HEIGHT_FACTOR);
	float floorheight = round(fracheight);
	if(ABS(fracheight - floorheight) < EPSILON)
		return floorheight;

	return fracheight;
}

float World::GetLevelPhysicalHeight()
{
	return GetLevelPhysicalHeight(GetCurrentHeight());
}

float World::GetLevelPhysicalHeight(float fractionalHeight)
{
	return fractionalHeight * PHYSICAL_HEIGHT_FACTOR;
}


void World::AddDebugRay(Ray newray)
{
#ifdef _DEBUG
	if(m_rayCountDebug < MAX_DEBUG_OBJECTS)
	{
		m_debugRays[m_rayCountDebug++] = newray;
	}
#endif
}

void World::AddDebugPoint(Vector3d newpoint)
{
#ifdef _DEBUG
	if(m_pointCountDebug < MAX_DEBUG_OBJECTS)
	{
		m_debugPoints[m_pointCountDebug++] = newpoint;
	}
#endif
}

void World::RenderDebugObjects()
{
#ifdef _DEBUG
	for(int i = 0; i < m_pointCountDebug; i++)
	{
		m_debugPoints[i].DebugRender();
	}

	for(int i = 0; i < m_rayCountDebug; i++)
	{
		m_debugRays[i].DebugRender();
	}
#endif
}

void World::ClearDebugObjects()
{
	m_rayCountDebug = m_pointCountDebug = 0;
}

ElapsedTime World::GetAbsoluteTime()
{
	return m_absoluteTime;
}