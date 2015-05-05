#ifndef settings_H_
#define settings_H_

namespace Settings
{
	bool initSettings();
	bool saveSettingsToFile();
	int  getWindowWidth();
	int  getRenderHeight();
	int  getRenderWidth();
	int  getShadowMapHeight();
	int  getShadowMapWidth();
	int  getWindowHeight();
	void setWindowWidth(int width);
	void setWindowHeight(int height);
	void setRenderWidth(int width);
	void setRenderHeight(int height);
	void setShadowMapWidth(int width);
	void setShadowMapHeight(int height);
}

#endif
