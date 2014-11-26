#pragma once
#include <utils.h>
#include "Timer.h"
#include "World.h"
#include "Viewer2d.h"
#include "ViewPort.h"
#include "Serializable.h"
#include "RenderableFactory.h"
#include "UserPromptUtil.h"
#include "GeneratorBase.h"
#include "SubGenerator.h"
#include "Generator.h"

#include "LayerManager.h"


#include "VideoOutput.h"

// [WODE]
#ifdef __APPLE__
	#include <Glui/glui.h>
#else
	#include <glui.h>
#endif

#define DEFAULT_FILENAME	"default.xml"

#define FPS 60
#define MS_PER_FRAME 1000 / FPS

#define VIEWPORT_COUNT 2


class GuiManager
{
public:
	GuiManager(void);
	~GuiManager(void);

	void SetDefaults();

	void DoScreenShot();

	//respond to mouse events
	void PanMouse(int delta_x, int delta_y);
	void ZoomMouse(int delta_x, int delta_y);

	void Update(bool force = false);

	void DoTick();
	void IncrementParameter(float delta);

	void Initialize(int window_id, GLUI *parentGlui);
	void SetViewportSize(int w, int h);
	void SetGuiInformation();
	void SetSelectedVariable();

	void DoKeyDown(int key);
	void DoKeyUp(int key);

	void PerformDisplay();

	void ToggleVideo();
	void DoArrowKey(int key);

private:

	//keys that are down.
	//methodology borrowed from http://www.nullterminator.net/glut.html
	int KeyDown[256];

	static GuiManager* m_Manager;

	void RemoveAllListboxItems(GLUI_Listbox *lb);
	void SyncronizeLayerList();

	//signals the main loop to redraw the viewport.
	int m_renderableCount;

	void SetSelectedLayer();

	//get this out of my face:
	void SetCamera();

	Timer m_timer;
	int m_last_frame_time;
	int m_window_id;

	int m_width, m_height;

	//"strategy" implementation
	ViewPort *m_availableViewports[VIEWPORT_COUNT];
	int m_activeViewport;
	ViewPort* GetCurrentViewPort();

	//The Nitty Gritty
	static void event_InverseCheck(int value);
	static void event_WindowsizeSpinner(int value);

	static void event_RemoveLayer(int value);
	static void event_AddLayer(int value);
	static void event_ChangeSelection(int value);
	static void event_SetInputLayer(int value);
	static void event_loadXML(int value);
	static void event_saveXML(int value);
	static void event_defaults(int value);
	static void event_visualmode(int value);
	static void event_rendermode(int value);
	

	static void event_VariableChangeSelection(int value);
	static void event_RemoveInputLayer(int value);
	static void event_NavigateInto(int value);
	static void event_NavigateUp(int value);

	void UpdateDisplayedPosition();

	int GetIdIndex(int id);

	void SetVariableOverrideValue();

	void AddLayer(Generator *newlayer);
	void RemoveLayer();

	void InverseChecked();
	void SetWindowSize();

	void SelectVisualMode();
	void SelectRenderMode();

	void LoadFromXml();
	XmlError LoadFromXmlWithErrors(string path = "");
	void SaveToXml();

	GLUI *m_parentGlui;
	
	GLUI_Panel *m_panelLayers, *m_layerVariables, *m_panelRendermode, *m_panelTools, *m_panelInfo;
	
	GLUI_Listbox 
		*m_lbLayers,
		*m_lbVariables, *m_lbLayerTypes, *m_lbEntityTypes;

	GLUI_EditText *m_txtVariableValue, *m_txtPos, *m_currCell, *m_txtBearing;

	GLUI_Checkbox *m_cbInverse;

	GLUI_Button
		*m_btnRemoveLayer, 
		*m_btnAddLayer,
		*m_btnSetInputLayer, *m_btnRemoveInputLayer,
		*m_btnNavigateInto, *m_btnNavigateUp,
		*m_btnLoadXML, *m_btnSaveXML,
		*m_btnLoadDefaults,
		*m_btnVisualModeCycle;

	GLUI_Spinner *m_3dWindow;

	GLUI_RadioGroup *m_grpRenderMode;
	GLUI_RadioButton *m_radSolid, *m_radWireFrame;


	//video
	bool m_doingVid;
	VideoOutput m_video;

	//ONLY static declarations of these!
	World m_World;
	LayerManager m_LayerManager;
};