#pragma once

#include <utils.h>
#include "Vector3d.h"
#include "World.h"
#include "Timer.h"

#include "GuiManager.h"

enum mouse_action_state
{
	MOUSE_INACTIVE,
	MOUSE_PANNING,
	MOUSE_ZOOMING,
	MOUSE_CUTOFF
};

class ProgramManager
{
public:
	ProgramManager(void);

	int Run(int argc, char **argv);
	~ProgramManager(void);
private:
	static ProgramManager* m_Manager;
	void DoSomeTests();

	void static mousebutton(int button, int state, int x, int y);
	void static mousemove(int x, int y);
	void static KeyboardDown(unsigned char key, int x, int y);
	void static KeyboardUp(unsigned char key, int x, int y);
	void static SpecialFunc(int key, int x, int y);

	void static tick();
	void static display(void);
	void static reshape(int w, int h);

	void doMouseMove(int x, int y);
	mouse_action_state m_MouseState;
	int mousedown_x, mousedown_y;

	//viewport dimensions
	int width, height;

	GuiManager m_gui;
};