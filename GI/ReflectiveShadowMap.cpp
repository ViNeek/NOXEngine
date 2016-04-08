#include "ReflectiveShadowMap.h"
#include "Renderer.h"
#include "Engine.h"
#include "Texture.h"

static const nxUInt32 DEFAULT_TEXTURE_SIZE = 512;

nxReflectiveShadowMap::nxReflectiveShadowMap(nxUInt32 size) {
	m_DimX = m_DimY = size;
	m_ShadowMap = -1;
	m_NormalMap = -1;
	m_FluxMap = -1;
}

nxReflectiveShadowMap::nxReflectiveShadowMap(nxUInt32 sizeX, nxUInt32 sizeY) {
	m_DimX = sizeX;
	m_DimY = sizeY;
	m_ShadowMap = -1;
	m_NormalMap = -1;
	m_FluxMap = -1;
	m_FrameBuffer = -1;
}

nxReflectiveShadowMap::nxReflectiveShadowMap() {
	m_DimX = DEFAULT_TEXTURE_SIZE;
	m_DimY = DEFAULT_TEXTURE_SIZE;
	m_ShadowMap = -1;
	m_NormalMap = -1;
	m_FluxMap = -1;
	m_FrameBuffer = -1;
}

void nxReflectiveShadowMap::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
}

void nxReflectiveShadowMap::Init() {

    // Generate Framebuffer for RSM
	glGenFramebuffers(1, m_FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

    glActiveTexture(GL_TEXTURE0 + NOX_FRAMEBUFFER_TARGET);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, m_ShadowMap);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_DimX, m_DimY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glGenTextures(1, m_NormalMap);
	glBindTexture(GL_TEXTURE_2D, m_NormalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_DimX, m_DimX, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, m_FluxMap);
	glBindTexture(GL_TEXTURE_2D, m_FluxMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_DimX, m_DimX, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowMap, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_FluxMap, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, m_NormalMap, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0+1 };
    glDrawBuffers(2, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	std::cout << status << " status flag\n\n\n";
	if (status != GL_FRAMEBUFFER_COMPLETE)
		Utils::GL::CheckGLState("RSM buffers creation : ");
	else
		std::cout << "Buffer creation completed\n\n\n";

    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

}

bool nxRSMInitializer::operator()(void* data) {
	nxRSMInitializerBlob* blob = (nxRSMInitializerBlob*)data;

	//BOOST_LOG_TRIVIAL(info) << "Initializing Distance Field ";
	std::cout << "Initializing Reflective Shadow Maps \n";

	blob->m_Engine->Renderer()->RSM()->Init();

	return true;
}