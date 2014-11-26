#include "GeneratorBase.h"
#include "SubGenerator.h"
#include "WindowMapping.h"
#include "LayerVariable.h"
#include "World.h"

// [WODE] caught this - it was in the wrong place.
int GeneratorBase::lastID = 0;

GeneratorBase::GeneratorBase(int typeID)
{
	m_variableCount = 0;

	ParentLayer = NULL;

	//set unique id for this session.
	id = ++lastID;
	m_typeID = typeID;
}

void GeneratorBase::Initialize()
{
	SetVariables();
}

GeneratorBase::~GeneratorBase(void)
{

}



void GeneratorBase::AssignLayerToVariable(int variableIndex, SubGenerator* subLayer)
{
	if(variableIndex >= 0 && variableIndex < m_variableCount)
	{
		//link the sublayer's parent to this layer.
		m_layerVariables[variableIndex].SetRenderable(subLayer);
	}
}

void GeneratorBase::ChangeParameter(int variableIndex, float newParameter)
{
	if(variableIndex < m_variableCount)
	{
		m_layerVariables[variableIndex].SetValue(newParameter);
	}
}

void GeneratorBase::IncrementParameter(int variableIndex, float deltacutoff)
{
	ChangeParameter(variableIndex, m_layerVariables[variableIndex].GetOverrideValue() + deltacutoff);
}