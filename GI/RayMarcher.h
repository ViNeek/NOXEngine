#pragma once

#include "GLObject.h"
#include "GLUtils.h"
#include "CustomTypes.h"
#include "Constants.h"

class nxVoxelizer;
class nxDistanceField;

class nxRayMarcher {

public:

	nxRayMarcher() {}
	nxRayMarcher(nxVoxelizer* voxel, nxDistanceField* df, nxInt32 viewport_x, nxInt32 viewport_y) : m_Voxelizer(voxel), m_DistanceField(df), m_VDimX(viewport_x), m_VDimY(viewport_y) {}

	void Init();
	glm::ivec2 VPort() { return glm::ivec2(m_VDimX, m_VDimY); }
	void Bind();
	void Calculate();
	nxStorageBufferObject Buffer() { return m_Buffer; }

private:

	nxInt32						m_VDimX, m_VDimY;
	nxStorageBufferObject		m_Buffer;
	nxVoxelizer*				m_Voxelizer;
	nxDistanceField*			m_DistanceField;

};