#include "Entity.h"

#include "JobFactory.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h> 

nxEntity::nxEntity() {
	m_IBOData = -1;
	m_VBOData = -1;
	m_VAOData = -1;
	m_MaterialIndex = -1;
	m_SceneIndex = -1;
	m_NumMeshes = -1;

	m_MaxX = m_MaxY = m_MaxZ = -100000.0f;
	m_MinX = m_MinY = m_MinZ = 100000.0f;

	m_DataCurrentSize = 0;
	m_VertexDataSize = 0;

}

nxEntity::nxEntity(const std::string filename) {
	nxEntity();

	InitFromFile(filename);
}

void nxEntity::InitFromFile(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)
	{
		printf("Problem Loading\n");
	}

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	const int vertex_size = sizeof(aiVector3D) * 2 + sizeof(aiVector2D);

	m_NumMeshes = scene->mNumMeshes;

	for (size_t i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		m_NumVertices += mesh->mNumVertices;
		m_NumFaces += mesh->mNumFaces;
		int before_size = m_DataCurrentSize;
		m_MaterialIndices.push_back(mesh->mMaterialIndex);
		m_MeshStartIndices.push_back(before_size / vertex_size);
		
		for (size_t j = 0; j < mesh->mNumFaces; j++) {
			const aiFace& face = mesh->mFaces[j];
			for (int k = 0; k < 3; k++)
			{
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];

				if (pos.x < m_MinX) m_MinX = pos.x;
				if (pos.y < m_MinY) m_MinY = pos.y;
				if (pos.z < m_MinZ) m_MinZ = pos.z;
				if (pos.x > m_MaxX) m_MaxX = pos.x;
				if (pos.y > m_MaxY) m_MaxY = pos.y;
				if (pos.z > m_MaxZ) m_MaxZ = pos.z;

				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->mNormals[face.mIndices[k]];
				m_EntityData.insert(m_EntityData.end(), (unsigned char*)&pos, (unsigned char*)&pos + sizeof(aiVector3D));
				m_DataCurrentSize += sizeof(aiVector3D);
				m_EntityData.insert(m_EntityData.end(), (unsigned char*)&uv, (unsigned char*)&uv + sizeof(aiVector2D));
				m_DataCurrentSize += sizeof(aiVector2D);
				m_EntityData.insert(m_EntityData.end(), (unsigned char*)&normal, (unsigned char*)&normal + sizeof(aiVector3D));
				m_DataCurrentSize += sizeof(aiVector3D);
			}
		}
		m_MeshSizes.push_back((m_DataCurrentSize - before_size) / vertex_size);
	}

	/*
	centroid.x = (maxX + minX) / 2;
	centroid.y = (maxY + minY) / 2;
	centroid.z = (maxZ + minZ) / 2;
	*/

	//m_NumMaterials = scene->mNumMaterials;
}

bool nxAssetLoader::operator()(void* data) {

	return true;
}