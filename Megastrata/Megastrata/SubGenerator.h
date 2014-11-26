#pragma once

#include <utils.h>
#include "GeneratorBase.h"
#include "WindowMapping.h"
#include "LayerVariable.h"
#include "RenderableFactory.h"
#include "Transform.h"



class SubGenerator : public GeneratorBase
{
public:
	SubGenerator(int TypeID);
	~SubGenerator(void);

	virtual void SaveToXml(TiXmlElement *currentElement);
	virtual void LoadFromXml(TiXmlElement *currentElement);

	void Translate(float delta_x, float delta_y);
	void Scale(float x, float y);

	void Render();
	void Update();

	void Initialize();

	float GetValueAt(float x, float y, float z);

	bool m_inverse;

protected:
	Transform m_transform;

	//physical bitmap
	GLubyte* m_bitmapBits;

	//GL texture!
	GLuint m_Texture;

	virtual float GetRawValueAt(float x, float y, float z) = 0;

	void WriteBitmap();
};