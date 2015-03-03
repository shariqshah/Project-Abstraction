#include "gui.h"
#include "input.h"
#include "settings.h"

#include "imgui.h"
#include "../include/SDL2/SDL.h"

#include "GL/glew.h"
#include "GL/gl.h"

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

namespace Gui
{
	static int shader_handle, vert_handle, frag_handle;
	static int texture_location, proj_mtx_location;
	static int position_location, uv_location, colour_location;
	static size_t vbo_max_size = 20000;
	static unsigned int vbo_handle, vao_handle;
	
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
		const float width = ImGui::GetIO().DisplaySize.x;
		const float height = ImGui::GetIO().DisplaySize.y;
		const float ortho_projection[4][4] =
			{
				{ 2.0f/width,	0.0f,			0.0f,		0.0f },
				{ 0.0f,			2.0f/-height,	0.0f,		0.0f },
				{ 0.0f,			0.0f,			-1.0f,		0.0f },
				{ -1.0f,		1.0f,			0.0f,		1.0f },
			};
		glUseProgram(shader_handle);
		glUniform1i(texture_location, 0);
		glUniformMatrix4fv(proj_mtx_location, 1, GL_FALSE, &ortho_projection[0][0]);

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
		glUseProgram(0);
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

	void LoadFontsTexture()
	{
		ImGuiIO& io = ImGui::GetIO();
		//ImFont* my_font1 = io.Fonts->AddFontDefault();
		// ImFont* droidSans = io.Fonts->AddFontFromFileTTF("../content/fonts/DroidSans.ttf", 15.0f);
		//ImFont* my_font3 = io.Fonts->AddFontFromFileTTF("extra_fonts/ProggyClean.ttf", 13.0f); my_font3->DisplayOffset.y += 1;
		//ImFont* my_font4 = io.Fonts->AddFontFromFileTTF("extra_fonts/ProggyTiny.ttf", 10.0f); my_font4->DisplayOffset.y += 1;
		//ImFont* my_font5 = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 20.0f, io.Fonts->GetGlyphRangesJapanese());

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
		int winW = Settings::getWindowWidth();
		int winH = Settings::getWindowHeight();
		io.DisplaySize = ImVec2(winW, winH);
		io.DeltaTime   = deltaTime;

		// Mouse
		int mouseX = Input::getMouseX();
		int mouseY = Input::getMouseY();
		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = Input::isPressed(Input::MouseButton::M_LEFT);
		io.MouseDown[1] = Input::isPressed(Input::MouseButton::M_RIGHT);
		
		ImGui::NewFrame();
	}

	void render()
	{
		ImGui::Render();
	}
	
	void initialize()
	{
		const GLchar *vertex_shader =
			"#version 130\n"
			"uniform mat4 ProjMtx;\n"
			"in vec2 Position;\n"
			"in vec2 UV;\n"
			"in vec4 Color;\n"
			"out vec2 Frag_UV;\n"
			"out vec4 Frag_Color;\n"
			"void main()\n"
			"{\n"
			"	Frag_UV = UV;\n"
			"	Frag_Color = Color;\n"
			"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
			"}\n";

		const GLchar* fragment_shader =
			"#version 130\n"
			"uniform sampler2D Texture;\n"
			"in vec2 Frag_UV;\n"
			"in vec4 Frag_Color;\n"
			"out vec4 Out_Color;\n"
			"void main()\n"
			"{\n"
			"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
			"}\n";

		shader_handle = glCreateProgram();
		vert_handle = glCreateShader(GL_VERTEX_SHADER);
		frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vert_handle, 1, &vertex_shader, 0);
		glShaderSource(frag_handle, 1, &fragment_shader, 0);
		glCompileShader(vert_handle);
		glCompileShader(frag_handle);
		glAttachShader(shader_handle, vert_handle);
		glAttachShader(shader_handle, frag_handle);
		glLinkProgram(shader_handle);

		texture_location = glGetUniformLocation(shader_handle, "Texture");
		proj_mtx_location = glGetUniformLocation(shader_handle, "ProjMtx");
		position_location = glGetAttribLocation(shader_handle, "Position");
		uv_location = glGetAttribLocation(shader_handle, "UV");
		colour_location = glGetAttribLocation(shader_handle, "Color");

		glGenBuffers(1, &vbo_handle);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
		glBufferData(GL_ARRAY_BUFFER, vbo_max_size, NULL, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &vao_handle);
		glBindVertexArray(vao_handle);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
		glEnableVertexAttribArray(position_location);
		glEnableVertexAttribArray(uv_location);
		glEnableVertexAttribArray(colour_location);

		glVertexAttribPointer(position_location,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(ImDrawVert),
							  (GLvoid*)OFFSETOF(ImDrawVert, pos));
		glVertexAttribPointer(uv_location,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(ImDrawVert),
							  (GLvoid*)OFFSETOF(ImDrawVert, uv));
		glVertexAttribPointer(colour_location,
							  4,
							  GL_UNSIGNED_BYTE,
							  GL_TRUE,
							  sizeof(ImDrawVert),
							  (GLvoid*)OFFSETOF(ImDrawVert, col));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		io.KeyMap[ImGuiKey_Tab]        = Input::Key::TAB;
		io.KeyMap[ImGuiKey_LeftArrow]  = Input::Key::LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = Input::Key::RIGHT;
		io.KeyMap[ImGuiKey_UpArrow]    = Input::Key::UP;
		io.KeyMap[ImGuiKey_DownArrow]  = Input::Key::DOWN;
		io.KeyMap[ImGuiKey_Home]       = Input::Key::HOME;
		io.KeyMap[ImGuiKey_End]        = Input::Key::END;
		io.KeyMap[ImGuiKey_Delete]     = Input::Key::DELETE;
		io.KeyMap[ImGuiKey_Backspace]  = Input::Key::BACKSPC;
		io.KeyMap[ImGuiKey_Enter]      = Input::Key::ENTER;
		io.KeyMap[ImGuiKey_Escape]     = Input::Key::ESC;
		io.KeyMap[ImGuiKey_A]          = Input::Key::A;
		io.KeyMap[ImGuiKey_C]          = Input::Key::C;
		io.KeyMap[ImGuiKey_V]          = Input::Key::V;
		io.KeyMap[ImGuiKey_X]          = Input::Key::X;
		io.KeyMap[ImGuiKey_Y]          = Input::Key::Y;
		io.KeyMap[ImGuiKey_Z]          = Input::Key::Z;

		io.RenderDrawListsFn  = renderImguiDisplayLists;
		io.SetClipboardTextFn = setClipboardText;
		io.GetClipboardTextFn = getClipboardText;

		LoadFontsTexture();
	}

	void cleanup()
	{
		if (vao_handle) glDeleteVertexArrays(1, &vao_handle);
		if (vbo_handle) glDeleteBuffers(1, &vbo_handle);
		glDetachShader(shader_handle, vert_handle);
		glDetachShader(shader_handle, frag_handle);
		glDeleteShader(vert_handle);
		glDeleteShader(frag_handle);
		glDeleteProgram(shader_handle);
		ImGui::Shutdown();
	}
	
	void text(const char* string)
	{
		ImGui::Text(string);
	}

	bool button(const char* string)
	{
		return ImGui::Button(string);
	}
}
