#include "JobFactory.h"

#include "Job.h"
#include "CustomEvents.h"
#include "Scheduler.h"
#include "Worker.h"

#include <GL/glew.h>

#include <iostream>

// Callback Objects
static const nxDummyJob						nxDummyJob_;
static const nxRendererTerminator			nxRendererTerminator_;
static const nxSchedulerTerminator			nxSchedulerTerminator_;
static const nxWorkerTerminator				nxWorkerTerminator_;
static const nxWorkerNotifier				nxWorkerNotifier_;
static const nxSceneLoader					nxSceneLoader_;
static const nxAssetLoader					nxAssetLoader_;
static const nxShaderLoader					nxShaderLoader_;
static const nxProgramSwapper				nxProgramSwapper_;
static const nxExtensionInitializer			nxExtensionInitializer_;
static const nxFramebufferInitializer		nxFramebufferInitializer_;
static const nxVoxelizerInitializer			nxVoxelizerInitializer_;
static const nxFramebufferResizer			nxFramebufferResizer_;
static const nxShaderCompiler				nxShaderCompiler_;
static const nxGLAssetLoader				nxGLAssetLoader_;
static const nxGLBufferedAssetLoader		nxGLBufferedAssetLoader_;
static const nxProgramLinker				nxProgramLinker_;

static const nxJobCallback nxJobDispatchTable[NX_JOB_MAX]
= {
	nxDummyJob_,
	nxDummyJob_,
	nxRendererTerminator_,
	nxSchedulerTerminator_,
	nxWorkerTerminator_,
	nxWorkerNotifier_,
	nxSceneLoader_,
	nxAssetLoader_,
	nxShaderLoader_,
	nxProgramSwapper_,
	nxExtensionInitializer_,
	nxFramebufferInitializer_,
	nxVoxelizerInitializer_,
	nxFramebufferResizer_,
	nxShaderCompiler_,
	nxGLAssetLoader_,
	nxGLBufferedAssetLoader_,
	nxProgramLinker_
};

nxJob* nxJobFactory::CreateJob(nxJobID id, void* data)
{	
	return new nxJob(data, nxJobDispatchTable[id]);

	/*
	switch (id) {
	case NX_JOB_DUMMY:
	{
		nxDummyJob cb;
		j = new nxJob(data, cb);
	}
		break;
	case NX_JOB_WORKER_FINISHED:
	{
		nxWorkerNotifier cb;
		j = new nxJob(data, cb);
	}
		break;
	case NX_JOB_RENDERER_EXIT:
	{
		nxRendererTerminator cb;
		j = new nxJob(data, cb);
	}
		break;
	case NX_JOB_WORKER_EXIT:
	{
		nxWorkerTerminator cb;
		j = new nxJob(data, cb);
	}
		break;
	case NX_JOB_SCHEDULER_EXIT:
	{
		nxSchedulerTerminator cb;
		j = new nxJob(data, cb);
	}
		break;
	case NX_JOB_LOAD_SCENE:
	{
		nxSceneLoader cb;
		j = new nxJob(data, cb);
	}
		break;
	case NX_JOB_LOAD_ASSET:
	{
		nxAssetLoader cb;
		j = new nxJob(data, cb);
	}
		break;
	case NX_JOB_LOAD_SHADER:
	{
		nxShaderLoader cb;
		j = new nxJob(data, cb);
	}
		break;
	case NX_GL_JOB_EXTENSION_INIT:
	{
		nxExtensionInitializer cb;
		j = new nxGLJob(data, cb);
	}
		break;
	case NX_GL_JOB_COMPILE_SHADER:
	{
		nxShaderCompiler cb;
		j = new nxGLJob(data, cb);
	}
		break;
	case NX_GL_JOB_VOXELIZER_INIT:
	{
		nxVoxelizerInitializer cb;
		j = new nxGLJob(data, cb);
	}
		break;
	case NX_GL_JOB_LINK_PROGRAM:
	{
		nxProgramLinker cb;
		j = new nxGLJob(data, cb);
	}
		break;
	case NX_GL_JOB_LOAD_ASSET:
	{
		nxGLAssetLoader cb;
		j = new nxGLJob(data, cb);
	}
		break;
	case NX_GL_JOB_FRAMEBUFFER_INIT:
	{
		nxFramebufferInitializer cb;
		j = new nxGLJob(data, cb);
	}
		break;
	case NX_GL_JOB_FRAMEBUFFER_RESIZE:
	{
		nxFramebufferResizer cb;
		j = new nxGLJob(data, cb);
	}
		break;
	default:
		break;
	}

	return j;
	*/
}

bool nxDummyJob::operator()(void* data) {
	//wxThread::Sleep(200);
	std::cout << "dummy here ";

	return true;
}

nxJob::nxJob(void* data, nxJobCallback cb)
{
	m_pData = data;
	m_Callback = cb;
}

nxGLJob::nxGLJob(void* data, nxJobCallback cb)
	: nxJob(data, cb) {

}