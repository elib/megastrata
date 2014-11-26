#pragma once
#include <utils.h>
#include "Entity3d.h"
#include "../../disjoint/DisjointSets.h"
#include "Vector3d.h"
#include "WindowMapping.h"

#include "ScriptableEntity.h"

#define TEXTURE_COUNT 10

class Entity3dCollection
{
public:

	~Entity3dCollection();

	bool AddEntity(int generator_id, WindowMapping &mapping, EntityRenderedType m_targetType, int center_x, int center_y,
		ConnectionStruct &connectivity,	int spanX = 1, int spanY = 1, int spanZ = 1);

	//shortcut
	bool AddEntity(int generator_id, WindowMapping &mapping, EntityRenderedType m_targetType, int center_x, int center_y,
		unsigned char connectivityValue = 0, int spanX = 1, int spanY = 1, int spanZ = 1);

	void SetEntityVariable(int x, int y, int varindex, float var);

	void GetAdjacentHeights(int x, int y, float *heights);
	void GetAdjacentTypes(int x, int y, EntityRenderedType *types);

	float GetEntityVariable(int x, int y, int index);

	void Clear();
	void Render(GLuint *textures, GLuint firstEntityList, GLuint scripttex);
	void CompileDisplayLists(GLuint firstEntityList, float height);

	void GetNodeExits(int x, int y, ConnectionStruct &exits);

	void EnsureAdjacentConnectivity();

	DisjointSets m_disjointSets;

	Entity3dCollection(void);

	
	void AddScriptable(ScriptableEntity *entity);
	void ClearScriptables();
	void GetExtraCollidables(CollidableBatch *batch);

	void UpdateScriptables(ElapsedTime absolutetime);

	void GetCollidables(CollidableBatch *batch, int x, int y, WindowMapping &mapping, bool onlyScriptables = false);

	bool active;

private:
	Entity3d m_entities[MAX_ENTITIES][MAX_ENTITIES];
	void DiscoverConnectedNodes(int x, int y, ConnectionStruct &connections, bool excludeBoxes = false);
	void UnionTwoNodes(int x, int y, int deltax, int deltay);
	bool AreDisjoint(int x, int y, int deltax, int deltay);
	void RenderSingleNode(int x, int y, int texture, bool shouldBakeCollisions);

	ScriptableEntity *m_scriptables[MAX_SCRIPTABLE];
	int m_scriptableCount;

	void RenderScriptables();
};