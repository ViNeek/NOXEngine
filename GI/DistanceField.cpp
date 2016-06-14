#include <iostream>
#include <utility>

#include "DistanceField.h"
#include "Engine.h"
#include "Renderer.h"
#include "JobFactory.h"

nxDistanceField::nxDistanceField (nxUInt32 truncation) {
	m_Truncation = truncation;
	m_DistanceFieldBackBuffer = -1;
	m_DistanceFieldFrontBuffer = -1;
}

nxDistanceField::nxDistanceField () {
	m_Truncation = 4;
	m_DistanceFieldBackBuffer = -1;
	m_DistanceFieldFrontBuffer = -1;
}

static const int g_WorkGroupSize = 8;
static const glm::uvec3 g_WorkGroupSizeVec(8, 8, 8);
void nxDistanceField::Calculate(nxStorageBufferObject input) {
	int l_InvocationCount = m_DimX * m_DimY * m_DimZ;
	glm::uvec3 l_GroupSize(
        (m_DimX - 1) / g_WorkGroupSizeVec.x + 1,
        (m_DimY - 1) / g_WorkGroupSizeVec.y + 1,
        (m_DimZ - 1) / g_WorkGroupSizeVec.z + 1
	);
	//glm::uvec3 l_OldGroupSize(
	//	m_resolution / g_WorkGroupSize,
	//	m_resolution / g_WorkGroupSize,
	//	m_resolution / g_WorkGroupSize
	//);

	std::cout << "\n\n\n\n\nDims : " << l_GroupSize.x << "\n\n\n\n";
	std::cout << "\n\n\n\n\nDims : " << m_DimX << "\n\n\n\n";

	glDispatchComputeGroupSizeARB(l_GroupSize.x, l_GroupSize.y, l_GroupSize.z, g_WorkGroupSize, g_WorkGroupSize, g_WorkGroupSize);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//int l_GroupSize = g_WorkGroupSize * g_WorkGroupSize * g_WorkGroupSize;
	//int l_GroupCount = (l_InvocationCount - 1) / (l_GroupSize.x * l_GroupSize.y * l_GroupSize.z) + 1;

}

void nxDistanceField::Calculate(GLuint input) {
	int l_InvocationCount = m_DimX * m_DimY * m_DimZ;
	glm::uvec3 l_GroupSize(
		(m_DimX - 1) / g_WorkGroupSize + 1,
		(m_DimY - 1) / g_WorkGroupSize + 1,
		(m_DimZ - 1) / g_WorkGroupSize + 1
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

	glDispatchComputeGroupSizeARB(l_GroupSize.x, l_GroupSize.y, l_GroupSize.z, g_WorkGroupSize, g_WorkGroupSize, g_WorkGroupSize);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//int l_GroupSize = g_WorkGroupSize * g_WorkGroupSize * g_WorkGroupSize;
	//int l_GroupCount = (l_InvocationCount - 1) / (l_GroupSize.x * l_GroupSize.y * l_GroupSize.z) + 1;

}

void nxDistanceField::SwapBuffers() {

	std::swap( m_DistanceFieldBackBuffer, m_DistanceFieldFrontBuffer );

}

void nxDistanceField::Init(nxUInt32 dimX, nxUInt32 dimY, nxUInt32 dimZ) {
	m_DimX = dimX;
	m_DimY = dimY;
	m_DimZ = dimZ;

	glGenBuffers(1, m_DistanceFieldBackBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DistanceFieldBackBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_DimX * m_DimY * m_DimZ * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, m_DistanceFieldFrontBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DistanceFieldFrontBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_DimX * m_DimY * m_DimZ * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_DistanceFieldBackBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_DistanceFieldFrontBuffer);

	Utils::GL::CheckGLState("Distance Field");
	// TODO: REMOVE (vao should be set externally)

}

bool nxDistanceFieldInitializer::operator()(void* data) {
	nxDistanceFieldInitializerBlob* blob = (nxDistanceFieldInitializerBlob*)data;
	
	//BOOST_LOG_TRIVIAL(info) << "Initializing Distance Field ";
	std::cout << "Initializing Distance Field \n";

	blob->m_Engine->Renderer()->DistanceField()->Init(blob->m_Dimensions.x, blob->m_Dimensions.y, blob->m_Dimensions.z);

	return true;
}