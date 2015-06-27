#pragma once

class nxJob;
class nxFrame;

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
	NX_GL_JOB_EXTENSION_INIT,
	NX_GL_JOB_LOAD_SHADER,
	NX_GL_JOB_LOAD_PROGRAM
};

class nxJobFactory {

public:

	static nxJob* CreateJob(nxJobID id, void* data = 0);

private:

};

struct nxDummyJob {
	bool operator()(void* data);
};

struct nxExtensionInitializer {
	bool operator()(void* data);
};

struct nxShaderLoader {
	bool operator()(void* data);
};

struct nxSceneLoader {
	bool operator()(void* data);
};

struct nxAssetLoader {
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