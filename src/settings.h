#ifndef settings_H_
#define settings_H_

namespace Settings
{
	bool initSettings();
	bool saveSettingsToFile();
	int  getWindowWidth();
	int  getWindowHeight();
	void setWindowWidth(int width);
	void setWindowHeight(int height);
}

#endif
