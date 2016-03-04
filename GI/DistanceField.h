#pragma once

#include "GLObject.h"
#include "GLUtils.h"
#include "CustomTypes.h"
#include "Constants.h"

class nxDistanceField {

public:

	nxDistanceField(nxUInt32 truncation);
	nxDistanceField();

	void Init(nxUInt32 dimX, nxUInt32 dimY, nxUInt32 dimZ);
	void Calculate(nxStorageBufferObject input);
	void Calculate(GLuint input);
	nxStorageBufferObject DistanceFieldBuffer() { return m_DistanceFieldBuffer; };

	bool nxDistanceField::operator()(void* data);

private:

	nxUInt32					m_Truncation;
	nxUInt32					m_DimX, m_DimY, m_DimZ;
	nxStorageBufferObject		m_DistanceFieldBuffer;

};