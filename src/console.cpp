#include "console.h"
#include "scenemanager.h"
#include "scriptengine.h"
#include "gui.h"

#include <vector>
#include <string.h>

namespace Console
{
	struct ConsoleMessage
	{
		ConsoleMessage(MessageType type, std::string message) : type(type), text(message) {}
		MessageType type;
		std::string text;
	};
	
	namespace
	{
		//int windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders;
		int windowFlags = ImGuiWindowFlags_NoCollapse;
		std::vector<ConsoleMessage> messages;
		bool windowShown   = true;
		bool dropDownShown = false;
		Vec4 colNormal     = Vec4(1.f);
		Vec4 colWarning    = Vec4(1.f, 1.f, 0.f, 1.f);
		Vec4 colError      = Vec4(1.f, 0.f, 0.f, 1.f);
		Vec4 colCommand    = Vec4(0.f, 0.63f, 1.f, 1.f);
		const int BUF_SIZE = 256;
		char commandBuf[BUF_SIZE] = "";
		std::vector<std::string> commandHistory;
		uint32_t historyPos = -1;
	}

	void initialize()
	{
	}
	
	void addMessage(MessageType type, const std::string& message)
	{
		std::string newMessage;
		switch(type)
		{
		case MSG_NORM:    newMessage = "[MSG] "  + message;	break;
		case MSG_WARNING: newMessage = "[WARN] " + message;	break;
		case MSG_ERROR:   newMessage = "[ERR] "  + message;	break;
		case MSG_COMMAND: newMessage = "[#] "    + message;	break;
		};
		messages.push_back(ConsoleMessage(type, newMessage));
	}
	
	void showDropDown(bool show)
	{
		dropDownShown = show;
	}
	
	void showWindow(bool show)
	{
		windowShown = show;
	}
	
	void cleanup()
	{
		messages.clear();
	}

	void executeCommand()
	{
		addMessage(MSG_COMMAND, std::string(&commandBuf[0]));
		commandHistory.push_back(std::string(&commandBuf[0]));
		// get the command
		char* commandLine = &commandBuf[0];
		char* command = strtok(commandLine, " ");
		//command = strtok(NULL, " ");
		// check if command is valid
		if(command)
		{
			const char* usageMessage = "";
			if(strcmp(command, "attach") == 0) // Attach script to gameObject
			{
				usageMessage = "usage : attach [gameobject name] [script name]";
				// get the name of each file and compile them
				const char* gameobjectName = strtok(NULL, " ");
				const char* scriptName       = strtok(NULL, " ");
				if(!scriptName)
				{
					addMessage(MSG_ERROR, "No script specified");
					addMessage(MSG_NORM, usageMessage);
				}
				else
				{
					if(!gameobjectName)
					{
						addMessage(MSG_ERROR, "No gameobject specified");
						addMessage(MSG_NORM, usageMessage);
					}
					else
					{
						GameObject* gameobject = SceneManager::find(gameobjectName);
						if(gameobject)
							ScriptEngine::addScript(gameobject, scriptName);
						else
							addMessage(MSG_ERROR, "'" + std::string(gameobjectName) + "' not found");
					}
				}		
			}
			else if(strcmp(command, "reload") == 0) // Reload script
			{
				usageMessage = "usage : reload [script name]";
				const char* scriptName = strtok(NULL, " ");
				if(scriptName)
				{
					ScriptEngine::reloadScript(scriptName);
				}
				else
				{
					addMessage(MSG_ERROR, "No script specified");
					addMessage(MSG_NORM, usageMessage);
				}	
			}
			else if(strcmp(command, "detach") == 0)
			{
				usageMessage = "detach [gameobject name] [script name]";
				const char* gameobjectName = strtok(NULL, " ");
				const char* scriptName     = strtok(NULL, " ");
				if(!scriptName)
				{
					addMessage(MSG_ERROR, "No script specified");
					addMessage(MSG_NORM, usageMessage);
				}
				else
				{
					if(!gameobjectName)
					{
						addMessage(MSG_ERROR, "No gameobject specified");
						addMessage(MSG_NORM, usageMessage);
					}
					else
					{
						GameObject* gameobject = SceneManager::find(gameobjectName);
						if(gameobject)
							ScriptEngine::removeScript(gameobject, scriptName);
						else
							addMessage(MSG_ERROR, "'" + std::string(gameobjectName) + "' not found");
					}
				}
			}
			else if(strcmp(command, "saveconf") == 0)
			{
				ScriptEngine::saveConfigToFile();
			}
			else
			{
				addMessage(MSG_ERROR, "Unrecognized command");
			}
		}
		memset(&commandBuf[0], '\0', BUF_SIZE);
	}
	
	void update()
	{
		if(windowShown)
		{
			bool opened = true;
			ImGui::Begin("Console", &opened, Vec2(400, 220), 1.f, windowFlags);

			static ImGuiTextFilter filter;
			filter.Draw("Filter entries");
			
			ImGui::BeginChild("ConsoleLog", Vec2(ImGui::GetWindowWidth() * 0.98f, 200), false);
			for(ConsoleMessage& message : messages)
			{
				if(!filter.PassFilter(message.text.c_str())) continue;		
				switch(message.type)
				{
				case MSG_NORM:    ImGui::PushStyleColor(ImGuiCol_Text, colNormal);	break;
				case MSG_WARNING: ImGui::PushStyleColor(ImGuiCol_Text, colWarning);	break;
				case MSG_ERROR:   ImGui::PushStyleColor(ImGuiCol_Text, colError);   break;
				case MSG_COMMAND: ImGui::PushStyleColor(ImGuiCol_Text, colCommand);	break;
				};
				ImGui::TextWrapped(message.text.c_str());
				ImGui::PopStyleColor();
			}
			static bool scrollToEnd = true;
			if(scrollToEnd)
			{
				ImGui::SetScrollPosHere();
				scrollToEnd = false;
			}
			ImGui::EndChild();

			if(ImGui::InputText("Command", &commandBuf[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if(commandBuf[0])
					executeCommand();
				scrollToEnd = true;
				ImGui::SetKeyboardFocusHere(-1);
			}
			if(ImGui::IsItemHovered()) ImGui::SetKeyboardFocusHere(-1);
			
			ImGui::End();
		}
	}
}
