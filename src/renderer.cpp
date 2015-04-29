#include <GL/glew.h>
#include <GL/gl.h>

#include "renderer.h"
#include "texture.h"
#include "shader.h"
#include "material.h"
#include "model.h"
#include "camera.h"
#include "settings.h"
#include "passert.h"
#include "geometry.h"
#include "framebuffer.h"

namespace Renderer
{
	namespace
	{
        char*      contentDir;
		DebugLevel sDebugLevel;
		bool       sRenderWireframe;
		bool       sRenderDebugView;
		Vec4       clearColor = Vec4(1.f);
		std::vector<std::string> sTextList;
		const char* texDir         = "/textures/";
		const char* shaderDir      = "/shaders/";
		const char* modelDir       = "/models/";
		const char* contentDirName = "/../content";
		RenderParams renderParams;

		GLuint textVAO      = 0;
		GLuint textVertVBO  = 0;
		GLuint textUVBO     = 0;
		GLuint textIndexVBO = 0;

		unsigned int textVertTop  = 0;
		unsigned int textUVTop    = 0;
		unsigned int textIndexTop = 0;
		
		uint32_t MAX_TEXT_VERT_VBO = 4 * 1000 * sizeof(Vec2);
		uint32_t MAX_TEXT_UV_VBO   = 4 * 1000 * sizeof(Vec2);
		uint32_t MAX_TEXT_IND_VBO  = 6 * 1000 * sizeof(GLuint);

		std::vector<Vec2>     quadVerts;
		std::vector<Vec2>     quadUVs;
		std::vector<uint32_t> quadIndices;

		std::vector<Rect> textList;
		int textShader = -1;

		Vec4 textColor = Vec4(1, 1, 0, 1);
	    Mat4 textProjMat;

		int texture = -1;

		void updateTextVBOs()
		{
			std::vector<Vec2>     totalVertices;
			std::vector<Vec2>     totalUVs;
			std::vector<uint32_t> totalIndices;
		
			for(uint32_t i = 0; i < textList.size(); i++)
			{			
				for(Vec2 vertex : quadVerts)
					totalVertices.push_back(vertex);

				for(Vec2 uv : quadUVs)
					totalUVs.push_back(uv);

				for(uint32_t index : quadIndices)
					totalIndices.push_back((i * 6) + index);
			}

			textVertTop  = totalVertices.size();
			textUVTop    = totalUVs.size();
			textIndexTop = totalIndices.size();
		
			glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_TEXT_VERT_VBO, totalVertices.data());
			checkGLError("Renderer::addRect::Vertices");
			glBindBuffer(GL_ARRAY_BUFFER, 0);			
		
			glBindBuffer(GL_ARRAY_BUFFER, textUVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_TEXT_UV_VBO, totalUVs.data());
			checkGLError("Renderer::addRect::UVs");
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textIndexVBO);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, MAX_TEXT_IND_VBO, totalIndices.data());
			checkGLError("Renderer::addRect::Indices");
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		// uint temp_vao;
	}

	void initText()
	{
		texture = Texture::create("font2.png");
		// texture = Texture::create("chessboard.png");
		
		// Load shader for text rendering
	    textShader = Shader::create("quad.vert", "quad.frag");
		
		glGenVertexArrays(1, &textVAO);
		glBindVertexArray(textVAO);

		// Vertices
		glGenBuffers(1, &textVertVBO);
		glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
		glBufferData(GL_ARRAY_BUFFER,
					 MAX_TEXT_VERT_VBO,
					 NULL,
					 GL_STREAM_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		checkGLError("Renderer::initText::Vertices");
		
		// UVs
		glGenBuffers(1, &textUVBO);
		glBindBuffer(GL_ARRAY_BUFFER, textUVBO);
		glBufferData(GL_ARRAY_BUFFER,
					 MAX_TEXT_UV_VBO,
					 NULL,
					 GL_STREAM_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		checkGLError("Renderer::initText::UVs");

		// Indices
		glGenBuffers(1, &textIndexVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textIndexVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					 MAX_TEXT_IND_VBO,
					 NULL,
					 GL_STREAM_DRAW);
		checkGLError("Renderer::initText::Indices");
		
		glBindVertexArray(0);
		checkGLError("Renderer::initText::VAO");

		quadVerts.push_back(Vec2(-0.5f,  0.5f));
		quadVerts.push_back(Vec2(-0.5f, -0.5f));
		quadVerts.push_back(Vec2( 0.5f, -0.5f));
		quadVerts.push_back(Vec2( 0.5f,  0.5f));

		// quadVerts.push_back(Vec2(-1.f,  1.f));
		// quadVerts.push_back(Vec2(-1.f, -1.f));
		// quadVerts.push_back(Vec2( 1.f, -1.f));
		// quadVerts.push_back(Vec2( 1.f,  1.f));

		char letter = 'B';
		// float x = (letter % 16)/16.f;
		// float y = (letter / 16)/16.f;
		float x = (letter % 16);
		float y = (letter / 16);
		x /= 16.f;
		y = ((int)y % 16) / 16.f;
		
		// quadUVs.push_back(Vec2(x           , 1.f - y));
		// quadUVs.push_back(Vec2(x           , 1.f - (y + 1.f/16.f)));
		// quadUVs.push_back(Vec2(x + 1.f/16.f, 1.f - (y + 1.f/16.f)));
		// quadUVs.push_back(Vec2(x + 1.f/16.f, 1.f - y));

		float offset = 1.f/16.f;
		// float start  = (6 % 16) / 16.f;
		quadUVs.push_back(Vec2(x           , y));
		quadUVs.push_back(Vec2(x           , y + offset));
		quadUVs.push_back(Vec2(x + 1.f/16.f, y + offset));
		quadUVs.push_back(Vec2(x + 1.f/16.f, y));
		
		// quadUVs.push_back(Vec2(0, 1));
		// // quadUVs.push_back(Vec2(0, 1));
		// quadUVs.push_back(Vec2(0, 0));
		// quadUVs.push_back(Vec2(1, 0));
		// // quadUVs.push_back(Vec2(1, 0));
		// quadUVs.push_back(Vec2(1, 1));
		// // quadUVs.push_back(Vec2(0, 1));

		quadIndices.push_back(0);
		quadIndices.push_back(1);
		quadIndices.push_back(2);	
		quadIndices.push_back(2);
		quadIndices.push_back(3);
		quadIndices.push_back(0);

		textProjMat = glm::ortho(0.f, 10.f, 0.f, 10.f);



		//////////////////////////////////////////////////////////////////////////////
		//TESTING, DRAGONS AHOY!
		//////////////////////////////////////////////////////////////////////////////
		// glGenVertexArrays(1, &temp_vao);
		// glBindVertexArray(temp_vao);

		// // Vertices
		// uint temp_vert_vbo;
		// glGenBuffers(1, &temp_vert_vbo);
		// glBindBuffer(GL_ARRAY_BUFFER, temp_vert_vbo);
		// glBufferData(GL_ARRAY_BUFFER,
		// 			 4 * sizeof(Vec2),
		// 			 quadVerts.data(),
		// 			 GL_STATIC_DRAW);
		// glEnableVertexAttribArray(0);
		// glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		// checkGLError("Renderer::initText::Vertices");
		
		// // UVs
		// uint temp_uv_vbo;
		// glGenBuffers(1, &temp_uv_vbo);
		// glBindBuffer(GL_ARRAY_BUFFER, temp_uv_vbo);
		// glBufferData(GL_ARRAY_BUFFER,
		// 			 4 * sizeof(Vec2),
		// 			 quadUVs.data(),
		// 			 GL_STATIC_DRAW);
		// glEnableVertexAttribArray(2);
		// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		// checkGLError("Renderer::initText::UVs");

		// // Indices
		// uint temp_index_vbo;
		// glGenBuffers(1, &temp_index_vbo);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp_index_vbo);
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		// 			 6 * sizeof(GLuint),
		// 			 quadIndices.data(),
		// 			 GL_STATIC_DRAW);
		// checkGLError("Renderer::initText::Indices");
		
		// glBindVertexArray(0);
		// checkGLError("Renderer::initText::VAO");
	}

	void generateUVForChar(std::vector<Vec2>* uvs, char character)
	{
		float x = (character % 16);
		float y = (character / 16);
		x /= 16.f;
		y = ((int)y % 16) / 16.f;

		float offset = 1.f/16.f;
		uvs->push_back(Vec2(x           , y));
		uvs->push_back(Vec2(x           , y + offset));
		uvs->push_back(Vec2(x + 1.f/16.f, y + offset));
		uvs->push_back(Vec2(x + 1.f/16.f, y));
	}

	void addText(const char* text, Vec2 position)
	{
		
	}

	void cleanupText()
	{
		Texture::remove(texture);
		glDeleteVertexArrays(1, &textVAO);
		Shader::remove(textShader);
	}

	void addTextRect(Rect text)
	{
		Vec2 position = text.position;
		Vec2 scale    = text.scale;
			
		Mat4 translation = glm::translate(Mat4(1.f),Vec3(position.x, position.y, 0.f));
		Mat4 scaleMat    = glm::scale(Mat4(1.f), Vec3(scale.x, scale.y, 1.f));
			
		text.transMat = translation * scaleMat;
		textList.push_back(text);

		updateTextVBOs();
	}

	void renderText()
	{
		// TODO: Bitmap font rendering
		Shader::bindShader(textShader);
		Shader::setUniformVec4(textShader, "textColor", textColor);
		glBindVertexArray(textVAO);
		// glBindVertexArray(temp_vao);

		Texture::bindTexture((unsigned int)texture);
		
		for(uint32_t i = 0; i < textList.size(); i++)
		{
			Mat4 mvp = textProjMat * textList[i].transMat;
			Shader::setUniformMat4(textShader, "mvp", mvp);
			glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*) 0, (i * 4));
		}

		// Mat4 mvp = textProjMat * Mat4(1.f);
		// Shader::setUniformMat4(textShader, "mvp", mvp);
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		Texture::unbindActiveTexture();
		glBindVertexArray(0);
		Shader::unbindActiveShader();
	}

	void checkGLError(const char* context)
	{
		GLenum error = glGetError();

		std::string errorString = "No Error";

		switch(error)
		{
		case GL_INVALID_OPERATION:
			errorString = "Invalid Operation";
			break;
		case GL_NO_ERROR:
			errorString = "No Error";
			break;
		case GL_INVALID_ENUM:
			errorString = "Invalid ENUM";
			break;
		case GL_INVALID_VALUE:
			errorString = "Invalid Value";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errorString = "Invalid FrameBuffer Operation";
			break;
		case GL_OUT_OF_MEMORY:
			errorString = "Out of Memory";
			break;
		case GL_STACK_UNDERFLOW:
			errorString = "Stack Underflow";
			break;
		case GL_STACK_OVERFLOW:
			errorString = "Stack Overflow";
			break;
		}

		if(error != GL_NO_ERROR)
			Log::error(context, errorString);
	}

	void setClearColor(const Vec4 newClearColor)
	{
		clearColor = newClearColor;
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}

	void initialize(const char* path)
	{
		contentDir = (char*)malloc(sizeof(char) * (strlen(path) + strlen(contentDirName)) + 1);
		PA_ASSERT(contentDir != NULL);
		
        strcpy(contentDir, path);
		strcat(contentDir, contentDirName);

		setClearColor(Vec4(0.55, 0.6, 0.8, 1.0));
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// glPolygonMode(GL_FRONT, GL_LINE);

        char* texturePath = (char *)malloc(sizeof(char) *
										   (strlen(contentDir) + strlen(texDir)) + 1);
        strcpy(texturePath, contentDir);
		strcat(texturePath, texDir);
		
		char* shaderPath = (char *)malloc(sizeof(char) *
										   (strlen(contentDir) + strlen(shaderDir)) + 1);
        strcpy(shaderPath, contentDir);
		strcat(shaderPath, shaderDir);

		char* geoPath = (char *)malloc(sizeof(char) *
										   (strlen(contentDir) + strlen(modelDir)) + 1);
        strcpy(geoPath, contentDir);
		strcat(geoPath, modelDir);

		Texture::initialize(texturePath);
		Shader::initialize(shaderPath);
		Geometry::initialize(geoPath);
		Material::initialize();
		Model::initialize();

		initText();	
		free(texturePath);
		free(shaderPath);
		free(geoPath);
	}

	void cleanup()
	{
		free(contentDir);
		cleanupText();
		Model::cleanup();
		Framebuffer::cleanup();
		Texture::cleanup();
		Shader::cleanup();
		Material::cleanup();
		Geometry::cleanup();
		Camera::cleanup();
	}

	void setDebugLevel(DebugLevel level)
	{
		sDebugLevel = level;

		if(level == DebugLevel::NONE)
			Log::message("Renderer debug level set to NONE");
		if(level == DebugLevel::MEDIUM)
			Log::message("Renderer debug level set to MEDIUM");
		if(level == DebugLevel::HIGH)
			Log::message("Renderer debug level set to HIGH");
	}

	void toggleDebugView()
	{
		if(sRenderDebugView)
			sRenderDebugView = false;
		else
			sRenderDebugView = true;
	}

	void toggleWireframe()
	{
		if(sRenderWireframe)
			sRenderWireframe = false;
		else
			sRenderWireframe = true;
	}

	void renderFrame()
	{
		CCamera* camera = Camera::getActiveCamera();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if(camera)
			Model::renderAllModels(camera, &renderParams);
		// else
		// 	Log::error("Renderer::renderFrame", "Can't render, no active camera set!");
	}

	Vec4 getClearColor()
	{
		return clearColor;
	}
	
	RenderParams* getRenderParams()
	{
		return &renderParams;
	}
    
	void addText(const std::string& text)
    {
		sTextList.push_back(text);
    }
}
