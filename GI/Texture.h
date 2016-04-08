#pragma once

#include <vector>
#include <string>

#include "CustomTypes.h"
#include "GLObject.h"

enum nxTextureType {
    NOX_DIFFUSE_MAP,
    NOX_NORMAL_MAP,
    NOX_BUMP_MAP,
    NOX_FLUX_MAP,
    NOX_DEPTH_MAP,
    NOX_FRAMEBUFFER_TARGET
};

class nxTexture
{

public:
    nxTexture(const std::string& path, nxTextureType type) : m_Path(path), m_Type(type), m_Loaded(false) {};

    void Bind();
    void Load();
    void Unload();
    nxBool Check();

    std::string& Path() { return m_Path; }

private:

    std::vector<nxByte>     m_Raw;
    std::string             m_Path;
    nxTextureType           m_Type;
    nxInt32                 m_Width;
    nxInt32                 m_Height;
    nxTextureObject         m_TextureHandle;
    nxBool                  m_Loaded;

};