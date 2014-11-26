#pragma once
#include <utils.h>
#include "WindowMapping.h"
#include "SubGenerator.h"
#include "RenderableFactory.h"
#include "Generator.h"
#include "Serializable.h"

#define MAX_RENDERABLE 5

typedef struct _LAYERINFO {
	char name[100];
	int id;
} LayerInfo;

typedef struct _GENERATORTEXTURE {
	int generator_id;
	int texture_group;
} GeneratorTexture;

class LayerManager: public Serializable
{
public:
	static LayerManager* theLayerManager;

	void GenerateEntities(Entity3dCollection *targetCollection, WindowMapping &mapping, float height);
	
	LayerManager(void);
	~LayerManager(void);

	virtual void SaveToXml(TiXmlElement *currentElement);
	virtual void LoadFromXml(TiXmlElement *currentElement);

	void AddLayer(Generator* newLayer);
	void RemoveLayer(int id);
	void SetSelectedLayer(int selectedID);
	void RemoveInputLayer(int selectedIndex);
	void RemoveAllLayers();
	GeneratorBase* GetCurrentLayer();
	int GetLayers(LayerInfo *infos);
	void InverseChecked(bool checked);
	bool IsCurrentLayer3d();
	void SetInputLayer(int selectedVariable, int typeIndex);

	void NavigateInto(int selectedVariable);
	void NavigateUp();
	int GetArchitecturalTextureCount();
	int GetTextureGroup(int GeneratedByID, int textureIndex);

private:

	int GetIdIndex(int id);

	GeneratorBase	*m_currentLayer;
	Generator		*m_architecture[MAX_RENDERABLE];
	int m_architectureCount;

	void BuildTextureList();

	GeneratorTexture m_generatorTextures[5 * MAX_RENDERABLE];
	int m_textureCount;
};