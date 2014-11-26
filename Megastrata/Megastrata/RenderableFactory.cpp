#include "RenderableFactory.h"

#include "NoiseSlice.h"
#include "SquareOverlay.h"
#include "Circles.h"
#include "GeneratorBase.h"
#include "Boxes.h"
#include "JunctionBoxes.h"
#include "JunctionTester.h"
#include "DistributedFeature.h"

LayerTypeInfo RenderableFactory::TypeInformation[] = {
	{"Noise", RENDERABLE_2D, DISPLAY_2D},
	{"Squares", RENDERABLE_2D, DISPLAY_2D},
	{"Circles", RENDERABLE_2D, DISPLAY_2D},
	{"Boxes", RENDERABLE_3D, DISPLAY_3D},
	//{"ConnectingBoxes", RENDERABLE_3D, DISPLAY_3D},
	{"JunctionBoxes", RENDERABLE_3D, DISPLAY_3D},
	{"JunctionTester", RENDERABLE_3D, DISPLAY_3D},
	{"ElevatorShaft", DISTRIBUTED_FEATURE, DISPLAY_3D}

};

int RenderableFactory::LayerTypeCount = sizeof(TypeInformation) / sizeof(LayerTypeInfo);


SubGenerator* RenderableFactory::CreateLayer2d(int renderable_type)
{
	if(renderable_type < 0 || renderable_type >= LayerTypeCount)
		return NULL;

	SubGenerator *retval;

	switch(renderable_type)
	{
	case 0:
		retval = new NoiseSlice(renderable_type);
		break;
	case 1:
		retval = new SquareOverlay(renderable_type);
		break;
	case 2:
		retval = new Circles(renderable_type);
		break;
	default:
		return NULL;
		break;
	}

	retval->Initialize();
	return retval;
}

Generator* RenderableFactory::CreateGenerator(int renderable_type)
{
	if(renderable_type < 0 || renderable_type >= LayerTypeCount)
		return NULL;

	Generator *retval;

	switch(renderable_type)
	{
	case 3:
		retval = new Boxes(renderable_type);
		break;
	case 4:
		retval = new JunctionBoxes(renderable_type);
		break;
	case 5:
		retval = new JunctionTester(renderable_type);
		break;
	case 6:
		{
			DistributedFeature *feature = new DistributedFeature(renderable_type);
			feature->SetDimensions(3, 3, -1);
			feature->SetEntityType(RENDERED_SHAFT);
			retval = (Generator*) feature;
		}
		break;
	default:
		return NULL;
		break;
	}

	retval->Initialize();
	return retval;
}