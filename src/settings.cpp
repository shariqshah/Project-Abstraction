#include "settings.h"
#include "utilities.h"
#include "log.h"
#include "jsondefs.h"

namespace Settings
{	
	namespace
	{
		int windowWidth;
		int windowHeight;
		int renderWidth;
		int renderHeight;
		int shadowMapWidth;
		int shadowMapHeight;
		const char* settingsFile = "../content/settings.json";
	}
	
	bool initSettings()
	{
		using namespace rapidjson;
		bool success = true;
		char* settings = Utils::loadFileIntoCString(settingsFile);
		if(settings)
		{
			Document document;
			document.Parse(settings);
			if(document.IsObject())
			{
				if(document.HasMember("WindowWidth") && document["WindowWidth"].IsInt())
				{
					const int width = document["WindowWidth"].GetInt();
					if(width > 0)
						windowWidth = width;
					else
						success = false;
				}

				if(document.HasMember("WindowHeight") && document["WindowHeight"].IsInt())
				{
					const int height = document["WindowHeight"].GetInt();
					if(height > 0)
						windowHeight = height;
					else
						success = false;
				}

				if(document.HasMember("RenderWidth") && document["RenderWidth"].IsInt())
				{
					const int width = document["RenderWidth"].GetInt();
					if(width > 0)
						renderWidth = width;
					else
						success = false;
				}

				if(document.HasMember("RenderHeight") && document["RenderHeight"].IsInt())
				{
					const int height = document["RenderHeight"].GetInt();
					if(height > 0)
						renderHeight = height;
					else
						success = false;
				}

				if(document.HasMember("ShadowMapWidth") && document["ShadowMapWidth"].IsInt())
				{
					const int width = document["ShadowMapWidth"].GetInt();
					if(width > 0)
						shadowMapWidth = width;
					else
						success = false;
				}

				if(document.HasMember("ShadowMapHeight") && document["ShadowMapHeight"].IsInt())
				{
					const int height = document["ShadowMapHeight"].GetInt();
					if(height > 0)
						shadowMapHeight = height;
					else
						success = false;
				}
			}
			else
			{
				Log::error("Settings::initSettings", "File is invalid");
			}
			if(success)
				Log::message("Settings loaded from settings.json");
			free(settings);
		}
		else
		{
			success = false;
			Log::error("Settings::initSettings", "settings.json not found!");
		}

		if(!success)
		{
			 // Set defaults and save if file not found or erros reading
			renderWidth  = windowWidth  = 800;
			renderHeight = windowHeight = 600;
			shadowMapWidth = shadowMapHeight = 512;
			success = saveSettingsToFile();
		}
		return success;
	}

	bool saveSettingsToFile()
	{
		using namespace rapidjson;
		bool success = true;
		FILE* newFile = fopen(settingsFile, "w+");
		if(newFile)
		{
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);

			writer.StartObject();
			writer.Key("WindowWidth");     writer.Int(windowWidth);
			writer.Key("WindowHeight");    writer.Int(windowHeight);
			writer.Key("RenderWidth");     writer.Int(renderWidth);
			writer.Key("RenderHeight");    writer.Int(renderHeight);
			writer.Key("ShadowMapWidth");  writer.Int(shadowMapWidth);
			writer.Key("ShadowMapHeight"); writer.Int(shadowMapHeight);
			writer.EndObject();

			size_t bytes = fwrite((void*)buffer.GetString(), buffer.GetSize(), 1, newFile);
			if(bytes > 0)
			{
				Log::message("Settings saved to settings.json");
			}
			else
			{
				Log::error("Settings::initSettings", "Error writing settings.json");
				success = false;
			}
			fclose(newFile);
		}
		return success;
	}

	int getWindowWidth()
	{
		return windowWidth;
	}

	int getWindowHeight()
	{
		return windowHeight;
	}

	int getRenderWidth()
	{
		return renderWidth;
	}

	int getRenderHeight()
	{
		return renderHeight;
	}

	int getShadowMapWidth()
	{
		return shadowMapWidth;
	}

	int getShadowMapHeight()
	{
		return shadowMapHeight;
	}

	void setWindowWidth(int width)
	{
		windowWidth = width;
	}

	void setWindowHeight(int height)
	{
		windowHeight = height;
	}
	
	void setRenderWidth(int width)
	{
		renderWidth = width;
	}

	void setRenderHeight(int height)
	{
		renderHeight = height;
	}

	void setShadowMapWidth(int width)
	{
		shadowMapWidth = width;
	}

	void setShadowMapHeight(int height)
	{
		shadowMapHeight = height;
	}
	
}
