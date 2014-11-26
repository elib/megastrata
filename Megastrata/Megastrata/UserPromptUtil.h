#pragma once
#include <string>
using namespace std;

#define SCREENSHOT_DIR		"screenshots"

class UserPromptUtil
{
public:
	static string SaveFileName();
	static string LoadFileName();

	static void UserMessageBox(string text, string caption = string("Megastrata"));

	static string GetExecutablePath();
	static string EnsureDirectory(string path, string dir);
	static void SaveScreenShot(int width, int height);

private:
	//private constructor, just a few static functions available.
	UserPromptUtil(void);
};