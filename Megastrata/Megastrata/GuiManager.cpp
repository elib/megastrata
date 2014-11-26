#include "GuiManager.h"

GuiManager* GuiManager::m_Manager = 0;

GuiManager::GuiManager(void)
{
	m_Manager = this;
	m_renderableCount = 0;
	m_width = m_height = 0;
	m_last_frame_time = 0;

	m_window_id = 0;
	memset(KeyDown, 0, sizeof(KeyDown));

	//"strategy"!
	m_activeViewport = DISPLAY_3D;
	m_availableViewports[DISPLAY_3D] = &m_World;
	m_availableViewports[DISPLAY_2D] = new Viewer2d();

	m_doingVid = false;
}

GuiManager::~GuiManager(void)
{
	if(m_doingVid)
		m_video.StopVideo();

	for(int i = 0; i < VIEWPORT_COUNT; i++)
		delete m_availableViewports[i];
}

void GuiManager::DoTick()
{
	//start timer
	int ms = (int)m_timer.TimeElapsed();

	//perform changes
	GetCurrentViewPort()->DoFrame(ms + m_last_frame_time, m_timer.GetAbsoluteTime(), KeyDown);

	//must set main GLUT window
	glutSetWindow(m_window_id);
	glutPostRedisplay();

	UpdateDisplayedPosition();

	//after all this
	if(m_doingVid)
		m_video.DoFrame();

	int thisframe = (int)m_timer.TimeElapsed();
	m_last_frame_time = thisframe;
	if(thisframe < MS_PER_FRAME)
	{
		int sleeptime = MS_PER_FRAME - m_last_frame_time;
		m_last_frame_time += sleeptime;
		Sleep(sleeptime);
	}
}

void GuiManager::UpdateDisplayedPosition()
{
#ifdef _DEBUG
	float x, y;
	m_World.GetCenterPosition(x, y);
	char pos[100];
	sprintf_s(pos, "%.2f, %.2f", x, y);
	m_txtPos->set_text(pos);

	float ix, iy;
	m_World.GetGridContainingPoint(x, y, ix, iy);
	char pos2[100];
	sprintf_s(pos2, "%.2f, %.2f", ix, iy);
	m_currCell->set_text(pos2);

	string bearing = m_World.GetLookDirection();
	m_txtBearing->set_text(bearing.c_str());
#endif
}

void GuiManager::PanMouse(int delta_x, int delta_y)
{
	//get relative to size of window
	float dx, dy;
	dx = (float)delta_x / m_width;
	dy = (float)delta_y / m_height;

	GetCurrentViewPort()->PanMouse(dx, dy);
	//GetCurrentViewPort()->Update();

	glutSetWindow(m_window_id);
	glutPostRedisplay();
}

void GuiManager::ZoomMouse(int delta_x, int delta_y)
{

	//get relative to size of window
	float dx, dy;
	dx = (float)delta_x / m_width;
	dy = (float)delta_y / m_height;

	GetCurrentViewPort()->ZoomMouse(dx, dy);
	//GetCurrentViewPort()->Update();

	glutSetWindow(m_window_id);
	glutPostRedisplay();
}

void GuiManager::PerformDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// Clear The Screen And The Depth Buffer

	m_Manager->SetCamera();

	m_Manager->GetCurrentViewPort()->Render();

	glFlush();
	glutSwapBuffers();
}

void GuiManager::SetViewportSize(int w, int h)
{
	for(int i = 0; i < VIEWPORT_COUNT; i++)
	{
		m_availableViewports[i]->SetViewportSize(w, h);
	}
	m_width = w;
	m_height = h;
}

void GuiManager::SetCamera()
{
	GetCurrentViewPort()->SetLightingAndCamera();
}

ViewPort* GuiManager::GetCurrentViewPort()
{
	if(m_LayerManager.IsCurrentLayer3d())
		return m_availableViewports[DISPLAY_3D];
	else
		return m_availableViewports[DISPLAY_2D];
}

void GuiManager::SetDefaults()
{
	m_LayerManager.RemoveAllLayers();
	std::string path = UserPromptUtil::GetExecutablePath();
	path.append(DEFAULT_FILENAME);
	if(FileExists(path.c_str()))
	{
		//try to get XML - if success, get outa here
		if(LoadFromXmlWithErrors(path) == XML_SUCCESS)
			return;
	}

	//we failed.
	m_LayerManager.RemoveAllLayers();
	Generator* newlayer = RenderableFactory::CreateGenerator(4);
	SubGenerator* noise1 = RenderableFactory::CreateLayer2d(0);
	SubGenerator* noise2 = RenderableFactory::CreateLayer2d(0);

	noise1->Scale(0.01f, 0.01f);
	noise2->Scale(-5, -5);

	newlayer->AssignLayerToVariable(0, noise1);
	newlayer->AssignLayerToVariable(1, noise2);

	m_World.SetWindowSize(14);

	m_World.ResetCameras();
	m_World.SetVisualMode(VISUAL_TOPDOWN);

	AddLayer(newlayer);
}

void GuiManager::Initialize(int window_id, GLUI* parentGlui)
{
	m_window_id = window_id;

	m_parentGlui = parentGlui;

	m_panelLayers = parentGlui->add_panel("Layer controls");

	m_lbEntityTypes = parentGlui->add_listbox_to_panel(m_panelLayers, "Base layer types: ");
	m_lbLayers = parentGlui->add_listbox_to_panel(m_panelLayers, "Base Layers: ", 0, -1, GuiManager::event_ChangeSelection);

	//add/remove buttons
	m_btnAddLayer = parentGlui->add_button_to_panel(m_panelLayers, "Add new layer", -1, GuiManager::event_AddLayer);
	m_btnRemoveLayer = parentGlui->add_button_to_panel(m_panelLayers, "Remove layer", -1, GuiManager::event_RemoveLayer);

	//m_btnAddLayer = parentGlui->add_button_to_panel(m_panelLayers, "Change base", -1, GuiManager::event_AddLayer);

	m_cbInverse = parentGlui->add_checkbox_to_panel(m_panelLayers, "Inverse", 0, -1, GuiManager::event_InverseCheck);

	m_layerVariables = parentGlui->add_panel_to_panel(m_panelLayers, "Layer Variables");
	m_lbVariables = parentGlui->add_listbox_to_panel(m_layerVariables, "Select variable: ", 0, -1, GuiManager::event_VariableChangeSelection);
	
	m_txtVariableValue = parentGlui->add_edittext_to_panel(m_layerVariables, "Value: ");

	m_lbLayerTypes = parentGlui->add_listbox_to_panel(m_layerVariables, "Layer Types: ");
	for(int i = 0; i < RenderableFactory::LayerTypeCount; i++)
	{
		if(RenderableFactory::TypeInformation[i].type_display_mode == DISPLAY_3D)
			m_lbEntityTypes->add_item(i, RenderableFactory::TypeInformation[i].type_name);
		else
			m_lbLayerTypes->add_item(i, RenderableFactory::TypeInformation[i].type_name);
	}

	m_btnSetInputLayer = parentGlui->add_button_to_panel(m_layerVariables, "Assign input layer", -1, GuiManager::event_SetInputLayer);
	m_btnRemoveInputLayer = parentGlui->add_button_to_panel(m_layerVariables, "Remove input layer", -1, GuiManager::event_RemoveInputLayer);
	m_btnNavigateInto = parentGlui->add_button_to_panel(m_layerVariables, "Edit input layer", -1, GuiManager::event_NavigateInto);

	m_btnNavigateUp = parentGlui->add_button_to_panel(m_panelLayers, "Go to parent", -1, GuiManager::event_NavigateUp);
	m_3dWindow = parentGlui->add_spinner_to_panel(m_panelLayers, "3D Window Size", GLUI_SPINNER_INT, 0, -1, GuiManager::event_WindowsizeSpinner);
	m_3dWindow->set_speed(0.1f);
	m_3dWindow->set_int_limits(2, 250);

	
#ifdef _DEBUG
	parentGlui->add_column();
#endif

#ifdef _DEBUG
	//info panel
	m_panelInfo = parentGlui->add_panel("Info");
	m_txtPos = parentGlui->add_edittext_to_panel(m_panelInfo, "(x, y): ");
	m_currCell = parentGlui->add_edittext_to_panel(m_panelInfo, "cell: ");
	m_txtBearing = parentGlui->add_edittext_to_panel(m_panelInfo, "Bearing: ");
	m_txtPos->set_w(180);
	m_currCell->set_w(180);
	m_txtBearing->set_w(180);
	
#endif

	m_panelRendermode = parentGlui->add_panel("Visual");
	m_btnVisualModeCycle = parentGlui->add_button_to_panel(m_panelRendermode, "Top-down/character view", -1, GuiManager::event_visualmode);

	m_grpRenderMode = parentGlui->add_radiogroup_to_panel(m_panelRendermode, 0, -1, GuiManager::event_rendermode);
	m_radSolid = parentGlui->add_radiobutton_to_group(m_grpRenderMode, "Solid Rendering");
	m_radWireFrame = parentGlui->add_radiobutton_to_group(m_grpRenderMode, "Wireframe");

		//save, load
	m_panelTools = parentGlui->add_panel("Tools");
	m_btnLoadXML = parentGlui->add_button_to_panel(m_panelTools, "Load from XML", -1, GuiManager::event_loadXML);
	m_btnSaveXML = parentGlui->add_button_to_panel(m_panelTools, "Save to XML", -1, GuiManager::event_saveXML);
	m_btnLoadDefaults = parentGlui->add_button_to_panel(m_panelTools, "Reload defaults", -1, GuiManager::event_defaults);



	SetSelectedLayer();
}

void GuiManager::SetGuiInformation()
{
	m_btnAddLayer->enable();
	m_btnRemoveLayer->disable();
	m_btnNavigateUp->disable();

	if(m_LayerManager.GetCurrentLayer() != NULL)
	{
		//a selected layer
		m_btnRemoveLayer->enable();

		RemoveAllListboxItems(m_lbVariables);

		for(int i = 0; i < m_LayerManager.GetCurrentLayer()->m_variableCount; i++)
			m_lbVariables->add_item(i, m_LayerManager.GetCurrentLayer()->m_layerVariables[i].GetName());

		SetSelectedVariable();
	}

	Update(true);
}

void GuiManager::RemoveAllListboxItems(GLUI_Listbox *lb)
{
	GLUI_Node *node = lb->items_list.first_child();
	while(node != NULL)
	{
		int id = ((GLUI_Listbox_Item*)node)->id;
		node = node->next();
		lb->delete_item(id);
	}
}

void GuiManager::SyncronizeLayerList()
{
	RemoveAllListboxItems(m_lbLayers);

	LayerInfo infos[MAX_RENDERABLE];
	int count;
	count = m_LayerManager.GetLayers(infos);

	for(int i = 0; i < count; i++)
	{
		m_lbLayers->add_item(infos[i].id, infos[i].name);
	}

	m_lbLayers->set_int_val(count - 1);
	SetSelectedLayer();
}

void GuiManager::SetVariableOverrideValue()
{
	char txt[100];
	int selectedVariable = m_lbVariables->get_int_val();
	sprintf_s(txt, "%g", m_LayerManager.GetCurrentLayer()->m_layerVariables[selectedVariable].GetOverrideValue());
	m_txtVariableValue->set_text(txt);
}

void GuiManager::SetSelectedVariable()
{
	m_btnNavigateInto->disable();
	m_btnRemoveInputLayer->disable();
	m_btnSetInputLayer->disable();
	m_btnNavigateUp->disable();

	if(m_LayerManager.GetCurrentLayer() != NULL)
	{
		SetVariableOverrideValue();

		int selectedVariable = m_lbVariables->get_int_val();

		if(m_LayerManager.GetCurrentLayer()->m_layerVariables[selectedVariable].GetRenderable() != NULL)
		{
			m_btnNavigateInto->enable();
			m_btnRemoveInputLayer->enable();
		}
		else
		{
			m_btnSetInputLayer->enable();
		}

		if(m_LayerManager.GetCurrentLayer()->ParentLayer != NULL)
        {
			m_btnNavigateUp->enable();
        }

		if(m_LayerManager.IsCurrentLayer3d())
		{
			m_cbInverse->disable();

			m_3dWindow->enable();

			int selectedlayer = m_lbLayers->get_int_val();
			int windowsize = m_World.GetWindowSize();
			m_3dWindow->set_int_val(windowsize);

			int rendermode = (int)World::theWorld->GetRenderMode();
			m_grpRenderMode->set_int_val(rendermode);
		}
		else
		{
			m_cbInverse->enable();
			int inversechecked = ((SubGenerator*)m_LayerManager.GetCurrentLayer())->m_inverse ? 1 : 0;
			m_cbInverse->set_int_val(inversechecked);

			m_3dWindow->disable();
		}
	}

    // Quick fix for rendering problem on OSX while shift-dragging.
	// GLUI seems to be difficult in this area..
	// THANKS, [WODE]!!
    Update(true);
}

void GuiManager::event_InverseCheck(int value)
{
	int checked = m_Manager->m_cbInverse->get_int_val();
	m_Manager->m_LayerManager.InverseChecked((bool)checked);
	m_Manager->SetGuiInformation();
}

void GuiManager::event_WindowsizeSpinner(int value)
{
	int newWindowSize = m_Manager->m_3dWindow->get_int_val();
	int selectedLayer = m_Manager->m_lbLayers->get_int_val();
	m_Manager->m_World.SetWindowSize(newWindowSize);
	m_Manager->SetGuiInformation();
}

void GuiManager::event_RemoveLayer(int value)
{
	m_Manager->RemoveLayer();
}

void GuiManager::RemoveLayer()
{
	int id = m_lbLayers->get_int_val();
	m_LayerManager.RemoveLayer(id);
	SyncronizeLayerList();
	Update(true);
}

void GuiManager::event_ChangeSelection(int value)
{
	m_Manager->SetSelectedLayer();
}

void GuiManager::event_VariableChangeSelection(int value)
{
	m_Manager->SetSelectedVariable();
}

void GuiManager::event_RemoveInputLayer(int value)
{
	int selectedIndex = m_Manager->m_lbVariables->get_int_val();
	m_Manager->m_LayerManager.RemoveInputLayer(selectedIndex);
	m_Manager->SetGuiInformation();
}

void GuiManager::event_SetInputLayer(int value)
{
	int selectedVariable = m_Manager->m_lbVariables->get_int_val();
	int typeIndex = m_Manager->m_lbLayerTypes->get_int_val();
	m_Manager->m_LayerManager.SetInputLayer(selectedVariable, typeIndex);
	m_Manager->SetGuiInformation();
}

void GuiManager::event_NavigateInto(int value)
{
	int selectedVariable = m_Manager->m_lbVariables->get_int_val();
	m_Manager->m_LayerManager.NavigateInto(selectedVariable);
	m_Manager->SetGuiInformation();
}

void GuiManager::event_NavigateUp(int value)
{
	m_Manager->m_LayerManager.NavigateUp();
	m_Manager->SetGuiInformation();
}

void GuiManager::event_AddLayer(int value)
{
	int typeIndex = m_Manager->m_lbEntityTypes->get_int_val();
	m_Manager->AddLayer(RenderableFactory::CreateGenerator(typeIndex));
}

void GuiManager::AddLayer(Generator *newlayer)
{
	m_LayerManager.AddLayer(newlayer);
	SyncronizeLayerList();
}

void GuiManager::event_loadXML(int value)
{
	m_Manager->m_LayerManager.RemoveAllLayers();
	m_Manager->LoadFromXml();
}

void GuiManager::event_saveXML(int value)
{
	m_Manager->SaveToXml();
}

void GuiManager::event_defaults(int value)
{
	m_Manager->SetDefaults();
}


void GuiManager::event_visualmode(int value)
{
	m_Manager->SelectVisualMode();
}

void GuiManager::event_rendermode(int value)
{
	m_Manager->SelectRenderMode();
}

void GuiManager::SelectVisualMode()
{
	World::theWorld->CycleVisualMode();
	Update(true);
}

void GuiManager::SelectRenderMode()
{
	int selected = m_grpRenderMode->get_int_val();
	World::theWorld->SetRenderMode((RenderMode)selected);
	Update(true);
}

void GuiManager::Update(bool force)
{
	GetCurrentViewPort()->Update(force);
}

void GuiManager::SetSelectedLayer()
{
	m_LayerManager.SetSelectedLayer(m_lbLayers->get_int_val());
	SetGuiInformation();
}

void GuiManager::IncrementParameter(float delta)
{
	if(m_LayerManager.GetCurrentLayer() != NULL)
	{
		int variable = m_lbVariables->get_int_val();
		m_LayerManager.GetCurrentLayer()->IncrementParameter(variable, delta);
		m_Manager->SetVariableOverrideValue();
		Update(true);
	}
}

#include <stdio.h>
#include <time.h>

void GuiManager::DoScreenShot()
{
	UserPromptUtil::SaveScreenShot(m_width, m_height);
}

void GuiManager::ToggleVideo()
{
	if(!m_doingVid)
	{
		LOG("Starting video.");
		m_doingVid = m_video.StartVideo("D:\\megastrata.mpg", m_width, m_height);
	}
	else
	{
		LOG("Stopping video.");
		m_video.StopVideo();
		m_doingVid = false;
	}
}

void GuiManager::DoKeyDown(int key)
{
	KeyDown[key] = 1;

	switch(key)
	{
	case 'g':
		SelectVisualMode();
		break;
	case 'l':
		{
			m_LayerManager.RemoveAllLayers();
			LoadFromXml();
		}
		break;
	case '-':
		m_timer.TogglePause();
		break;
	}
}

void GuiManager::DoKeyUp(int key)
{
	KeyDown[key] = 0;
}

void GuiManager::DoArrowKey(int key)
{
	//float amount = 0.05f;

	//switch(key)
	//{
	//case GLUT_KEY_UP:
	//	m_World.PanMouse(0, -amount);
	//	Update();
	//	break;
	//case GLUT_KEY_DOWN:
	//	m_World.PanMouse(0, amount);
	//	Update();
	//	break;
	//case GLUT_KEY_LEFT:
	//	m_World.PanMouse(-amount, 0);
	//	Update();
	//	break;
	//case GLUT_KEY_RIGHT:
	//	m_World.PanMouse(amount, 0);
	//	Update();
	//	break;
	//}
}