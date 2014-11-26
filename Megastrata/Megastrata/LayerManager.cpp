#include "LayerManager.h"

LayerManager* LayerManager::theLayerManager = 0;

LayerManager::LayerManager(void)
{
	assert(theLayerManager == 0);

	theLayerManager = this;
	m_currentLayer = NULL;

	m_architectureCount = 0;

	for(int i = 0; i < MAX_RENDERABLE; i++)
		m_architecture[i] = NULL;
}

LayerManager::~LayerManager(void)
{
	RemoveAllLayers();
}


void LayerManager::AddLayer(Generator* newLayer)
{
	if(m_architectureCount < MAX_RENDERABLE - 1 && m_architectureCount >= 0)
	{
		m_architecture[m_architectureCount++] = newLayer;
		m_currentLayer = (GeneratorBase*)newLayer;
	}

	BuildTextureList();
}

void LayerManager::RemoveLayer(int id)
{
	int index = GetIdIndex(id);

	//does specified index exist in list
	if(index >= 0)
	{
		delete m_architecture[index];
		m_architecture[index] = NULL;

		//move everything back
		for(int i = index; i < (m_architectureCount-1); i++)
		{
			m_architecture[i] = m_architecture[i+1];
			m_architecture[i+1] = NULL;
		}

		m_architectureCount--;


		if(m_architectureCount > 0)
		{
			m_currentLayer = (GeneratorBase*) m_architecture[m_architectureCount - 1];
		}
		else
		{
			m_currentLayer = NULL;
		}
	}

	BuildTextureList();
}

void LayerManager::SetSelectedLayer(int selectedID)
{
	if(m_architectureCount <= 0)
	{
		m_currentLayer = NULL;
	}
	else
	{
		int selectedIndex = GetIdIndex(selectedID);
		if(selectedIndex > -1)
			m_currentLayer = (GeneratorBase*)m_architecture[selectedIndex];
	}

	BuildTextureList();
}

int LayerManager::GetIdIndex(int id)
{
	for(int i = 0; i < m_architectureCount; i++)
	{
		if(m_architecture[i]->id == id)
			return i;
	}

	return -1;
}

void LayerManager::RemoveInputLayer(int selectedIndex)
{
	if(m_currentLayer != NULL)
	{
		m_currentLayer->m_layerVariables[selectedIndex].RemoveRenderable();
	}
}

void LayerManager::RemoveAllLayers()
{
	for(int i = m_architectureCount - 1; i >= 0; i--)
		RemoveLayer(m_architecture[i]->id);

	m_architectureCount = 0;
}

GeneratorBase* LayerManager::GetCurrentLayer()
{
	return m_currentLayer;
}

//expects LayerInfo array at least MAX_RENDERABLE in length.
int LayerManager::GetLayers(LayerInfo *infos)
{
	for(int i = 0; i < m_architectureCount; i++)
	{
		Generator* gen = m_architecture[i];
		sprintf_s(infos[i].name, "%s %d", RenderableFactory::TypeInformation[gen->m_typeID].type_name, gen->id);
		infos[i].id = gen->id;
	}

	return m_architectureCount;
}

void LayerManager::InverseChecked(bool checked)
{
	if(m_currentLayer != NULL)
	{
		if(!IsCurrentLayer3d())
			((SubGenerator*)m_currentLayer)->m_inverse = checked;
	}
}

bool LayerManager::IsCurrentLayer3d()
{
	if(!m_currentLayer) return false;

	return RenderableFactory::TypeInformation[m_currentLayer->m_typeID].type_display_mode == DISPLAY_3D;
}

void LayerManager::SetInputLayer(int selectedVariable, int typeIndex)
{
	if(m_currentLayer != NULL)
	{
		//ensure that the selected sublayer type is not 3D
		if(RenderableFactory::TypeInformation[typeIndex].type_metatype == RENDERABLE_2D)
		{
			SubGenerator *newLayer = RenderableFactory::CreateLayer2d(typeIndex);

			m_currentLayer->AssignLayerToVariable(selectedVariable, newLayer);
			m_currentLayer = newLayer;
		}
	}
}

void LayerManager::NavigateInto(int selectedVariable)
{
	SubGenerator *sublayer = m_currentLayer->m_layerVariables[selectedVariable].GetRenderable();
	if((m_currentLayer != NULL) && (sublayer != NULL))
	{
		m_currentLayer = sublayer;
	}
}

void LayerManager::NavigateUp()
{
	if((m_currentLayer != NULL) && (m_currentLayer->ParentLayer != NULL))
	{
		m_currentLayer = m_currentLayer->ParentLayer;
	}
}

void LayerManager::GenerateEntities(Entity3dCollection *targetCollection, WindowMapping &mapping, float height)
{
	for(int i = 0; i < m_architectureCount; i++)
		m_architecture[i]->BuildCollection(targetCollection, mapping, height);
}

int LayerManager::GetArchitecturalTextureCount()
{
	return m_textureCount;
}

int LayerManager::GetTextureGroup(int GeneratedByID, int textureIndex)
{
	assert(textureIndex < m_textureCount);

	if(m_generatorTextures[textureIndex].generator_id != GeneratedByID)
		return -1;
	else 
		return m_generatorTextures[textureIndex].texture_group;

}

void LayerManager::BuildTextureList()
{
	m_textureCount = 0;
	for(int i = 0; i < m_architectureCount; i++)
	{
		//add these texture items.
		for(int j = 0; j < m_architecture[i]->GetTextureCount(); j++)
		{
			m_generatorTextures[m_textureCount].generator_id = m_architecture[i]->id;
			m_generatorTextures[m_textureCount].texture_group = j;
			m_textureCount++;
		}
	}
}