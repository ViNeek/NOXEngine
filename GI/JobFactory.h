#pragma once

#include <GL/glew.h>
#include <string>

#include <glm/vec3.hpp>

class nxJob;
class nxFrame;
class nxShader;
class nxProgram;
class nxEngine;
class nxEntity;
class nxScene;
class nxRenderer;

// Request from glbal NOX Engine State
#define NOX_ENGINE_GET(Object) ((nxEngine*)data)->Object()

enum nxJobID {
	NX_JOB_DUMMY,
	NX_JOB_EXIT, 
	NX_JOB_RENDERER_EXIT,
	NX_JOB_SCHEDULER_EXIT,
	NX_JOB_WORKER_EXIT,
	NX_JOB_RENDERER_FINISHED,
	NX_JOB_WORKER_FINISHED,
	NX_JOB_LOAD_SCENE,
	NX_JOB_LOAD_ASSET,
	NX_JOB_LOAD_SHADER,
	NX_GL_JOB_PROGRAM_SWAP,
	NX_GL_JOB_EXTENSION_INIT,
	NX_GL_JOB_FRAMEBUFFER_INIT,
	NX_GL_JOB_VOXELIZER_INIT,
	NX_GL_JOB_FRAMEBUFFER_RESIZE,
	NX_GL_JOB_COMPILE_SHADER,
	NX_GL_JOB_LOAD_ASSET,
	NX_GL_JOB_LINK_PROGRAM
};

class nxJobFactory {

public:

	static nxJob* CreateJob(nxJobID id, void* data = 0);

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
	nxShaderLoaderBlob(nxEngine* eng, nxProgram* prog, std::string source, GLenum type)
		: m_Engine( eng ), m_Prog( prog ), m_Source( source ), m_Type( type ) {}

	nxEngine*		m_Engine;
	nxProgram*		m_Prog;
	std::string		m_Source;
	GLenum			m_Type;
};

struct nxShaderLoader {
	bool operator()(void* data);
};


struct nxShaderCompilerBlob {
	nxShaderCompilerBlob(nxEngine* eng, nxProgram* prog, nxShader* shad )
		: m_Engine( eng ), m_Prog( prog ), m_Shader( shad ) {}

	nxEngine*		m_Engine;
	nxProgram*		m_Prog;
	nxShader*		m_Shader;
	
};

struct nxShaderCompiler {
	bool operator()(void* data);
};

struct nxProgramLinkerBlob {
	nxProgramLinkerBlob(nxEngine* eng, nxProgram* prog)
		: m_Engine( eng ), m_Prog( prog ) {}

	nxEngine*		m_Engine;
	nxProgram*		m_Prog;

};

struct nxProgramSwapper {
	bool operator()(void* data);
};

struct nxProgramSwapperBlob {
	nxProgramSwapperBlob(nxRenderer* rend, nxProgram* prog)
		: m_Renderer(rend), m_Prog(prog) {}

	nxRenderer*		m_Renderer;
	nxProgram*		m_Prog;

};

struct nxProgramLinker {
	bool operator()(void* data);
};

struct nxSceneLoader {
	bool operator()(void* data);
};

struct nxGLAssetLoaderBlob {
	nxGLAssetLoaderBlob(nxEngine* eng, nxEntity* ent )
		: m_Engine( eng ), m_Entity( ent ) {}

	nxEngine*		m_Engine;
	nxEntity*		m_Entity;

};

struct nxGLAssetLoader {
	bool operator()(void* data);
};

struct nxAssetLoaderBlob {
	nxAssetLoaderBlob(nxEngine* eng, std::string path, std::string type, glm::vec3 cent, float scale)
		: m_Engine( eng ), m_ResourcePath( path ), m_ResourceType( type ), m_Center( cent ), m_ScaleFactor( scale ) {}

	nxEngine*		m_Engine;
	std::string		m_ResourcePath;
	std::string		m_ResourceType;
	glm::vec3		m_Center;
	float			m_ScaleFactor;
};

struct nxAssetLoader {
	bool operator()(void* data);
};

struct nxVoxelizerInitializerBlob {
	nxVoxelizerInitializerBlob(nxEngine* eng, glm::uvec3 dims)
		: m_Engine(eng), m_Dimensions(dims) {}

	nxEngine*		m_Engine;
	glm::uvec3		m_Dimensions;
};

struct nxVoxelizerInitializer {
	bool operator()(void* data);
};

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