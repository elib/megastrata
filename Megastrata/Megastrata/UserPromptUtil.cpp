//WINDOWS VERSION
#include "Windows.h"

#include "UserPromptUtil.h"
#include <utils.h>

#include <stdio.h>
#include <time.h>

#pragma(lib, "comdlg32.lib");

//internal utility function, attempts to find main GUI window of application using calling thread.
HWND GetMainWindow()
{
	GUITHREADINFO info;
	info.cbSize = sizeof(GUITHREADINFO);
	if(!GetGUIThreadInfo(NULL, &info))
		return 0;

	return info.hwndActive;
}

string UserPromptUtil::SaveFileName()
{
	char filename[1024];
	memset(filename, 0, sizeof(filename));

	OPENFILENAMEA Ofn;
	Ofn.lStructSize = sizeof(OPENFILENAMEA);
	Ofn.hwndOwner = GetMainWindow();
	Ofn.lpstrFilter = "XML Files (*.xml)\0*.xml\0All Files\0*.*\0\0";
	Ofn.lpstrFile= filename;
	Ofn.nMaxFile = sizeof(filename) / sizeof(*filename); 
	Ofn.lpstrFileTitle = NULL;
	Ofn.lpstrCustomFilter = NULL;
	Ofn.lpstrInitialDir = (LPSTR)NULL; 
	Ofn.Flags = OFN_OVERWRITEPROMPT; 
	Ofn.lpstrTitle = "ARIGHT";
	Ofn.lpstrDefExt = NULL;
	Ofn.hInstance = NULL;

	GetSaveFileNameA(&Ofn);
	return string(filename);
}

string UserPromptUtil::LoadFileName()
{
	char filename[1024];
	memset(filename, 0, sizeof(filename));

	OPENFILENAMEA Ofn;
	Ofn.lStructSize = sizeof(OPENFILENAMEA);
	Ofn.hwndOwner = GetMainWindow();
	Ofn.lpstrFilter = "XML Files (*.xml)\0*.xml\0All Files\0*.*\0\0";
	Ofn.lpstrFile= filename;
	Ofn.nMaxFile = sizeof(filename) / sizeof(*filename); 
	Ofn.lpstrFileTitle = NULL;
	Ofn.lpstrCustomFilter = NULL;
	Ofn.lpstrInitialDir = (LPSTR)NULL; 
	Ofn.Flags = OFN_OVERWRITEPROMPT; 
	Ofn.lpstrTitle = "ARIGHT";
	Ofn.lpstrDefExt = NULL;
	Ofn.hInstance = NULL;

	GetOpenFileNameA(&Ofn);
	return string(filename);
}

void UserPromptUtil::UserMessageBox(string text, string caption)
{
	//get main window
	HWND mainwnd = GetMainWindow();
	MessageBoxA(mainwnd, text.c_str(), caption.c_str(), MB_ICONINFORMATION);
}


UserPromptUtil::UserPromptUtil(void)
{
}

string UserPromptUtil::GetExecutablePath()
{
	char buffer[MAX_PATH];
    DWORD dwResult = GetModuleFileNameA(NULL, buffer, MAX_PATH);
	if(dwResult)
	{
		char *chop = strrchr(buffer, '\\');
#ifdef _DEBUG
		*chop = 0;
		chop = strrchr(buffer, '\\'); //perform again, go up one level
#endif
		chop++;
		*chop = 0;
		return string(buffer);
	}
	else
		return string("");
}

string UserPromptUtil::EnsureDirectory(string path, string dir)
{
	WIN32_FIND_DATAA data;
	//strcpy_s(data.cFileName, dir.c_str());
	string fullpath = path + dir;
	HANDLE hFind = FindFirstFileA(fullpath.c_str(), &data);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		if(CreateDirectoryA(fullpath.c_str(), NULL))
			return fullpath;
		else
			return "";
	}

	return fullpath;
}

void UserPromptUtil::SaveScreenShot(int width, int height)
{
	string path = GetExecutablePath();
	path = EnsureDirectory(path, SCREENSHOT_DIR);

	path.append("/");
	path.append("screenshot-");
	char dateStr [9];
	_strdate(dateStr);
	string date = dateStr;
	path.append(date.substr(6, 2));
	path.append("-");
	path.append(date.substr(0, 2));
	path.append("-");
	path.append(date.substr(3, 2));

	int i = 1;
	string filepath;

	do
	{
		char num[10];
		itoa(i++, num, 10);
		filepath = path + "_" + num + ".bmp";
	}
	while(FileExists(filepath.c_str()));

	///height *= 2;

	//filepath should be unique, or something.
	FILE *pf = fopen(filepath.c_str(), "wb");
	if(pf)
	{
		//fix for packing - crude solution
		while(width % 4 != 0) width--;

		glReadBuffer(GL_BACK);
		unsigned char *imageData = new unsigned char[width*height*3]; //Allocate memory for storing the image
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, imageData); //Copy

		BITMAPINFOHEADER header;
		header.biSize = sizeof(BITMAPINFOHEADER);
		header.biBitCount = 24;
		header.biHeight = height;
		header.biWidth = width;
		header.biCompression = BI_RGB;
		header.biSizeImage = width*height*3;


		BITMAPFILEHEADER bmfh;
        int nBitsOffset = sizeof(BITMAPFILEHEADER) + header.biSize; 
        LONG lImageSize = header.biSizeImage;
        LONG lFileSize = nBitsOffset + lImageSize;
        bmfh.bfType = 'B'+('M'<<8);
        bmfh.bfOffBits = nBitsOffset;
        bmfh.bfSize = lFileSize;
        bmfh.bfReserved1 = bmfh.bfReserved2 = 0;

		//Write the bitmap file header
        UINT nWrittenFileHeaderSize = fwrite(&bmfh, 1, 
                     sizeof(BITMAPFILEHEADER), pf);

		//And then the bitmap info header
        UINT nWrittenInfoHeaderSize = fwrite(&header, 
               1, sizeof(BITMAPINFOHEADER), pf);

        UINT nWrittenDIBDataSize = 
             fwrite(imageData, 1, lImageSize, pf);

        fclose(pf);

		delete [] imageData;
	}
}