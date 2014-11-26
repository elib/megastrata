#pragma once

#include <utils.h>
class Generator;

enum RenderableMetaType
{
	RENDERABLE_2D,
	RENDERABLE_3D,
	DISTRIBUTED_FEATURE
};

enum DisplayMode
{
	DISPLAY_3D,
	DISPLAY_2D
};

typedef struct layer_type_info
{
	char type_name[100];
	RenderableMetaType type_metatype;
	DisplayMode type_display_mode;

} LayerTypeInfo;

class RenderableFactory
{
public:
	static SubGenerator* CreateLayer2d(int renderable_type);
	static Generator* CreateGenerator(int renderable_type);
	static LayerTypeInfo TypeInformation[];
	static int LayerTypeCount;
private:
	//static functions only
	RenderableFactory(void){}
};