#include "DistanceField.h"

nxDistanceField::nxDistanceField (nxUInt32 truncation) {
	m_Truncation = truncation;
	m_DistanceFieldBuffer = -1;
}

nxDistanceField::nxDistanceField () {
	m_Truncation = 4;
	m_DistanceFieldBuffer = -1;
}

static const int g_WorkGroupSize = 4;
void nxDistanceField::Calculate(nxStorageBufferObject input) {
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
	//BOOST_LOG_TRIVIAL(info) << "\n\n\n\n\nDims : " << l_OldGroupSize.x << "\n\n\n\n";

	glDispatchComputeGroupSizeARB(l_GroupSize.x, l_GroupSize.y, l_GroupSize.z, g_WorkGroupSize, g_WorkGroupSize, g_WorkGroupSize);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//int l_GroupSize = g_WorkGroupSize * g_WorkGroupSize * g_WorkGroupSize;
	//int l_GroupCount = (l_InvocationCount - 1) / (l_GroupSize.x * l_GroupSize.y * l_GroupSize.z) + 1;

}

void nxDistanceField::Init(nxUInt32 dimX, nxUInt32 dimY, nxUInt32 dimZ) {
	m_DimX = dimX;
	m_DimY = dimY;
	m_DimZ = dimZ;

	glGenBuffers(1, m_DistanceFieldBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_DistanceFieldBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_DimX * m_DimY * m_DimZ * 4, NULL, GL_DYNAMIC_DRAW);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, m_resolution * m_resolution * m_resolution / 8, NULL, GL_DYNAMIC_COPY);

	// TODO: REMOVE (vao should be set externally)

}