#include "SubGenerator.h"
#include "World.h"

//#include "../../gauss/gaussianblur.h"

SubGenerator::SubGenerator(int TypeID) : GeneratorBase(TypeID)
{
	m_inverse = false;

	m_bitmapBits = NULL;
	m_Texture = 0;

}

SubGenerator::~SubGenerator(void)
{
	glDeleteTextures(1, &m_Texture);
	delete [] m_bitmapBits;
}

#define FIND_MINMAX

void SubGenerator::WriteBitmap()
{
	//creating 2d bitmap texture.
	GLubyte gray;

#ifdef FIND_MINMAX
	float min = 0.5f, max = 0.0f;
#endif

	for(int i = 0; i < World::theWorld->m_width; i++)
	{
		for(int j = 0; j < World::theWorld->m_height; j++)
		{
			float x, y; x = i; y = j;
			World::theWorld->GetWorldCoordinates(x, y);
			float value = GetValueAt(x, y, 0);
			gray = (GLubyte) 0xFF * value;

#ifdef FIND_MINMAX
			if(value < min) min = value;
			if(value > max) max = value;
#endif

			m_bitmapBits[i + World::theWorld->m_width*j] = gray;

		}
	}

#ifdef FIND_MINMAX
	LOG("Min: %g, max: %g", min, max);
#endif


//	TGaussianBlur<GLubyte> BlurFilter;
//	BlurFilter.Filter(m_bitmapBits, NULL, m_layerManager->m_width, m_layerManager->m_height, 51);
}


void SubGenerator::Update()
{
	// write to the texture
	WriteBitmap();

	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Min Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Mag Filter

	if(!m_bitmapBits)
	{
		//I don't think this part gets called - because we never recreate the bitmap.
		//resizing the window SHOULD do that, eventually.
		GLubyte gray = 0x33;
		glTexImage2D(GL_TEXTURE_2D, 0, 1, 1, 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, &gray);
	}
	else
		glTexImage2D(GL_TEXTURE_2D, 0, 1, World::theWorld->m_width, World::theWorld->m_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_bitmapBits);

}

void SubGenerator::Render()
{
	glColor4f(1.0f,1.0f,1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	glBegin(GL_QUADS);
		// Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0,		0,    0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f,	0.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f,  1.0f,  0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f,  1.0f,  0.0f);
	glEnd();
}

float SubGenerator::GetValueAt(float x, float y, float z)
{
	//transform to this sub-layer
	m_transform.Apply(x, y);

	//perform further standard checks
	float value = GetRawValueAt(x, y, z);
	if(m_inverse)
		return (1.0 - value);

	return value;
}

void SubGenerator::Initialize()
{
	GeneratorBase::Initialize();
	delete[] m_bitmapBits;
	m_bitmapBits = NULL;
	
	//generate textures
	glGenTextures(1, &m_Texture);

	m_bitmapBits = new GLubyte[World::theWorld->m_width * World::theWorld->m_height];
}

void SubGenerator::Translate(float delta_x, float delta_y)
{
	m_transform.Translation.x += delta_x / m_transform.Scale.x;
	m_transform.Translation.y -= delta_y / m_transform.Scale.y;
}


void SubGenerator::Scale(float x, float y)
{
	float s_x = 1.0f - x;
	float s_y = 1.0f - y;
	assert(s_x > 0);
	assert(s_y > 0);

	m_transform.Scale.x *= s_x;
	m_transform.Scale.y *= s_y;
}
