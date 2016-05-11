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


	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader

    glActiveTexture(GL_TEXTURE0 + NOX_DEPTH_MAP);

    glGenTextures(1, m_ShadowMap);
	glBindTexture(GL_TEXTURE_2D, m_ShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_DimX, m_DimY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float l_BorderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    //float l_BorderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, l_BorderColor);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    glActiveTexture(GL_TEXTURE0 + NOX_FRAMEBUFFER_TARGET);

	glGenTextures(1, m_NormalMap);
    glBindTexture(GL_TEXTURE_2D, m_NormalMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_DimX, m_DimX, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

	auto buffer = new std::vector<nxByte>;
	auto t_TempVec = glm::vec3(0.5, 0.8, 0.01);
	auto t_TempCoordVec = glm::vec2(0, 1);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.9, 0.8, 0.01);
	t_TempCoordVec = glm::vec2(1, 1);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.9, 0.3, 0.01);
	t_TempCoordVec = glm::vec2(1, 0);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.9, 0.3, 0.01);
	t_TempCoordVec = glm::vec2(1, 0);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.5, 0.3, 0.01);
	t_TempCoordVec = glm::vec2(0, 0);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.5, 0.8, 0.01);
	t_TempCoordVec = glm::vec2(0, 1);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));

	nxGLBufferedAssetLoaderBlob* bufferData = new nxGLBufferedAssetLoaderBlob(blob->m_Engine, (glm::vec3*) buffer->data(), buffer->size(), blob->m_Engine->Renderer()->RSM()->FLuxMap());
	blob->m_Engine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_LOAD_PREVIEW_ASSET, bufferData));

	float l_OffsetY = 0.6;
	
	buffer = new std::vector<nxByte>;
	t_TempVec = glm::vec3(0.5, 0.8 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(0, 1);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.9, 0.8 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(1, 1);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.9, 0.3 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(1, 0);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.9, 0.3 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(1, 0);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.5, 0.3 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(0, 0);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.5, 0.8 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(0, 1);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));

	bufferData = new nxGLBufferedAssetLoaderBlob(blob->m_Engine, (glm::vec3*) buffer->data(), buffer->size(), blob->m_Engine->Renderer()->RSM()->NormalMap());
	blob->m_Engine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_LOAD_PREVIEW_ASSET, bufferData));

	l_OffsetY = 1.2;

	buffer = new std::vector<nxByte>;
	t_TempVec = glm::vec3(0.5, 0.8 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(0, 1);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.9, 0.8 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(1, 1);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.9, 0.3 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(1, 0);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.9, 0.3 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(1, 0);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.5, 0.3 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(0, 0);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));
	t_TempVec = glm::vec3(0.5, 0.8 - l_OffsetY, 0.01);
	t_TempCoordVec = glm::vec2(0, 1);
	buffer->insert(buffer->end(), (nxByte*)&t_TempVec, (nxByte*)&t_TempVec + sizeof(glm::vec3));
	buffer->insert(buffer->end(), (nxByte*)&t_TempCoordVec, (nxByte*)&t_TempCoordVec + sizeof(glm::vec2));

	bufferData = new nxGLBufferedAssetLoaderBlob(blob->m_Engine, (glm::vec3*) buffer->data(), buffer->size(), blob->m_Engine->Renderer()->RSM()->ShadowMap());
	blob->m_Engine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_LOAD_PREVIEW_ASSET, bufferData));

	return true;
}