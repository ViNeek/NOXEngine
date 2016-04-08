#pragma once

#include "Constants.h"

#include <GL/glew.h>
#include <string>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class nxJob;
class nxFrame;
class nxShader;
class nxProgram;
class nxEngine;
class nxEntity;
class nxScene;
class nxRenderer;
class nxScheduler;
class nxTexture;

// Request from global NOX Engine State
#define NOX_ENGINE_GET(Object) ((nxEngine*)data)->Object()

enum nxJobID {
	NX_JOB_DUMMY,
	NX_JOB_EXIT,
	NX_JOB_RESOURCE_LOOP,
	NX_JOB_RENDERER_EXIT,
	NX_JOB_SCHEDULER_EXIT,
	NX_JOB_WORKER_EXIT,
	NX_JOB_WORKER_FINISHED,
	NX_JOB_LOAD_SCENE,
	NX_JOB_LOAD_ASSET,
	NX_JOB_LOAD_SHADER,
	NX_GL_JOB_PROGRAM_SWAP,
	NX_GL_JOB_EXTENSION_INIT,
	NX_GL_JOB_FRAMEBUFFER_INIT,
	NX_GL_JOB_VOXELIZER_INIT,
	NX_GL_JOB_DISTANCE_FIELD_INIT,
	NX_GL_JOB_RSM_INIT,
	NX_GL_JOB_FRAMEBUFFER_RESIZE,
    NX_GL_JOB_COMPILE_SHADER,
    NX_GL_JOB_LOAD_TEXTURE,
    NX_GL_JOB_LOAD_ASSET,
	NX_GL_JOB_LOAD_BUFF_ASSET,
	NX_GL_JOB_LOAD_DEBUG_ASSET,
	NX_GL_JOB_LINK_PROGRAM,
	NX_JOB_MAX
};

class nxJobFactory {

public:

	static nxJob*	CreateJob(nxJobID id, void* data = 0, bool later = false, nxUInt64 after = NOXConstants::nxTimeNow);

	void			Init();

private:

};

struct nxDummyJob {
	bool operator()(void* data);
};

struct nxFramebufferResizerBlob {
	nxFramebufferResizerBlob(nxEngine* eng, nxRenderer* rend)
		: m_Engine(eng), m_Renderer(rend) {}

	nxEngine*		m_Engine;
	nxRenderer*		m_Renderer;
};

struct nxFramebufferResizer {
	bool operator()(void* data);
};

struct nxExtensionInitializerBlob {
	nxExtensionInitializerBlob(nxEngine* eng, nxRenderer* rend)
		: m_Engine(eng), m_Renderer(rend) {}

	nxEngine*		m_Engine;
	nxRenderer*		m_Renderer;
};

struct nxExtensionInitializer {
	bool operator()(void* data);
};

struct nxResourceLooper {
	bool operator()(void* data);
};

struct nxFramebufferInitializerBlob {
	nxFramebufferInitializerBlob(nxEngine* eng, nxRenderer* rend)
		: m_Engine(eng), m_Renderer(rend) {}

	nxEngine*		m_Engine;
	nxRenderer*		m_Renderer;

};

struct nxFramebufferInitializer {
	bool operator()(void* data);
};

struct nxShaderLoaderBlob {
	nxShaderLoaderBlob(nxEngine* eng, nxProgram* prog, std::string source, GLenum type, bool use = false)
		: m_Engine( eng ), m_Prog( prog ), m_Source( source ), m_Type( type ), m_Use ( use ) {}

	nxEngine*		m_Engine;
	nxProgram*		m_Prog;
	std::string		m_Source;
	GLenum			m_Type;
	bool			m_Use;
};

struct nxShaderLoader {
	bool operator()(void* data);
};


struct nxShaderCompilerBlob {
	nxShaderCompilerBlob(nxEngine* eng, nxProgram* prog, nxShader* shad, bool use = false )
		: m_Engine( eng ), m_Prog( prog ), m_Shader( shad ), m_Use(use) {}

	nxEngine*		m_Engine;
	nxProgram*		m_Prog;
	nxShader*		m_Shader;
	bool			m_Use;
};

struct nxShaderCompiler {
	bool operator()(void* data);
};

struct nxProgramLinkerBlob {
	nxProgramLinkerBlob(nxEngine* eng, nxProgram* prog, bool use = false)
		: m_Engine( eng ), m_Prog( prog ), m_Use( use ) {}

	nxEngine*		m_Engine;
	nxProgram*		m_Prog;
	bool			m_Use;

};

struct nxProgramLinker {
	bool operator()(void* data);
};

struct nxProgramSwapperBlob {
	nxProgramSwapperBlob(nxRenderer* rend, nxProgram* prog)
		: m_Renderer(rend), m_Prog(prog) {}

	nxRenderer*		m_Renderer;
	nxProgram*		m_Prog;

};

struct nxProgramSwapper {
	bool operator()(void* data);
};

struct nxSceneLoader {
	bool operator()(void* data);
};

/*

Asset loading

*/

struct nxGLAssetLoaderBlob {
	nxGLAssetLoaderBlob(nxEngine* eng, nxEntity* ent )
		: m_Engine( eng ), m_Entity( ent ) {}

	nxEngine*		m_Engine;
	nxEntity*		m_Entity;

};

struct nxGLBufferedAssetLoaderBlob {
	nxGLBufferedAssetLoaderBlob(nxEngine* eng, glm::vec3* buff, nxInt32 size)
		: m_Engine(eng), m_Buffer(buff), m_BSize(size) {}

	nxEngine*		m_Engine;
	glm::vec3*		m_Buffer;
	nxInt32			m_BSize;

};

struct nxGLTextureLoaderBlob {
    nxGLTextureLoaderBlob(nxEngine* eng, nxTexture* texture)
        : m_Engine(eng), m_Texture(texture) {}

    nxEngine*		m_Engine;
    nxTexture*		m_Texture;

};

struct nxGLTextureLoader {
    bool operator()(void* data);
};

struct nxGLAssetLoader {
	bool operator()(void* data);
};

struct nxGLBufferedAssetLoader {
	bool operator()(void* data);
};

struct nxGLDebugAssetLoader {
	bool operator()(void* data);
};

struct nxAssetLoaderBlob {
	nxAssetLoaderBlob(nxEngine* eng, std::string path, std::string type, glm::vec3 cent, float scale)
		: m_Engine( eng ), m_ResourcePath( path ), m_ResourceType( type ), m_Center( cent ), m_ScaleFactor( scale ) {}

	nxEngine*					m_Engine;
	std::string					m_ResourcePath;
	std::string					m_ResourceType;
	glm::vec3					m_Center;
	float						m_ScaleFactor;

};

struct nxAssetLoader  {
	bool operator()(void* data);
};

/*

Voxel Grid Generation

*/

struct nxVoxelizerInitializerBlob {
	nxVoxelizerInitializerBlob(nxEngine* eng, glm::uvec3 dims)
		: m_Engine(eng), m_Dimensions(dims) {}

	nxEngine*		m_Engine;
	glm::uvec3		m_Dimensions;
};

struct nxVoxelizerInitializer {
	bool operator()(void* data);
};

/*

Distance Field Generation

*/

struct nxDistanceFieldInitializerBlob {
	nxDistanceFieldInitializerBlob(nxEngine* eng, glm::uvec3 dims)
		: m_Engine(eng), m_Dimensions(dims) {}

	nxEngine*		m_Engine;
	glm::uvec3		m_Dimensions;

};

struct nxDistanceFieldInitializer {
	bool operator()(void* data);
};

/*

Reflective SHadow Map Generation

*/

struct nxRSMInitializerBlob {
	nxRSMInitializerBlob(nxEngine* eng, glm::uvec2 dims)
		: m_Engine(eng), m_Dimensions(dims) {}

	nxEngine*		m_Engine;
	glm::uvec2		m_Dimensions;

};

struct nxRSMInitializer {
	bool operator()(void* data);
};

/*

Termination functionalities

*/

struct nxRendererTerminator {
	bool operator()(void* data);
};

struct nxSchedulerTerminator {
	bool operator()(void* data);
};

struct nxWorkerTerminator {
	bool operator()(void* data);
};

struct nxWorkerNotifier {
	bool operator()(void* data);
};