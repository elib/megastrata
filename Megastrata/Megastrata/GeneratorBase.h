#pragma once

#include <utils.h>
#include "WindowMapping.h"
#include "LayerVariable.h"
#include "Serializable.h"

#define MAX_VARIABLES 10

class GeneratorBase: public Serializable
{
public:
	GeneratorBase(int TypeID);
	virtual ~GeneratorBase(void);

	void SaveToXml(TiXmlElement *currentElement);
	void LoadFromXml(TiXmlElement *currentElement);


	virtual void ChangeParameter(int parameterIndex, float newParameter);
	virtual void IncrementParameter(int parameterIndex, float deltaParameter);
	void AssignLayerToVariable(int variableIndex, SubGenerator* subLayer);

	int m_variableCount;
	LayerVariable m_layerVariables[MAX_VARIABLES];

	virtual void Initialize();

	int id;
	int m_typeID;

	GeneratorBase *ParentLayer;

protected:

	virtual void SetVariables() = 0;

private:
	static int lastID;
};