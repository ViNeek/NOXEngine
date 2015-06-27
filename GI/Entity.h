#pragma once

#include <gl/glew.h>

#include <string>
#include <vector>

#include "CustomTypes.h"

class nxEntity {
public:

							nxEntity();
							nxEntity(const std::string filename);
	void					InitFromFile(const std::string& filename);
	void					ClearData();

	float					MaxX(){ return m_MaxX; }
	float					MaxY(){ return m_MaxY; }
	float					MaxZ(){ return m_MaxZ; }
	float					MinX(){ return m_MinX; }
	float					MinY(){ return m_MinY; }
	float					MinZ(){ return m_MinZ; }

private:

	std::vector<nxByte>		m_EntityData;

	GLuint					m_VBOData;
	GLuint					m_VAOData;
	GLuint					m_IBOData;
	unsigned int			m_Indices;
	unsigned int			m_MaterialIndex;

	unsigned int			m_SceneIndex;
	unsigned int			m_NumMeshes;

	unsigned int			m_NumFaces = 0;
	unsigned int			m_NumVertices = 0;
	unsigned int			m_NumNormals = 0;
	unsigned int			m_NumTexCoords = 0;

	float					m_MaxX, m_MaxY, m_MaxZ;
	float					m_MinX, m_MinY, m_MinZ;

	unsigned int			m_DataCurrentSize;
	unsigned int			m_VertexDataSize;

	std::vector<int>		m_MaterialIndices;
	std::vector<int>		m_MeshStartIndices;
	std::vector<int>		m_MeshSizes;

};