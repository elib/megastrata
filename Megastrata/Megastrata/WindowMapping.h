#pragma once

#include <utils.h>


class WindowMapping
{
public:

	void SetWorldWindow(float top, float right, float left, float bottom);
	void SetPhysicalWindow(int width, int height);

	//translates from physical window to world coordinates.
	void GetWorldCoordinates(float &x, float &y);
	void GetPhysicalCoordinates(float xpos, float ypos, int &x, int &y);

	WindowMapping(void);
	
	float GetSpaceWidth() {return m_windowWidth;}
	float GetSpaceHeight(){return m_windowHeight;}
	
	int GetPhysicalWidth(){return m_physicalWidth;}
	int GetPhysicalHeight(){return m_physicalHeight;}

	float GetLevelHeight();
	void SetLevelHeight(float height);

	bool operator==(WindowMapping const& other);
	WindowMapping(WindowMapping const& source);

private:
	
	RECTFLOAT spaceWindow;

	float m_levelHeight;

	int m_physicalWidth, m_physicalHeight;
	float m_windowWidth, m_windowHeight;
};