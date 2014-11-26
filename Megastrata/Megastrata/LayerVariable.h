#pragma once
#include <utils.h>
#include "serializable.h"
#include "WindowMapping.h"

class LayerVariable : public Serializable
{
public:
	LayerVariable(void);
	~LayerVariable(void);

	SubGenerator* GetRenderable();
	void SetRenderable(SubGenerator* layer);
	void RemoveRenderable();
	
	void PrepareDownSample(WindowMapping &mapping, float height);
	float GetDownSampledValue(int x, int y, float cutoff = 0);

	float GetValue(float x, float y, float z, float cutoff = 0);
	float GetOverrideValue();
	void SetValue(float value);

	virtual void SaveToXml(TiXmlElement *currentElement);
	virtual void LoadFromXml(TiXmlElement *currentElement);

	const char* GetName();

	void Initialize(GeneratorBase *belongs_to, const char* newName, float overrideValue = 0.0f, float maxVal = 1.0f, float minVal = -1.0f);

	//navigation
	void Scale(float scale_x, float scale_y);
	void Translate(float delta_x, float delta_y);

private:
	//VariableType variable_type;
	float m_minValue, m_maxValue;
	char name[100];
	SubGenerator *renderable_value;
	GeneratorBase *belongs_to_renderable;

	WindowMapping m_downsampleMapping;
	float *blurredSubimage;
	
	//the override value
	float m_overrideValue;
};