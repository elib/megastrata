#pragma once
#include "Entity3dCollection.h"
#include "Serializable.h"
#include "WindowMapping.h"

enum RefreshEntitiesCommand
{
	NO_REFRESH,
	REFRESH_ALL,
	REFRESH_ABOVE,
	REFRESH_BELOW
};

class GeneratedEntities
{
public:
	GeneratedEntities();
	~GeneratedEntities(void);

	void Render();
	void Update(bool force = false);

	bool ConstrainToIndoors(Vector3d &originalPoint, Vector3d &delta);

	bool FillCollection(bool force);

	float GetEntityHeight(float gridx, float gridy);

	//measured in cells
	int m_WindowSizeX, m_WindowSizeY;

	bool SetWindowSize(int newWindowSize);

	void UpdateScriptables();

private:

	RefreshEntitiesCommand SetSpaceWindow();
	void PerformTests(WindowMapping &mapping);
	void InitializeTextures();

	Entity3dCollection *m_EntityCollections[2];
	GLuint m_firstEntityList[2];

	bool BetweenLevels();

	GLuint m_entityTextures	[TEXTURE_COUNT];
	GLuint m_scriptableTextures [2];

	WindowMapping m_spaceWindow;
};