#pragma once

#include <utils.h>
#include "TopDownCamera.h"
#include "Serializable.h"
#include "Character.h"
#include "CharacterObserver.h"
#include "ViewPort.h"
#include "GeneratedEntities.h"
#include "VehicleEventRegistry.h"
#include "CharacterEventRegistry.h"

#define MINIMUM_MAGNITUDE (1.0f)
#define PHYSICAL_HEIGHT_FACTOR	(2.0f)

#define CHARACTER_HEIGHT (0.1f)

enum RenderMode
{
	RENDER_SOLID,
	RENDER_WIREFRAME
};

enum VisualMode
{
	VISUAL_TOPDOWN,
	VISUAL_CHARACTER
};

#define MAX_DEBUG_OBJECTS	500

class World: public ViewPort, public Serializable, public CharacterObserver
{
public:
	//singleton object
	static World* theWorld;

	VisualMode GetVisualMode();
	void SetVisualMode(VisualMode selected);
	void CycleVisualMode();

	RenderMode GetRenderMode();
	void SetRenderMode(RenderMode selected);

	virtual void SetLightingAndCamera();

	virtual void OnCharacterMove(Vector3d position);

	void ResetCameras();

	virtual void Render();
	virtual void Update(bool force = false);

	virtual void PanMouse(float dx, float dy);
	virtual void ZoomMouse(float dx, float dy);

	void SetWindowSize(int newWindowSize);
	int GetWindowSize();

	virtual void DoFrame(int timeElapsed, ElapsedTime absolutetime, int *keysDown);

	void GetWindowCenter(float &x, float &y);
	void GetCenterPosition(float &x, float &y);

	string GetLookDirection();

	void GetGridContainingPoint(float xpos, float ypos, float &x, float &y);
	void GetGridContainingPoint(const Vector3d &point, float &x, float &y);

	void BuildLighting();
	void CreateView();

	void SaveToXml(TiXmlElement *currentElement);
	void LoadFromXml(TiXmlElement *currentElement);

	bool ConstrainToIndoors(Vector3d &originalPoint, Vector3d &delta);

	World(void);
	~World(void);

	float GetCurrentHeight();
	float GetLevelPhysicalHeight(float fractionalHeight);
	float GetLevelPhysicalHeight();

	void ClearDebugObjects();
	void AddDebugRay(Ray newray);
	void AddDebugPoint(Vector3d newpoint);
	void RenderDebugObjects();

	VehicleEventRegistry m_VehicleEventRegistry;
	CharacterEventRegistry m_CharacterEventRegistry;

	ElapsedTime GetAbsoluteTime();

private:
	VisualMode m_visualmode;
	RenderMode m_rendermode;

	//float m_currentHeight;

	TopDownCamera camera;
	Character character;
	GeneratedEntities m_architecture;

	Ray m_debugRays[MAX_DEBUG_OBJECTS];
	Vector3d m_debugPoints[MAX_DEBUG_OBJECTS];
	int m_rayCountDebug, m_pointCountDebug;

	ElapsedTime m_absoluteTime;
};