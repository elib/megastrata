#include "LayerVariable.h"
#include "SubGenerator.h"

#include "../../gauss/gaussianblur.h"

#define GAUSS_WINDOW_WIDTH 19
#define GAUSS_EXPANSION_FACTOR 10

LayerVariable::LayerVariable(void)
{
	//defaults, so we don't have garbage vars!
	blurredSubimage = NULL;
	renderable_value = NULL;
	m_overrideValue = 0.0f;
	m_maxValue = 1.0f;
	m_minValue = -1.0f;
	belongs_to_renderable = NULL;
	memset(name, 0, sizeof(name));
}

const char* LayerVariable::GetName()
{
	return name;
}

void LayerVariable::Initialize(GeneratorBase *belongs_to, const char* newName, float overrideValue, float maxVal, float minVal)
{
	belongs_to_renderable = belongs_to;
	m_overrideValue = overrideValue;
	m_maxValue = maxVal;
	m_minValue = minVal;
	sprintf_s(name, newName);
}

LayerVariable::~LayerVariable(void)
{
	RemoveRenderable();
}

void LayerVariable::RemoveRenderable()
{
	if(renderable_value != NULL)
		delete renderable_value;
	renderable_value = NULL;
}

float LayerVariable::GetValue(float x, float y, float z, float cutoff)
{
	if(renderable_value == NULL)
		return GetOverrideValue();
	else
	{
		//GetValueAt returns a value between 0 and 1.
		float subLayerValue	= renderable_value->GetValueAt(x,y,z);

		if (subLayerValue < cutoff)
		{
			return 0;
		}
		
		//normalize
		float scale = (subLayerValue - cutoff) / (1.0 - cutoff);
		//Rescale to proper range:
		return (((m_maxValue - m_minValue) * scale) + m_minValue);
	}

	return 0;
}

float LayerVariable::GetDownSampledValue(int x, int y, float cutoff)
{
	if(renderable_value == NULL)
		return GetOverrideValue();
	else
	{
		//GetValueAt returns a value between 0 and 1.
		if(!blurredSubimage)
			return 0;

		//float subLayerValue	= blurredSubimage[(GAUSS_WINDOW_WIDTH + x)*2 + m_downsampleMapping.GetPhysicalWidth() * (GAUSS_WINDOW_WIDTH + y) * 2];
		float subLayerValue	= blurredSubimage[(x + m_downsampleMapping.GetPhysicalWidth() * y ) * GAUSS_EXPANSION_FACTOR];

		if (subLayerValue < cutoff)
		{
			return 0;
		}
		
		//normalize
		float scale = (subLayerValue - cutoff) / (1.0 - cutoff);
		//Rescale to proper range:
		return (((m_maxValue - m_minValue) * scale) + m_minValue);
	}

	return 0;
}


void LayerVariable::PrepareDownSample(WindowMapping &mapping, float height)
{
	if(!renderable_value)
	{
		delete [] blurredSubimage;
		blurredSubimage = NULL;
		return;
	}

	//double resolution
	m_downsampleMapping = mapping;
	//int xwidth = (mapping.GetPhysicalWidth() + GAUSS_WINDOW_WIDTH) * 2;
	//int ywidth = (mapping.GetPhysicalHeight() + GAUSS_WINDOW_WIDTH) * 2;
	int xwidth = mapping.GetPhysicalWidth() * GAUSS_EXPANSION_FACTOR;
	int ywidth = mapping.GetPhysicalHeight() * GAUSS_EXPANSION_FACTOR;

	if(blurredSubimage)
		delete [] blurredSubimage;
	blurredSubimage = new float[xwidth * ywidth];

	m_downsampleMapping.SetPhysicalWindow(xwidth, ywidth);

	for(int i = 0; i < xwidth; i++)
	{
		for(int j = 0; j < ywidth; j++)
		{
			float xpos = i, ypos = j;
			m_downsampleMapping.GetWorldCoordinates(xpos, ypos);
			blurredSubimage[i + xwidth*j] = renderable_value->GetValueAt(xpos, ypos, height);
		}
	}

	TGaussianBlur<float> BlurFilter;
	BlurFilter.Filter(blurredSubimage, NULL, xwidth, ywidth, GAUSS_WINDOW_WIDTH);
}

float LayerVariable::GetOverrideValue()
{
	return m_overrideValue;
}

void LayerVariable::SetValue(float value)
{
	//clamp values.
	float finalValue;
	finalValue = MAX(m_minValue, value);
	finalValue = MIN(m_maxValue, finalValue);

	m_overrideValue = finalValue;
}

SubGenerator* LayerVariable::GetRenderable()
{
	return renderable_value;
}

void LayerVariable::SetRenderable(SubGenerator* layer)
{
	if(renderable_value != NULL)
		RemoveRenderable();

	renderable_value = layer;
	layer->ParentLayer = belongs_to_renderable;
}

void LayerVariable::Translate(float delta_x, float delta_y)
{
	if(renderable_value != NULL)
	{
		renderable_value->Translate(delta_x, delta_y);
	}
}

void LayerVariable::Scale(float scale_x, float scale_y)
{
	if(renderable_value != NULL)
		renderable_value->Scale(scale_x, scale_y);
}