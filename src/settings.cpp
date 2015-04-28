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
				if(document.HasMember("Width") && document["Width"].IsInt())
				{
					const int width = document["Width"].GetInt();
					if(width > 0)
						windowWidth = width;
					else
						success = false;
				}

				if(document.HasMember("Height") && document["Height"].IsInt())
				{
					const int height = document["Height"].GetInt();
					if(height > 0)
						windowHeight = height;
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
			windowWidth  = 800;
			windowHeight = 600;
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
			writer.Key("Width");  writer.Int(windowWidth);
			writer.Key("Height"); writer.Int(windowHeight);
			writer.EndObject();

			size_t rc = fwrite((void*)buffer.GetString(), buffer.GetSize(), 1, newFile);
			if(rc > 0)
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

	void setWindowWidth(int width)
	{
		windowWidth = width;
	}

	void setWindowHeight(int height)
	{
		windowHeight = height;
	}
	
}
