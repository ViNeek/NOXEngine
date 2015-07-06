#include "JobFactory.h"

#include "Job.h"
#include "CustomEvents.h"
#include "Scheduler.h"
#include "Worker.h"

#include <GL/glew.h>

#include <iostream>

nxJob* nxJobFactory::CreateJob(nxJobID id, void* data)
{
	nxJob* j = NULL;
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