#include <GL/glew.h>
#include <GL/gl.h>

#include "geometry.h"
#include "log.h"
#include "passert.h"
#include "renderer.h"
#include "boundingvolumes.h"
#include "editor.h"

namespace Geometry
{
	struct GeometryData
	{
		std::string               filename    = "UNDEFINED";
		bool                      drawIndexed = false;
		std::vector<Vec3>         vertices;
		std::vector<Vec3>         vertexColors;
		std::vector<Vec3>         normals;
		std::vector<Vec2>         uvs;
		std::vector<unsigned int> indices;
		unsigned int              vao;
		unsigned int              refCount;
		BoundingBox               boundingBox;
		BoundingSphere            boundingSphere;
	};

	namespace
	{
		std::vector<GeometryData> geometryList;
		std::vector<uint32_t>     emptyIndices;
		char*                     geometryPath;
		int                       cullingMode = CM_BOX;
	}

	void setCullingMode(CullingMode mode)
	{
		cullingMode = (int)mode;
	}

	int find(const char* filename)
	{
		int index = -1;
		for(int i = 0; i < (int)geometryList.size(); i++)
		{
			if(strcmp(geometryList[i].filename.c_str(), filename) == 0)
			{
				index = i;
				break;
			}
		}
		return index;
	}

	void generateBoundingBox(int index)
	{
		BoundingBox*    boundingBox    = &geometryList[index].boundingBox;
		BoundingSphere* boundingSphere = &geometryList[index].boundingSphere;
		for(Vec3& vertex : geometryList[index].vertices)
		{
			if(vertex.x > boundingBox->max.x) boundingBox->max.x = vertex.x;
			if(vertex.y > boundingBox->max.y) boundingBox->max.y = vertex.y;
			if(vertex.z > boundingBox->max.z) boundingBox->max.z = vertex.z;

			if(vertex.x < boundingBox->min.x) boundingBox->min.x = vertex.x;
			if(vertex.y < boundingBox->min.y) boundingBox->min.y = vertex.y;
			if(vertex.z < boundingBox->min.z) boundingBox->min.z = vertex.z;
		}

		// boundingBox->min -= Vec3(10);
		// boundingBox->max += Vec3(10);
		// boundingBox->size   = boundingBox->max - boundingBox->min;
		// boundingBox->center = (boundingBox->max + boundingBox->min) / 2.f;

		// Vec3 halfSize = boundingBox->size / 2.f;
		// Vec3 center   = boundingBox->center;
		// Back
		// boundingBox->points[0] = Vec3(center.x - halfSize.x, center.y + halfSize.y, center.z - halfSize.z);
		// boundingBox->points[1] = Vec3(center.x + halfSize.x, center.y + halfSize.y, center.z - halfSize.z);
		// boundingBox->points[2] = Vec3(center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z);
		// boundingBox->points[3] = Vec3(center.x + halfSize.x, center.y - halfSize.y, center.z - halfSize.z);
		// // Front
		// boundingBox->points[4] = Vec3(center.x - halfSize.x, center.y + halfSize.y, center.z + halfSize.z);
		// boundingBox->points[5] = Vec3(center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z);
		// boundingBox->points[6] = Vec3(center.x - halfSize.x, center.y - halfSize.y, center.z + halfSize.z);
		// boundingBox->points[7] = Vec3(center.x + halfSize.x, center.y - halfSize.y, center.z + halfSize.z);

		boundingSphere->center = (boundingBox->max + boundingBox->min) / 2.f;
		boundingSphere->radius = glm::abs(glm::length(boundingBox->max - boundingSphere->center));
	}

	bool loadFromFile(GeometryData* geometry, const char* filename)
	{
		PA_ASSERT(filename);
		bool success = true;
		char* fullPath = (char *)malloc(sizeof(char) * (strlen(geometryPath) + strlen(filename)) + 1);
		strcpy(fullPath, geometryPath);
		strcat(fullPath, filename);
			
		FILE* file = fopen(fullPath, "rb");
		free(fullPath);
		if(file)
		{				
			const uint32_t INDEX_SIZE = sizeof(uint32_t);
			const uint32_t VEC3_SIZE  = sizeof(Vec3);
			const uint32_t VEC2_SIZE  = sizeof(Vec2);
			uint32_t header[4];
			size_t bytesRead = 0;
			if((bytesRead = fread(header, INDEX_SIZE, 4, file)) <= 0)
			{
				Log::error("Geometry::loadFromFile", "Read failed");
				success = false;
			}
			else
			{
				uint32_t indicesCount  = header[0];
				uint32_t verticesCount = header[1];
				uint32_t normalsCount  = header[2];
				uint32_t uvsCount      = header[3];
				// Indices
				geometry->indices.reserve(indicesCount);
				geometry->indices.insert(geometry->indices.begin(), indicesCount, 0);				
				fread(&geometry->indices[0], INDEX_SIZE, indicesCount, file);
				// Vertices
				geometry->vertices.reserve(verticesCount);
				geometry->vertices.insert(geometry->vertices.begin(), verticesCount, Vec3(0.f));
				fread(&geometry->vertices[0], VEC3_SIZE, verticesCount, file);
				// Normals
				geometry->normals.reserve(normalsCount);
				geometry->normals.insert(geometry->normals.begin(), normalsCount, Vec3(0.f));				
				fread(&geometry->normals[0], VEC3_SIZE, normalsCount, file);
				// UVs
				geometry->uvs.reserve(uvsCount);
				geometry->uvs.insert(geometry->uvs.begin(), uvsCount, Vec2(0.f));				
				fread(&geometry->uvs[0], VEC2_SIZE, uvsCount, file);					
			}
			fclose(file);
			geometry->filename    = filename;
			geometry->drawIndexed = true;
			geometry->refCount++;
		}
		else
		{
			success = false;
		}
		
		return success;
	}

	void createVAO(GeometryData* geometry)
	{
		// TODO : Add support for different model formats and interleaving VBO
		PA_ASSERT(geometry);

		glGenVertexArrays(1, &geometry->vao);
		glBindVertexArray(geometry->vao);

		GLuint vertexVBO;
		glGenBuffers(1, &vertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glBufferData(GL_ARRAY_BUFFER,
					 geometry->vertices.size() * sizeof(Vec3),
					 geometry->vertices.data(),
					 GL_STATIC_DRAW);
		Renderer::checkGLError("Geometry::createVBO::vertex");
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		if(geometry->normals.size() > 0)
		{
			GLuint normalVBO;
			glGenBuffers(1, &normalVBO);
			glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
			glBufferData(GL_ARRAY_BUFFER,
						 geometry->normals.size() * sizeof(Vec3),
						 geometry->normals.data(),
						 GL_STATIC_DRAW);
			Renderer::checkGLError("Geometry::createVBO::normal");
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0);
		}

		if(geometry->uvs.size() > 0)
		{
			GLuint uvVBO;
			glGenBuffers(1, &uvVBO);
			glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
			glBufferData(GL_ARRAY_BUFFER,
						 geometry->uvs.size() * sizeof(Vec2),
						 geometry->uvs.data(),
						 GL_STATIC_DRAW);
			Renderer::checkGLError("Geometry::createVBO::uv");
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if(geometry->vertexColors.size() > 0)
		{
			GLuint colorVBO;
			glGenBuffers(1, &colorVBO);
			glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
			glBufferData(GL_ARRAY_BUFFER,
						 geometry->vertexColors.size() * sizeof(Vec3),
						 geometry->vertexColors.data(),
						 GL_STATIC_DRAW);
			Renderer::checkGLError("Geometry::createVBO::color");
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if(geometry->indices.size() > 0)
		{
			GLuint ibo;
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
						 geometry->indices.size() * sizeof(GLuint),
						 geometry->indices.data(),
						 GL_STATIC_DRAW);
			geometry->drawIndexed = true;
		}
		glBindVertexArray(0);
	}
	
	int create(const char* filename)
	{
		// check if exists
		int index = find(filename);
		if(index == -1)
		{
			if(emptyIndices.size() == 0)
			{
				geometryList.push_back(GeometryData());
				index = geometryList.size() - 1;
			}
			else
			{
				index = emptyIndices.back();
				emptyIndices.pop_back();
			}
			GeometryData* newGeo = &geometryList[index];
			if(loadFromFile(newGeo, filename))
			{
				createVAO(newGeo);
				generateBoundingBox(index);
			}
			else
			{
				geometryList.pop_back();
				index = -1;
			}
		}
		else
		{
			geometryList[index].refCount++;
		}
		return index;
	}
	
	unsigned int getVAO(int index)
	{
		unsigned int vao = 0;
		if(index >= 0 && index < (int)geometryList.size()) vao = geometryList[index].vao;
		return vao;
	}
	
	void initialize(const char* path)
	{
		geometryPath = (char *)malloc(sizeof(char) * strlen(path) + 1);
		strcpy(geometryPath, path);
	}
	
	void cleanup()
	{
		free(geometryPath);
		geometryList.clear();
		emptyIndices.clear();
	}
	
	void remove(int index)
	{
		if(index >= 0 && index < (int)geometryList.size())
		{
			if(--geometryList[index].refCount == 0)
			{
				emptyIndices.push_back(index);
				glDeleteVertexArrays(1, &geometryList[index].vao);
				geometryList[index].vertices.clear();
				geometryList[index].indices.clear();
				geometryList[index].normals.clear();
				geometryList[index].uvs.clear();
				geometryList[index].vertexColors.clear();
			}
		}
	}

	bool render(int index, Frustum* frustum, CTransform* transform)
	{
		bool rendered = false;
		if(index >= 0 && index < (int)geometryList.size())
		{
			GeometryData* geometry = &geometryList[index];
			int intersection = IT_INSIDE;
			if(cullingMode == CM_BOX)
				 intersection = BoundingVolume::isIntersecting(frustum, &geometry->boundingBox, transform);
			else if(cullingMode == CM_SPHERE)
				intersection = BoundingVolume::isIntersecting(frustum, &geometry->boundingSphere, transform);
			if(intersection == IT_INTERSECT || intersection == IT_INSIDE)
			{
				glBindVertexArray(geometry->vao);
				if(geometry->drawIndexed)
					glDrawElements(GL_TRIANGLES, geometry->indices.size(), GL_UNSIGNED_INT, (void*)0);
				else
					glDrawArrays(GL_TRIANGLES, 0, geometry->vertices.size());
				glBindVertexArray(0);
				rendered = true;
			}
		}
		return rendered;
	}
	
	const std::string getName(int index)
	{
		std::string filename;
		if(index >= 0 && index < (int)geometryList.size())
			filename = geometryList[index].filename;
		return filename;
	}
	
	const std::vector<Vec3>* getVertices(int index)
	{
		const std::vector<Vec3>* vertices = NULL;
		if(index >= 0 && index < (int)geometryList.size())
			vertices = &geometryList[index].vertices;
		return vertices;
	}

	const std::vector<Vec3>* getNormals(int index)
	{
		const std::vector<Vec3>* normals = NULL;
		if(index >= 0 && index < (int)geometryList.size())
			normals = &geometryList[index].normals;
		return normals;
	}

	const std::vector<Vec3>* getVertexColors(int index)
	{
		const std::vector<Vec3>* vertexColors = NULL;
		if(index >= 0 && index < (int)geometryList.size())
			vertexColors = &geometryList[index].vertexColors;
		return vertexColors;
	}

	const std::vector<Vec2>* getUvs(int index)
	{
		const std::vector<Vec2>* uvs = NULL;
		if(index >= 0 && index < (int)geometryList.size())
			uvs = &geometryList[index].uvs;
		return uvs;
	}

	const std::vector<unsigned int>* getIndices(int index)
	{
		const std::vector<unsigned int>* indices = NULL;
		if(index >= 0 && index < (int)geometryList.size())
			indices = &geometryList[index].indices;
		return indices;
	}
}
