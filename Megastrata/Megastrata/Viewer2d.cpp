#include "Viewer2d.h"
#include "GeneratorBase.h"
#include "RenderableFactory.h"
#include "SubGenerator.h"
#include "Generator.h"
#include "WindowMapping.h"


void Viewer2d::SetViewportSize(int w, int h)
{
	ViewPort::SetViewportSize(w, h);
}

Viewer2d::Viewer2d(void)
{
	m_LayerManager = LayerManager::theLayerManager;
}

void Viewer2d::Translate(float delta_x, float delta_y)
{
	if((m_LayerManager->GetCurrentLayer() != NULL) && !m_LayerManager->IsCurrentLayer3d())
	{
		((SubGenerator*)m_LayerManager->GetCurrentLayer())->Translate(delta_x * m_mapping2d.GetSpaceWidth(), delta_y * m_mapping2d.GetSpaceHeight());
	}
}

void Viewer2d::Scale(float scale_x, float scale_y)
{
	if((m_LayerManager->GetCurrentLayer() != NULL) && !m_LayerManager->IsCurrentLayer3d())
	{
		((SubGenerator*)m_LayerManager->GetCurrentLayer())->Scale(scale_x, scale_y);
	}
}



//just for active layer
void Viewer2d::Update(bool force)
{
	if((m_LayerManager->GetCurrentLayer() != NULL) && !m_LayerManager->IsCurrentLayer3d())
		((SubGenerator*)m_LayerManager->GetCurrentLayer())->Update();
}

//just for active layer
void Viewer2d::Render()
{
	//only show the current layer
	if((m_LayerManager->GetCurrentLayer() != NULL) && !m_LayerManager->IsCurrentLayer3d())
		((SubGenerator*)m_LayerManager->GetCurrentLayer())->Render();
}

void Viewer2d::SetLightingAndCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Viewer2d::PanMouse(float dx, float dy)
{
	Translate(dx, dy);
	Update();
}

void Viewer2d::ZoomMouse(float dx, float dy)
{
	Scale(dy, dy);
	Update();
}