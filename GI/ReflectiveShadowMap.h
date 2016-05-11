#pragma once

#include "GLObject.h"
#include "GLUtils.h"
#include "CustomTypes.h"
#include "Constants.h"

class nxReflectiveShadowMap {

public:

	nxReflectiveShadowMap(nxUInt32 size);
	nxReflectiveShadowMap(nxUInt32 sizeX, nxUInt32 sizeY);
	nxReflectiveShadowMap();

	void Init();
    void Bind();
    nxUInt32        Width() { return m_DimX; }
    nxUInt32        Height() { return m_DimY; }

    nxShadowMapObject ShadowMap() { return m_ShadowMap; }
	nxTextureObject FLuxMap() { return m_FluxMap; }
	nxTextureObject NormalMap() { return m_NormalMap; }

	bool nxReflectiveShadowMap::operator()(void* data);

private:

	nxUInt32					m_DimX, m_DimY;
	nxFrameBufferObject			m_FrameBuffer;
	nxShadowMapObject			m_ShadowMap;
	nxTextureObject				m_FluxMap;
	nxTextureObject				m_NormalMap;

};

typedef nxReflectiveShadowMap nxRSM;