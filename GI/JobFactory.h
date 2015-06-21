#pragma once

class nxJob;

enum nxJobID {
	NX_JOB_DUMMY,
	NX_JOB_LOAD_SCENE,
	NX_JOB_LOAD_ASSET,
	NX_GL_JOB_EXTENSION_INIT,
	NX_GL_JOB_SHADER_LOAD
};

class nxJobFactory {
	static nxJob* CreateJob(nxJobID id, void* data);
};

struct nxExtensionInitializer {
	void operator()(void* data);
};

struct nxShaderLoader {
	void operator()(void* data);
};

struct nxSceneLoader {
	void operator()(void* data);
};

struct nxAssetLoader {
	void operator()(void* data);
};