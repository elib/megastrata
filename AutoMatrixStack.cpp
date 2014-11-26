#include "AutoMatrixStack.h"

#include <utils.h>

int AutoMatrixStack::maxdepth = 0;
int AutoMatrixStack::max_found_depth = 0;


AutoMatrixStack::AutoMatrixStack(void)
{
	stackdepth = 0;

#ifdef _DEBUG
	if(maxdepth == 0)
	{
		glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &maxdepth);
		LOG("Max matrix depth: %d", maxdepth);
	}
#endif
	
	Push();
}

AutoMatrixStack::~AutoMatrixStack(void)
{
	PopAll();
}


void AutoMatrixStack::Push()
{
	stackdepth++;
#ifdef _DEBUG
	int currdepth;
	glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &currdepth);
	if(currdepth >= maxdepth)
	{
		LOG("Hey there, you're past the limit of max modelview matrix depth! current: %d, max: %d", stackdepth, maxdepth);
	}

	if(currdepth > max_found_depth)
	{
		max_found_depth = currdepth;
		LOG("Deepest stack so far: %d", max_found_depth);
	}
#endif

	glPushMatrix();
	
}

void AutoMatrixStack::Pop()
{
	stackdepth--;
	glPopMatrix();
}

void AutoMatrixStack::PopAll()
{
	while(stackdepth > 0)
	{
		Pop();
	}
}