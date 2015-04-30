#include "gui.h"
#include "input.h"
#include "settings.h"
#include "shader.h"
#include "passert.h"

#include "../include/SDL2/SDL.h"

#include "GL/glew.h"
#include "GL/gl.h"

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

namespace Gui
{
	static int shader_handle;
	static int texture_location;
	static size_t vbo_max_size = 20000;
	static unsigned int vbo_handle, vao_handle;
	static Mat4 projMat;

	void resize()
	{
		ImGuiIO& io = ImGui::GetIO();
		float winW = (float)Settings::getWindowWidth();
		float winH = (float)Settings::getWindowHeight();
		io.DisplaySize = ImVec2(winW, winH);
		projMat = glm::ortho(0.f, winW, winH, 0.f);
	}

	void setStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		// Colors
		style.Colors[ImGuiCol_Text]                  = ImVec4(0.97f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_Border]                = ImVec4(0.32f, 0.50f, 0.58f, 1.00f);
		style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
		style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.32f, 0.32f, 0.32f, 0.36f);
		style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.27f, 0.41f, 0.48f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.33f, 0.51f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.32f, 0.50f, 0.58f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.39f, 0.61f, 0.71f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.35f, 0.65f, 0.71f, 1.00f);
		style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
		style.Colors[ImGuiCol_CheckHovered]          = ImVec4(0.22f, 0.41f, 0.45f, 1.00f);
		style.Colors[ImGuiCol_CheckActive]           = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.92f, 0.92f, 0.92f, 0.50f);
		style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.25f, 0.47f, 0.52f, 1.00f);
		style.Colors[ImGuiCol_Button]                = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.19f, 0.29f, 0.34f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.31f, 0.47f, 0.55f, 1.00f);
		style.Colors[ImGuiCol_Header]                = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.27f, 0.50f, 0.55f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.33f, 0.60f, 0.66f, 1.00f);
		style.Colors[ImGuiCol_Column]                = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.39f, 0.61f, 0.71f, 1.00f);
		style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.35f, 0.65f, 0.71f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
		style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.27f, 0.50f, 0.55f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.33f, 0.60f, 0.66f, 1.00f);
		style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.39f, 0.61f, 0.71f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.39f, 0.61f, 0.71f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.24f, 0.38f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_TooltipBg]             = ImVec4(0.13f, 0.13f, 0.13f, 0.90f);

		// Sizes
		style.ScrollBarWidth = 13.f;
		style.Alpha          = 1.f;
		style.WindowRounding = 0.f;
	}
	
	static void renderImguiDisplayLists(ImDrawList** const cmd_lists, int cmd_lists_count)
	{
		if (cmd_lists_count == 0)
			return;

		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glActiveTexture(GL_TEXTURE0);

		// Setup orthographic projection matrix
		const float height = ImGui::GetIO().DisplaySize.y;
		Shader::bind(shader_handle);
		glUniform1i(texture_location, 0);
		Shader::setUniformMat4(shader_handle, "projMat", projMat);

		// Grow our buffer according to what we need
		size_t total_vtx_count = 0;
		for (int n = 0; n < cmd_lists_count; n++)
			total_vtx_count += cmd_lists[n]->vtx_buffer.size();
		glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
		size_t neededBufferSize = total_vtx_count * sizeof(ImDrawVert);
		if (neededBufferSize > vbo_max_size)
		{
			vbo_max_size = neededBufferSize + 5000;  // Grow buffer
			glBufferData(GL_ARRAY_BUFFER, vbo_max_size, NULL, GL_STREAM_DRAW);
		}

		// Copy and convert all vertices into a single contiguous buffer
		unsigned char* buffer_data = (unsigned char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		if (!buffer_data)
			return;
		for (int n = 0; n < cmd_lists_count; n++)
		{
			const ImDrawList* cmd_list = cmd_lists[n];
			memcpy(buffer_data, &cmd_list->vtx_buffer[0], cmd_list->vtx_buffer.size() * sizeof(ImDrawVert));
			buffer_data += cmd_list->vtx_buffer.size() * sizeof(ImDrawVert);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(vao_handle);

		int cmd_offset = 0;
		for (int n = 0; n < cmd_lists_count; n++)
		{
			const ImDrawList* cmd_list = cmd_lists[n];
			int vtx_offset = cmd_offset;
			const ImDrawCmd* pcmd_end = cmd_list->commands.end();
			for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->texture_id);
				glScissor((int)pcmd->clip_rect.x,
						  (int)(height - pcmd->clip_rect.w),
						  (int)(pcmd->clip_rect.z - pcmd->clip_rect.x),
						  (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
				glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
				vtx_offset += pcmd->vtx_count;
			}
			cmd_offset = vtx_offset;
		}

		// Restore modified state
		glBindVertexArray(0);
		Shader::unbind();
		glDisable(GL_SCISSOR_TEST);
		glBindTexture(GL_TEXTURE_2D, 0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	}

	void setClipboardText(const char* text)
	{
		SDL_SetClipboardText(text);
	}

	const char* getClipboardText()
	{
		return SDL_GetClipboardText();
	}

	void loadFontsTexture()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("../content/fonts/DroidSans.ttf", 14.0f);

		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

		GLuint tex_id;
		glGenTextures(1, &tex_id);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		// Store our identifier
		io.Fonts->TexID = (void *)(intptr_t)tex_id;
	}

	void update(float deltaTime)
	{
		ImGuiIO& io = ImGui::GetIO();
		resize();
		io.DeltaTime = deltaTime;

		// Mouse
		int mouseX = Input::getMouseX();
		int mouseY = Input::getMouseY();
		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = Input::isPressed(Input::MouseButton::M_LEFT);
		io.MouseDown[1] = Input::isPressed(Input::MouseButton::M_RIGHT);
		
		ImGui::NewFrame();

		if(io.WantCaptureKeyboard)
			Input::enableTextInput(true);
		else if(Input::isTextInputActive())
			Input::enableTextInput(false);
	}

	void render()
	{
		ImGui::Render();
	}
	
	void initialize()
	{
		shader_handle = Shader::create("gui.vert", "gui.frag");
		texture_location = Shader::getUniformLocation(shader_handle, "sampler");

		glGenBuffers(1, &vbo_handle);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
		glBufferData(GL_ARRAY_BUFFER, vbo_max_size, NULL, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &vao_handle);
		glBindVertexArray(vao_handle);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
		glEnableVertexAttribArray(Shader::POSITION_LOC);
		glEnableVertexAttribArray(Shader::UV_LOC);
		glEnableVertexAttribArray(Shader::COLOR_LOC);

		glVertexAttribPointer(Shader::POSITION_LOC,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(ImDrawVert),
							  (GLvoid*)OFFSETOF(ImDrawVert, pos));
		glVertexAttribPointer(Shader::UV_LOC,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(ImDrawVert),
							  (GLvoid*)OFFSETOF(ImDrawVert, uv));
		glVertexAttribPointer(Shader::COLOR_LOC,
							  4,
							  GL_UNSIGNED_BYTE,
							  GL_TRUE,
							  sizeof(ImDrawVert),
							  (GLvoid*)OFFSETOF(ImDrawVert, col));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		io.KeyMap[ImGuiKey_Tab]        = SDL_GetScancodeFromKey(Input::Key::TAB);
		io.KeyMap[ImGuiKey_LeftArrow]  = SDL_GetScancodeFromKey(Input::Key::LEFT);
		io.KeyMap[ImGuiKey_RightArrow] = SDL_GetScancodeFromKey(Input::Key::RIGHT);
		io.KeyMap[ImGuiKey_UpArrow]    = SDL_GetScancodeFromKey(Input::Key::UP);
		io.KeyMap[ImGuiKey_DownArrow]  = SDL_GetScancodeFromKey(Input::Key::DOWN);
		io.KeyMap[ImGuiKey_Home]       = SDL_GetScancodeFromKey(Input::Key::HOME);
		io.KeyMap[ImGuiKey_End]        = SDL_GetScancodeFromKey(Input::Key::END);
		io.KeyMap[ImGuiKey_Delete]     = SDL_GetScancodeFromKey(Input::Key::DELETE);
		io.KeyMap[ImGuiKey_Backspace]  = SDL_GetScancodeFromKey(Input::Key::BACKSPC);
		io.KeyMap[ImGuiKey_Enter]      = SDL_GetScancodeFromKey(Input::Key::ENTER);
		io.KeyMap[ImGuiKey_Escape]     = SDL_GetScancodeFromKey(Input::Key::ESC);
		io.KeyMap[ImGuiKey_A]          = SDL_GetScancodeFromKey(Input::Key::A);
		io.KeyMap[ImGuiKey_C]          = SDL_GetScancodeFromKey(Input::Key::C);
		io.KeyMap[ImGuiKey_V]          = SDL_GetScancodeFromKey(Input::Key::V);
		io.KeyMap[ImGuiKey_X]          = SDL_GetScancodeFromKey(Input::Key::X);
		io.KeyMap[ImGuiKey_Y]          = SDL_GetScancodeFromKey(Input::Key::Y);
		io.KeyMap[ImGuiKey_Z]          = SDL_GetScancodeFromKey(Input::Key::Z);

		io.RenderDrawListsFn  = renderImguiDisplayLists;
		io.SetClipboardTextFn = setClipboardText;
		io.GetClipboardTextFn = getClipboardText;

		resize();
		loadFontsTexture();
		setStyle();
	}

	void cleanup()
	{
		if(vao_handle) glDeleteVertexArrays(1, &vao_handle);
		if(vbo_handle) glDeleteBuffers(1, &vbo_handle);
		Shader::remove(shader_handle);
		ImGui::Shutdown();
	}

	void generateBindings()
	{
		
	}

	void updateKeyDown(uint8_t key, bool isDown, bool modCtrl, bool modShift)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[key] = isDown;
		io.KeyCtrl       = modCtrl;
		io.KeyShift      = modShift;
	}

	void updateMouseWheel(float scroll)
	{
		ImGuiIO* io = &ImGui::GetIO();
		io->MouseWheel += scroll;
	}

	void textEntered(const char* text)
	{
		ImGuiIO* io = &ImGui::GetIO();
		for(size_t i = 0; i < strlen(text); i++)
		{
			io->AddInputCharacter(text[i]);
		}
	}
}
