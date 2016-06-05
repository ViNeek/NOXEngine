#include "Entity.h"

#include "JobFactory.h"
#include "Engine.h"
#include "Renderer.h"
#include "Scene.h"
#include "Voxelizer.h"

#include "GLUtils.h"

#include "CustomTypes.h"

#include <iostream>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h> 

#include <boost/log/trivial.hpp>
#include <boost/multi_array.hpp>
#include <boost/log/trivial.hpp>

#include "Texture.h"

nxEntity::nxEntity() {
	m_IBO = -1;
	m_VBO = -1;
	m_VAO = -1;
	m_MaterialIndex = -1;
	m_SceneIndex = -1;
	m_NumMeshes = -1;

	m_MaxX = m_MaxY = m_MaxZ = -300000.0f;
	m_MinX = m_MinY = m_MinZ = +300000.0f;

	m_DataCurrentSize = 0;
	m_VertexDataSize = 0;

	m_AdHocTextureObject = -1;

}

nxEntity::nxEntity(const std::string filename) {
	m_IBO = -1;
	m_VBO = -1;
	m_VAO = -1;
	m_MaterialIndex = -1;
	m_SceneIndex = -1;
	m_NumMeshes = -1;

	m_MaxX = m_MaxY = m_MaxZ = -100000.0f;
	m_MinX = m_MinY = m_MinZ = 100000.0f;

	m_DataCurrentSize = 0;
	m_VertexDataSize = 0;

	m_AdHocTextureObject = -1;

	std::string model_name;
	std::string::size_type posEnd = filename.find('.');
	std::string::size_type posStart = filename.find_last_of('/');
	if ( (posStart != std::string::npos) && (posEnd != std::string::npos))
	{
		m_ModelName = filename.substr(posStart, posEnd);
	}
	else
	{
		m_ModelName = filename;
	}

	InitFromFile(filename);
}

void nxEntity::InitPreviewFromBuffer(glm::vec3* buffer, nxInt32 size, GLuint adHocTexture) {
    glGenBuffers(1, &m_VBO);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);

    for (nxSizeType i = 0; i < size; i++) {
        if (buffer[i].x < m_MinX) m_MinX = buffer[i].x;
        if (buffer[i].y < m_MinY) m_MinY = buffer[i].y;
        if (buffer[i].z < m_MinZ) m_MinZ = buffer[i].z;
        if (buffer[i].x > m_MaxX) m_MaxX = buffer[i].x;
        if (buffer[i].y > m_MaxY) m_MaxY = buffer[i].y;
        if (buffer[i].z > m_MaxZ) m_MaxZ = buffer[i].z;
    }

    const auto l_VertexSize = sizeof(aiVector3D) + sizeof(aiVector2D);

    m_NumMeshes = 1;
    m_MeshStartIndices.push_back(0);
    m_MeshSizes.push_back(size / 5);
    m_AdHocTextureObject = adHocTexture;

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, l_VertexSize, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, l_VertexSize, (void*)sizeof(aiVector3D));
    // Normal vectors
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D), (void*)(sizeof(aiVector3D) + sizeof(aiVector2D)));

    //Utils::GL::CheckGLState("VAOs");
}

void nxEntity::UpdateFromVoxelizer(nxVoxelizer* voxel) {

}

void nxEntity::InitFromVoxelizer(nxVoxelizer* voxelizer) {
    glm::vec3 size = voxelizer->GridSize();
    glm::vec3 l_GridMin = voxelizer->GridMin();
    glm::vec3 half_size = size * 0.5f;
    glm::vec3 voxel = size / glm::vec3(voxelizer->Dimesions());
    auto voxelCoords = new std::vector<glm::vec3>();

    //printf("Grid Size %f %f %f\n", size.x, size.y, size.z);
    //printf("Voxel Size %f %f %f\n", voxel.x, voxel.y, voxel.z);
    //printf("Dims %d %d %d\n", m_dimensions.x, m_dimensions.y, m_dimensions.z);

    const auto l_VertexSize = sizeof(aiVector3D);

    m_NumMeshes = 1;
    m_MeshStartIndices.push_back(0);
    m_AdHocTextureObject = -1;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelizer->VoxelBuffer());

    nxUInt32* p = (nxUInt32*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    //if (error) Utils::GL::CheckGLState("Frame");
    int countVoxels = 0;
    if (p) {
        typedef boost::multi_array_ref<nxUInt32, 3> array_type;
        typedef array_type::index index;
        array_type ip(p, boost::extents[voxelizer->Dimesions().x][voxelizer->Dimesions().y][voxelizer->Dimesions().z]);

        for (int i = 0; i < voxelizer->Dimesions().x; i++) {
            for (int j = 0; j < voxelizer->Dimesions().y; j++) {
                for (int k = 0; k < voxelizer->Dimesions().z; k++) {
                    if (ip[i][j][k] == 1) {
                        voxelCoords->push_back(
                            glm::vec3(i, j, k) * voxel + voxel * glm::vec3(0.5) + l_GridMin
                        );
                        countVoxels++;
                    }
                }
            }
        }

        printf("Voxel Count %d\n", countVoxels);

    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    m_MeshSizes.push_back(countVoxels);

    glGenBuffers(1, &m_VBO);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, countVoxels * sizeof(glm::vec3), voxelCoords->data(), GL_DYNAMIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, l_VertexSize, 0);

    Utils::GL::CheckGLState("VAOs");
}

void nxEntity::InitFromBuffer(glm::vec3* buffer, nxInt32 size, GLuint adHocTexture) {
	glGenBuffers(1, &m_VBO);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, size * 3 * 4, &buffer[0], GL_STATIC_DRAW);

	for (nxSizeType i = 0; i < size; i++) {
		if (buffer[i].x < m_MinX) m_MinX = buffer[i].x;
		if (buffer[i].y < m_MinY) m_MinY = buffer[i].y;
		if (buffer[i].z < m_MinZ) m_MinZ = buffer[i].z;
		if (buffer[i].x > m_MaxX) m_MaxX = buffer[i].x;
		if (buffer[i].y > m_MaxY) m_MaxY = buffer[i].y;
		if (buffer[i].z > m_MaxZ) m_MaxZ = buffer[i].z;
	}
	
    const auto l_VertexSize = sizeof(aiVector3D);

	m_NumMeshes = 1;
	m_MeshStartIndices.push_back(0);
	m_MeshSizes.push_back(size);
	m_AdHocTextureObject = adHocTexture;

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, l_VertexSize, 0);
	// Texture coordinates
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, l_VertexSize, (void*)sizeof(aiVector3D));
	// Normal vectors
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D) + sizeof(aiVector2D), (void*)(sizeof(aiVector3D) + sizeof(aiVector2D)));

	Utils::GL::CheckGLState("VAOs");
}

void nxEntity::InitFromFile(const std::string& path) {
	Assimp::Importer importer;
    std::cout << "Loading " << path << std::endl;

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
	const int vertex_size = sizeof(aiVector3D) * 4 + sizeof(aiVector2D);

    m_NumMeshes = scene->mNumMeshes;

    for (auto i = 0; i < m_NumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        //std::cout << "Num of Faces " << mesh->mNumFaces << std::endl;

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
                aiVector3D l_Tangent = mesh->mTangents[face.mIndices[k]];
                aiVector3D l_Bitangent = mesh->mBitangents[face.mIndices[k]];

                m_EntityData.insert(m_EntityData.end(), (nxByte*)&pos, (nxByte*)&pos + sizeof(aiVector3D));
                m_DataCurrentSize += sizeof(aiVector3D);
                m_EntityData.insert(m_EntityData.end(), (nxByte*)&uv, (nxByte*)&uv + sizeof(aiVector2D));
                m_DataCurrentSize += sizeof(aiVector2D);
                m_EntityData.insert(m_EntityData.end(), (nxByte*)&normal, (nxByte*)&normal + sizeof(aiVector3D));
                m_DataCurrentSize += sizeof(aiVector3D);
                m_EntityData.insert(m_EntityData.end(), (nxByte*)&l_Tangent, (nxByte*)&l_Tangent + sizeof(aiVector3D));
                m_DataCurrentSize += sizeof(aiVector3D);
                m_EntityData.insert(m_EntityData.end(), (nxByte*)&l_Bitangent, (nxByte*)&l_Bitangent + sizeof(aiVector3D));
                m_DataCurrentSize += sizeof(aiVector3D);
            }
        }

        m_MeshSizes.push_back((m_DataCurrentSize - before_size) / vertex_size);
    }
    std::cout << "Entity Data Length " << m_EntityData.size() << std::endl;

	std::cout << "Num of Meshes " << scene->mNumMeshes << std::endl;

    auto l_NumOfMaterials = scene->mNumMaterials;
    std::vector<int> l_MaterialsRemap(l_NumOfMaterials);

    for (auto i = 0; i < l_NumOfMaterials; i++) {
		auto l_TextureIndex = 0;
        auto l_DiffTexturePath = aiString();  // filename
        auto l_NormalTexturePath = aiString();  // filename
        auto l_Material = scene->mMaterials[i];

        aiReturn l_TexFound;

        l_TexFound = l_Material->GetTexture(aiTextureType_DIFFUSE, l_TextureIndex, &l_DiffTexturePath);

        //std::cout << "Image path : " << l_DiffTexturePath.data << std::endl;
        auto&& l_CacheRef = Cache();
        if (l_TexFound == AI_SUCCESS) {
			//fill map with textures, OpenGL image ids set to 0
			//textureIdMap[path.data] = 0;
			// more textures?

            std::string l_PathEnd = Utils::GetParentDirectory(l_DiffTexturePath.data);
            std::string l_PathStart = Utils::GetParentDirectory(path);
            std::string l_Filename = Utils::GetFilename(l_DiffTexturePath.data);
            std::string l_FullRelativePath = l_PathStart + "/" + l_PathEnd + "/" + l_Filename;

            std::cout << "Full path: " << l_FullRelativePath << '\n';
            
            auto l_TextureIterator = std::find_if(l_CacheRef.begin(), l_CacheRef.end(), [&l_FullRelativePath](nxTexture* t) { return t->Path() == l_FullRelativePath; });
            //auto l_TextureIndex = std::distance(l_CacheRef.begin(), l_TextureIterator);

            if (l_TextureIterator == l_CacheRef.end()) {
                l_CacheRef.push_back(new nxTexture(l_FullRelativePath, NOX_DIFFUSE_MAP));
                auto l_LastEntryIndex = l_CacheRef.size() - 1;
                //auto& l_LastEntry = nxTexture::CachedAt(l_LastEntryIndex);
                //l_LastEntry.Load();
                l_MaterialsRemap[i] = l_LastEntryIndex;
            }
            else {
                auto l_CachedTextureIndex = std::distance(l_CacheRef.begin(), l_TextureIterator) - 1;
                l_MaterialsRemap[i] = l_CachedTextureIndex;
            }

            std::cout << "Index : " << l_TextureIndex << std::endl;

			//l_TextureIndex++;
            //l_TexFound = scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, l_TextureIndex, &l_DiffTexturePath);
		}

        /*
        l_TextureIndex = 0;
        l_TexFound = l_Material->GetTexture(aiTextureType_HEIGHT, l_TextureIndex, &l_NormalTexturePath);
        std::cout << "Bump Image path : " << l_NormalTexturePath.data << std::endl;
        while (l_TexFound == AI_SUCCESS) {
            //fill map with textures, OpenGL image ids set to 0
            //textureIdMap[path.data] = 0;
            // more textures?
            l_TextureIndex++;
            l_TexFound = scene->mMaterials[i]->GetTexture(aiTextureType_HEIGHT, l_TextureIndex, &l_NormalTexturePath);
        }
        */
	}

    for (nxInt32 i = 0; i < m_MeshSizes.size(); i++ )
    {
        nxInt32 l_OldIndex = m_MaterialIndices[i];
        std::cout << "Old : " << l_OldIndex << std::endl;
        std::cout << "I : " << i << std::endl;
        if (l_MaterialsRemap[l_OldIndex] < 0)
            m_MaterialIndices[i] = 0;
        else
            m_MaterialIndices[i] = l_MaterialsRemap[l_OldIndex];
    }

	//m_NumMaterials = scene->mNumMaterials;
}

void nxEntity::UploadData() {
    const auto l_VertexSize = sizeof(aiVector3D) * 4 + sizeof(aiVector2D);

    glGenBuffers(1, &m_VBO);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_EntityData.size(), &m_EntityData[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, l_VertexSize, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, l_VertexSize, (void*)sizeof(aiVector3D));
	// Normal vectors
	glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, l_VertexSize, (void*)(sizeof(aiVector3D) + sizeof(aiVector2D)));
    // Tangent vectors
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, l_VertexSize, (void*)(2 * sizeof(aiVector3D) + sizeof(aiVector2D)));
    // Bitangent vectors
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, l_VertexSize, (void*)(3 * sizeof(aiVector3D) + sizeof(aiVector2D)));


	Utils::GL::CheckGLState("VAOs");

}

void nxEntity::Draw() {
	BindVAO();
    m_NumMeshes = m_MeshSizes.size();
    for (nxInt32 i = 0; i < m_NumMeshes; i++ )
	{
		if (m_MaterialIndices.size() > i) {
			nxInt32 l_MatIndex = m_MaterialIndices[i];
            //if (l_MatIndex > 0 )
            m_TextureCache[l_MatIndex]->Bind();
		}
		else if (m_AdHocTextureObject >= 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_AdHocTextureObject);
			//printf("Texture with id : %d", m_AdHocTextureObject);
		}
		glDrawArrays(GL_TRIANGLES, m_MeshStartIndices[i], m_MeshSizes[i]);
	}
}

void nxEntity::LineDraw() {
	BindVAO();
	for (size_t i = 0; i < m_NumMeshes; i++)
	{
		glDrawArrays(GL_LINES, m_MeshStartIndices[i], m_MeshSizes[i]);
	}
}

void nxEntity::PointDraw() {
    BindVAO();
    for (size_t i = 0; i < m_NumMeshes; i++)
    {
        glDrawArrays(GL_POINTS, m_MeshStartIndices[i], m_MeshSizes[i]);
    }
}

void nxEntity::Scale(nxFloat32 factor) {
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	nxInt32 l_Size = m_EntityData.size();
	nxInt32 l_Skip = 4 * sizeof(aiVector3D) + sizeof(aiVector2D);

	nxFloat32* p = (nxFloat32*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	
	for (nxInt32 i = 0; i < l_Size; i += l_Skip) {
		nxByte* l_BPtr = (nxByte*)p + i;
		nxFloat32* l_FPtr = (nxFloat32*)l_BPtr;

		l_FPtr[0] /= factor;
		l_FPtr[1] /= factor;
		l_FPtr[2] /= factor;

		m_MaxX /= factor;
		m_MaxY /= factor;
		m_MaxZ /= factor;
		m_MinX /= factor;
		m_MinY /= factor;
		m_MinZ /= factor;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

bool nxAssetLoader::operator()(void* data) {
	nxAssetLoaderBlob* blob = (nxAssetLoaderBlob*)data;
	
	//std::cout << "Asset Loading " << blob->m_ResourcePath << std::endl;
	//std::cout << "Asset Loading " << blob->m_ResourceType << std::endl;
	//std::cout << "Asset Loading " << blob->m_Center.z << std::endl;

	nxEntity* ent = new nxEntity(blob->m_ResourcePath + blob->m_ResourceType);

    for (auto l_Texture : ent->Cache()) {
        nxGLTextureLoaderBlob* textureBlob = new nxGLTextureLoaderBlob(blob->m_Engine, l_Texture);
        blob->m_Engine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_LOAD_TEXTURE, textureBlob));
    }

	glm::vec3 center;
	center.x = ((ent->MaxX() + ent->MinX())) / 2.0f;
	center.y = ((ent->MaxY() + ent->MinY())) / 2.0f;
	center.z = ((ent->MaxZ() + ent->MinZ())) / 2.0f;
	    
	ent->SetModelTransform(blob->m_Center);
	//ent->SetModelTransform(glm::vec3());
	ent->SetScale(blob->m_ScaleFactor);

	/*
    BOOST_LOG_TRIVIAL(info) << "Asset MaxX " << ent->MaxX();
	BOOST_LOG_TRIVIAL(info) << "Asset MaxY " << ent->MaxY();
	BOOST_LOG_TRIVIAL(info) << "Asset MaxZ " << ent->MaxZ();
	BOOST_LOG_TRIVIAL(info) << "Asset MaxX " << ent->MinX();
	BOOST_LOG_TRIVIAL(info) << "Asset MaxY " << ent->MinY();
	BOOST_LOG_TRIVIAL(info) << "Asset MaxZ " << ent->MinZ();
	BOOST_LOG_TRIVIAL(info) << "Asset CenterX " << center.x;
	BOOST_LOG_TRIVIAL(info) << "Asset CenterY " << center.y;
	BOOST_LOG_TRIVIAL(info) << "Asset CenterZ " << center.z;
    */

	nxGLAssetLoaderBlob* newData = new nxGLAssetLoaderBlob(blob->m_Engine, ent);
	blob->m_Engine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_LOAD_ASSET, newData));

	return true;
}

bool nxGLDebugAssetLoader::operator()(void* data) {
	nxGLBufferedAssetLoaderBlob* blob = (nxGLBufferedAssetLoaderBlob*)data;

	nxEntity* newEnt = new nxEntity();
	newEnt->InitFromBuffer(blob->m_Buffer, blob->m_BSize);

    //blob->m_Engine->Scene()->ClearDebugEntities();
    blob->m_Engine->Scene()->AddDebugEntity(newEnt);
    //blob->m_Engine->Scene()->AddEntity(newEnt);
	//blob->m_Engine->Scene()->UpdateBounds(newEnt);
	//blob->m_Engine->Renderer()->Voxelizer()->SetMatrices();

	return true;
}

bool nxGLBufferedAssetLoader::operator()(void* data) {
	nxGLBufferedAssetLoaderBlob* blob = (nxGLBufferedAssetLoaderBlob*)data;

	nxEntity* newEnt = new nxEntity();
	newEnt->InitFromBuffer(blob->m_Buffer, blob->m_BSize, blob->m_TextureObject);
	newEnt->Scale(22);

	blob->m_Engine->Scene()->AddEntity(newEnt);
	blob->m_Engine->Scene()->UpdateBounds(newEnt);
	blob->m_Engine->Renderer()->Voxelizer()->SetMatrices();

	return true;
}

bool nxGLAssetLoader::operator()(void* data) {
	nxGLAssetLoaderBlob* blob = (nxGLAssetLoaderBlob*)data;
	
	blob->m_Entity->UploadData();
	blob->m_Entity->Scale(blob->m_Entity->Scale());
	blob->m_Engine->Scene()->AddEntity(blob->m_Entity);
	blob->m_Engine->Scene()->UpdateBounds(blob->m_Entity);
	blob->m_Engine->Renderer()->Voxelizer()->SetMatrices();

	return true;
}

bool nxGLPreviewAssetLoader::operator()(void* data) {
	nxGLBufferedAssetLoaderBlob* blob = (nxGLBufferedAssetLoaderBlob*)data;

	nxEntity* newEnt = new nxEntity();
	newEnt->InitPreviewFromBuffer(blob->m_Buffer, blob->m_BSize, blob->m_TextureObject);

	blob->m_Engine->Scene()->AddPreviewEntity(newEnt);
	//blob->m_Engine->Scene()->UpdateBounds(newEnt);
	//blob->m_Engine->Renderer()->Voxelizer()->SetMatrices();

	return true;
}