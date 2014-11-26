#include "WindowMapping.h"
#include <stdio.h>

void WindowMapping::GetWorldCoordinates(float &x, float &y)
{
	//map from rectangular physical window to world space.
	float fractionX = x / ((float)m_physicalWidth);
	float fractionY = y / ((float)m_physicalHeight);

	x = spaceWindow.left + m_windowWidth*fractionX;
	y = spaceWindow.top + m_windowHeight*fractionY;
}

void WindowMapping::GetPhysicalCoordinates(float xpos, float ypos, int &x, int &y)
{
	x = round((m_physicalWidth / m_windowWidth)   * (xpos - spaceWindow.left));
	y = round((m_physicalHeight / m_windowHeight) * (ypos - spaceWindow.top ));
}


WindowMapping::WindowMapping()
{
	m_levelHeight = 0;
	spaceWindow.bottom = spaceWindow.left = spaceWindow.right = spaceWindow.top = 0.0f;
	m_physicalWidth = m_physicalHeight = 0;
	m_windowWidth = m_windowHeight = 0.0f;
}

void WindowMapping::SetWorldWindow(float top, float right, float left, float bottom)
{
	assert(right > left);
	assert(bottom > top);

	spaceWindow.top = top;
	spaceWindow.bottom = bottom;
	spaceWindow.right = right;
	spaceWindow.left = left;

	m_windowWidth = right - left;
	m_windowHeight = bottom - top;
}

//sets where the physical window is stationed
void WindowMapping::SetPhysicalWindow(int width, int height)
{
	assert(width > 0);
	assert(height > 0);

	m_physicalWidth = width;
	m_physicalHeight = height;
}

bool WindowMapping::operator==(WindowMapping const& other)
{
	//either both are exactly on the same level
	//or they are both BETWEEN levels, and between the same levels.

	bool samelevel = 
		(ABS(m_levelHeight - other.m_levelHeight) < EPSILON)
		|| ((m_levelHeight != floor(m_levelHeight)) && (other.m_levelHeight != floor(other.m_levelHeight))
			&& (floor(m_levelHeight) == floor(other.m_levelHeight)));

	return 
		m_physicalWidth == other.m_physicalWidth
	&&	m_physicalHeight == other.m_physicalHeight
	&&	spaceWindow.top == other.spaceWindow.top
	&&	spaceWindow.bottom == other.spaceWindow.bottom
	&&	spaceWindow.left == other.spaceWindow.left
	&&	spaceWindow.right == other.spaceWindow.right
	&& samelevel;
}

WindowMapping::WindowMapping(WindowMapping const& source)
{
	m_physicalWidth = source.m_physicalWidth;
	m_physicalHeight = source.m_physicalHeight;
	spaceWindow.top = source.spaceWindow.top;
	spaceWindow.bottom = source.spaceWindow.bottom;
	spaceWindow.left = source.spaceWindow.left;
	spaceWindow.right = source.spaceWindow.right;
	
	m_windowWidth = source.m_windowWidth;
	m_windowHeight = source.m_windowHeight;

	m_levelHeight = source.m_levelHeight;
}

float WindowMapping::GetLevelHeight()
{
	return m_levelHeight;
}

void WindowMapping::SetLevelHeight(float height)
{
	m_levelHeight = height;
}