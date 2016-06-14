#include "RayMarcher.h"

#include "Engine.h"
#include "Renderer.h"

#include "Voxelizer.h"
#include "DistanceField.h"

void nxRayMarcher::Init() {
	//nxInt32 l_BufferSize = m_VDimX * m_VDimY * 6 * sizeof(glm::vec4);
	nxInt32 l_BufferSize = m_VDimX * m_VDimY * 6 * sizeof(glm::vec4);
    //glm::uvec3 l_Dims = m_Voxelizer->Dimesions();
    glm::uvec3 l_Dims = m_Voxelizer->DimesionsCubes();

    m_Buffer = -1;

	/*glGenBuffers(1, m_Buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Buffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, l_Dims.x * l_Dims.y * l_Dims.z * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_SHADER_STORAGE_BUFFER, l_Dims.x * l_Dims.y * l_Dims.z * l_BufferSize, NULL, GL_DYNAMIC_DRAW);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, l_BufferSize, NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_Buffer);
    */

    auto l_IndexBufferSize = l_Dims.x * l_Dims.y * l_Dims.z * sizeof(int);
    glGenBuffers(1, m_IndexBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_IndexBuffer);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, l_Dims.x * l_Dims.y * l_Dims.z * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_SHADER_STORAGE_BUFFER, l_IndexBufferSize, NULL, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_IndexBuffer);
}

void nxRayMarcher::Reserve(int p_NewLength) {
    if (p_NewLength <= m_BufferCapacity )
        return;

    m_BufferCapacity = p_NewLength;

    GLsizeiptr l_BufferSize = m_VDimX * m_VDimY * 6 * sizeof(glm::vec4) * p_NewLength;
    std::cout << "Buffer Size : " << l_BufferSize << " " << p_NewLength;
    std::cout << "Buffer Size : " << l_BufferSize << " " << sizeof(glm::vec4);
    //printf("Buffer Size : %d %d", m_VDimX, sizeof(glm::vec4));

    if ( m_Buffer < 0 )
        glGenBuffers(1, m_Buffer);
    Utils::GL::CheckGLState("SSBO Create");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Buffer);
    Utils::GL::CheckGLState("SSBO Bind");

    glBufferData(GL_SHADER_STORAGE_BUFFER, l_BufferSize, NULL, GL_DYNAMIC_DRAW);
    Utils::GL::CheckGLState("SSBO Storage Alloc");


    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_Buffer);
}

void nxRayMarcher::Bind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_Buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_IndexBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_IndexBuffer);
}

static const int g_WorkGroupSize = 8;
static const glm::uvec3 g_WorkGroupSizeVec(8, 8, 8);
void nxRayMarcher::Calculate() {
	glm::uvec3 l_GroupSize(
        (m_Voxelizer->Dimesions().x - 1) / g_WorkGroupSizeVec.x + 1,
        (m_Voxelizer->Dimesions().y - 1) / g_WorkGroupSizeVec.y + 1,
        (m_Voxelizer->Dimesions().z - 1) / g_WorkGroupSizeVec.z + 1
	);
    glm::uvec3 l_GroupSizeCubes(
        (m_Voxelizer->DimesionsCubes().x - 1) / g_WorkGroupSizeVec.x + 1,
        (m_Voxelizer->DimesionsCubes().y - 1) / g_WorkGroupSizeVec.y + 1,
        (m_Voxelizer->DimesionsCubes().z - 1) / g_WorkGroupSizeVec.z + 1
        );
	//glm::uvec3 l_OldGroupSize(
	//	m_resolution / g_WorkGroupSize,
	//	m_resolution / g_WorkGroupSize,
	//	m_resolution / g_WorkGroupSize
	//);

	//BOOST_LOG_TRIVIAL(info) << "\n\n\n\n\nDims : " << l_GroupSize.x << "\n\n\n\n";
	//std::cout << "\n\n\n\n\nDims : " << "here" << "\n\n\n\n";
	//std::cout << l_GroupSize.x << "\n";
	//std::cout << "Group : " << l_GroupSize.x << ", " << l_GroupSize.y << ", " << l_GroupSize.z << std::endl;
	//std::cout << "Dims : " << m_DimX << ", " << m_DimY << ", " << m_DimZ << std::endl;

    //glDispatchComputeGroupSizeARB(l_GroupSize.x, l_GroupSize.y, l_GroupSize.z, g_WorkGroupSize, g_WorkGroupSize, g_WorkGroupSize);
    //glDispatchComputeGroupSizeARB(l_GroupSize.x, l_GroupSize.y, l_GroupSize.z, g_WorkGroupSize, g_WorkGroupSize, g_WorkGroupSize);
    glDispatchComputeGroupSizeARB(l_GroupSizeCubes.x, l_GroupSizeCubes.y, l_GroupSizeCubes.z, g_WorkGroupSize, g_WorkGroupSize, g_WorkGroupSize);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
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