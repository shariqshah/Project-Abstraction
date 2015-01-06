#include "settings.h"

namespace Settings
{
	namespace
	{
		int cWindowWidth;
		int cWindowHeight;
	}
	
	bool initSettings()
	{
		// TODO: Load settings from a file instead
		cWindowWidth  = 800;
		cWindowHeight = 600;

		return true;
	}

	int getWindowWidth()
	{
		return cWindowWidth;
	}

	int getWindowHeight()
	{
		return cWindowHeight;
	}

	void setWindowWidth(int width)
	{
		cWindowWidth = width;
	}

	void setWindowHeight(int height)
	{
		cWindowHeight = height;
	}
	
}
