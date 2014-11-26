#include "ProgramManager.h"
#include <utils.h>

#include <VersionInfo.h>

#ifdef _DEBUG
#define WINDOW_WIDTH	1000
#define WINDOW_HEIGHT	400
#else
//GLUI: 239px
#ifdef INCLUDE_VIDEO
#define WINDOW_WIDTH	(480*2 + 239)
#define WINDOW_HEIGHT	(360*2)
#else
#define WINDOW_WIDTH	700
#define WINDOW_HEIGHT	600
#endif
#endif

#include "NoiseSlice.h"

#define PROGRAM_NAME "Megastrata v" VERSION_STRING

ProgramManager* ProgramManager::m_Manager = NULL;

ProgramManager::ProgramManager(void)
{
	m_Manager = this;
	m_MouseState = MOUSE_INACTIVE;
	mousedown_x = 0;
	mousedown_y = 0;
	width = height = 0;
}

ProgramManager::~ProgramManager(void)
{
	m_Manager = NULL;
}


void ProgramManager::mousebutton(int button, int state, int x, int y)
{
	if(state != GLUT_DOWN)
		m_Manager->m_MouseState = MOUSE_INACTIVE;
	else
	{
		m_Manager->mousedown_x = x;
		m_Manager->mousedown_y = y;

		if(button == GLUT_RIGHT_BUTTON)
		{
			m_Manager->m_MouseState = MOUSE_ZOOMING;
		}
		else if(button == GLUT_LEFT_BUTTON)
		{

			if(glutGetModifiers() & GLUT_ACTIVE_SHIFT)
			{
				m_Manager->m_MouseState = MOUSE_CUTOFF;
			}
			else if(glutGetModifiers() & GLUT_ACTIVE_CTRL)
			{
				m_Manager->m_MouseState = MOUSE_ZOOMING;
			}
			else
			{
				m_Manager->m_MouseState = MOUSE_PANNING;
			}
		}
	}
}

void ProgramManager::KeyboardDown(unsigned char key, int x, int y)
{
	m_Manager->m_gui.DoKeyDown(key);
}

void ProgramManager::KeyboardUp(unsigned char key, int x, int y)
{
	m_Manager->m_gui.DoKeyUp(key);
}

void ProgramManager::SpecialFunc(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F9:
		//screenshot
		m_Manager->m_gui.DoScreenShot();
		break;
	case GLUT_KEY_F8:
		//toggle video
		m_Manager->m_gui.ToggleVideo();
		break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
		m_Manager->m_gui.DoArrowKey(key);
		break;
	}
		
}

void ProgramManager::doMouseMove(int x, int y)
{
	if(m_MouseState != MOUSE_INACTIVE)
	{
		int delta_x = mousedown_x - x;
		int delta_y = (mousedown_y - y);
		mousedown_x = x;
		mousedown_y = y;

		switch(m_MouseState)
		{
		case MOUSE_PANNING:
			m_gui.PanMouse(delta_x, delta_y);
			break;
		case MOUSE_ZOOMING:
			m_gui.ZoomMouse(delta_x, delta_y);
			break;
		case MOUSE_CUTOFF:
			m_gui.IncrementParameter(((float)delta_y)/100);
			break;
		}
	}
}



void ProgramManager::mousemove(int x, int y)
{
	m_Manager->doMouseMove(x, y);
}

void ProgramManager::tick()
{
	m_Manager->m_gui.DoTick();
}


void ProgramManager::display(void)
{
	m_Manager->m_gui.PerformDisplay();
}

void ProgramManager::reshape(int w, int h)
{
	int x,y;
	GLUI_Master.get_viewport_area(&x, &y, &m_Manager->width, &m_Manager->height);
	GLUI_Master.auto_set_viewport();

	m_Manager->m_gui.SetViewportSize(m_Manager->width, m_Manager->height);
}

void ProgramManager::DoSomeTests()
{

#ifdef _DEBUG
	//just doing some round() tests.
	assert(round(0.3f) == 0);
	assert(round(-0.3f) == 0);
	assert(round(-1.3f) == -1);
	assert(round(1.3f) == 1);

	assert(round(0.99999f) == 1);
	assert(round(-0.99999f) == -1);
	assert(round(-10.99999f) == -11);
	assert(round(10.99999f) == 11);

	assert(round(0.0001f) == 0);
	assert(round(-0.0001f) == 0);
	assert(round(-10.0001f) == -10);
	assert(round(10.0001f) == 10);
#endif

}

int ProgramManager::Run(int argc, char **argv)
{

	DoSomeTests();

	int x, y;
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	int window_id = glutCreateWindow(PROGRAM_NAME);

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glDisable(GL_BLEND); //just killing to be sure

	//glBlendFunc (GL_SRC_ALPHA, GL_DST_ALPHA);
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LESS);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	glEnable(GL_NORMALIZE);


	//set up key state tracking
	glutIgnoreKeyRepeat(true);
	GLUI_Master.set_glutKeyboardFunc(ProgramManager::KeyboardDown);
	glutKeyboardUpFunc(ProgramManager::KeyboardUp);
	GLUI_Master.set_glutSpecialFunc(ProgramManager::SpecialFunc);

	GLUI_Master.set_glutMouseFunc( ProgramManager::mousebutton );
	GLUI_Master.set_glutReshapeFunc( ProgramManager::reshape );

	glutMotionFunc(ProgramManager::mousemove);
	glutDisplayFunc(ProgramManager::display);

	GLUI *glui = GLUI_Master.create_glui_subwindow(window_id, GLUI_SUBWINDOW_RIGHT);
	glui->set_main_gfx_window(window_id);

	GLUI_Master.set_glutIdleFunc(ProgramManager::tick);
	
	m_gui.Initialize(window_id, glui);

	GLUI_Master.get_viewport_area(&x, &y, &width, &height);
	m_gui.SetViewportSize(width, height);
	//char sdf[100];
	//sprintf_s(sdf, "width: %d", WINDOW_WIDTH - width);
	//UserPromptUtil::UserMessageBox(sdf, "ARAR");
	
	m_gui.SetDefaults();

	glutMainLoop();

	return 0;
}