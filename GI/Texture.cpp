#include <freeimage/FreeImage.h>

#include "Texture.h"

#include <wx/thread.h>

void nxTexture::Bind() {
    glActiveTexture(GL_TEXTURE0 + m_Type);
    //std::cout << "Binding " << m_Path;
    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
}

void nxTexture::Load() {
        auto l_FilePath = m_Path.c_str();
    FREE_IMAGE_FORMAT l_Format = FreeImage_GetFileType(l_FilePath, 0);

    FIBITMAP* l_Image = FreeImage_Load(l_Format, l_FilePath);
    if (l_Image) {
        std::cout << "Texture named : " << m_Path << " loaded!!!!" << std::endl;
    }

    FIBITMAP* l_TempImage = l_Image;
    l_Image = FreeImage_ConvertTo32Bits(l_Image);
    FreeImage_Unload(l_TempImage);

    
    nxInt32 l_Width = FreeImage_GetWidth(l_Image);
    nxInt32 l_Height = FreeImage_GetHeight(l_Image);

    std::cout << "The size of the image is: " << m_Path << " es " << l_Width << "*" << l_Height << std::endl; //Some debugging code

    nxByte* l_InvertedBufferData = new GLubyte[4 * l_Width * l_Height];
    nxByte* l_BufferData = FreeImage_GetBits(l_Image);

    //FreeImage loads in BGR format, so you need to swap some bytes(Or use GL_BGR).

    for (int j = 0; j<l_Width*l_Height; j++){
        l_InvertedBufferData[j * 4 + 0] = l_BufferData[j * 4 + 2];
        l_InvertedBufferData[j * 4 + 1] = l_BufferData[j * 4 + 1];
        l_InvertedBufferData[j * 4 + 2] = l_BufferData[j * 4 + 0];
        l_InvertedBufferData[j * 4 + 3] = l_BufferData[j * 4 + 3];
    }
    
    glActiveTexture(GL_TEXTURE0 + m_Type);

    glGenTextures(1, m_TextureHandle);
    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, l_Width, l_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)l_InvertedBufferData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    m_Loaded = true;
}

void nxTexture::Unload() {

}

nxBool nxTexture::Check() {
    return m_Loaded;
}

bool nxGLTextureLoader::operator()(void* data) {
    nxGLTextureLoaderBlob* blob = (nxGLTextureLoaderBlob*)data;

    //std::cout << "To load " << blob->m_Texture->Path() << std::endl;
    //std::cout << "Thread " << wxThread::GetCurrentId() << " worker released" << std::endl;

    blob->m_Texture->Load();

    return true;
}