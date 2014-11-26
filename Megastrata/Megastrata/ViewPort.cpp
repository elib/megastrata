#include "ViewPort.h"

ViewPort::ViewPort(void)
{
	m_width = 0;
	m_height = 0;

}

ViewPort::~ViewPort(void)
{
}

void ViewPort::SetViewportSize(int w, int h)
{
	//should notify all layers about new size.
	m_width = w;
	m_height = h;

	m_mapping2d.SetPhysicalWindow(m_width, m_height);
	m_mapping2d.SetWorldWindow(-2.5f, 2.5f, -2.5f, 2.5f);
}

void ViewPort::GetWorldCoordinates(float &x, float &y)
{
	m_mapping2d.GetWorldCoordinates(x, y);
}