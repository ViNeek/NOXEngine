#include "RayMarcher.h"

#include "Engine.h"
#include "Renderer.h"

#include "Voxelizer.h"
#include "DistanceField.h"

void nxRayMarcher::Init() {
	//nxInt32 l_BufferSize = m_VDimX * m_VDimY * 6 * sizeof(glm::vec4);
	nxInt32 l_BufferSize = m_VDimX * m_VDimY * 6 * sizeof(glm::vec4);
	glm::uvec3 l_Dims = m_Voxelizer->Dimesions();

	glGenBuffers(1, m_Buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Buffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, l_Dims.x * l_Dims.y * l_Dims.z * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glBufferData(GL_SHADER_STORAGE_BUFFER, l_BufferSize, NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_Buffer);

}

void nxRayMarcher::Bind() {

}

bool nxRayMarcherInitializer::operator()(void* data) {
	nxRayMarcherInitializerBlob* blob = (nxRayMarcherInitializerBlob*)data;
	nxVoxelizer* l_Voxel = blob->m_Engine->Renderer()->Voxelizer();

	std::cout << "\n\n\n\n\n Initializing Ray Marcher \n\n\n\n\n\n";
	glm::uvec3 l_Dims = l_Voxel->Dimesions();
	//glm::uvec3 l_GridSize = l_Voxel->Grid();

	blob->m_Engine->Renderer()->RayMarcher()->Init();

	return true;
}