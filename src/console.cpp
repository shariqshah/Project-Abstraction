#include "console.h"
#include "gui.h"

#include <vector>

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
		int windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders;
		std::vector<ConsoleMessage> messages;
		bool windowShown   = false;
		bool dropDownShown = false;
		Vec4 colNormal     = Vec4(1.f);
		Vec4 colWarning    = Vec4(1.f, 1.f, 0.f, 1.f);
		Vec4 colError      = Vec4(1.f, 0.f, 0.f, 1.f);
		Vec4 colCommand    = Vec4(0.f, 1.f, 0.f, 1.f);
		const int BUF_SIZE = 256;
		char commandBuf[BUF_SIZE] = "";
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

			ImGui::Separator();
			if(ImGui::InputText("Command", &commandBuf[0], BUF_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				executeCommand();
				scrollToEnd = true;
			}
			
			ImGui::End();
		}
	}
}
