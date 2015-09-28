#include "JobFactory.h"

#include "Job.h"
#include "CustomEvents.h"
#include "Scheduler.h"
#include "Worker.h"

#include <GL/glew.h>

#include <iostream>

nxDummyJob						nxDummyJob_;
nxRendererTerminator			nxRendererTerminator_;
nxSchedulerTerminator			nxSchedulerTerminator_;
nxWorkerTerminator				nxWorkerTerminator_;
nxWorkerNotifier				nxWorkerNotifier_;
nxSceneLoader					nxSceneLoader_;				
nxAssetLoader					nxAssetLoader_;
nxShaderLoader					nxShaderLoader_;
nxProgramSwapper				nxProgramSwapper_;
nxExtensionInitializer			nxExtensionInitializer_;
nxFramebufferInitializer		nxFramebufferInitializer_;
nxVoxelizerInitializer			nxVoxelizerInitializer_;
nxFramebufferResizer			nxFramebufferResizer_;
nxShaderCompiler				nxShaderCompiler_;
nxGLAssetLoader					nxGLAssetLoader_;
nxProgramLinker					nxProgramLinker_;

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
	nxProgramLinker_,
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