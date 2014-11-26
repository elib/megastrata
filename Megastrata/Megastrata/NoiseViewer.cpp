#include "ProgramManager.h"


#if defined(_WIN32)
	int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lPCmdLine, int nShowCmd)
#else
	int main(int argc, char **argv)
#endif
{
	ProgramManager program;
	return program.Run(0, NULL);
}