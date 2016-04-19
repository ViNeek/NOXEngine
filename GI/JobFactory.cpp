#include "JobFactory.h"

#include "Constants.h"
#include "Job.h"
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
static const nxShaderReloader				nxShaderReloader_;
static const nxProgramAdder				    nxProgramAdder_;
static const nxResourceLooper				nxResourceLooper_;
static const nxProgramSwapper				nxProgramSwapper_;
static const nxExtensionInitializer			nxExtensionInitializer_;
static const nxFramebufferInitializer		nxFramebufferInitializer_;
static const nxVoxelizerInitializer			nxVoxelizerInitializer_;
static const nxDistanceFieldInitializer		nxDistanceFieldInitializer_;
static const nxRayMarcherInitializer		nxRayMarcherInitializer_;
static const nxRSMInitializer				nxRSMInitializer_;
static const nxFramebufferResizer			nxFramebufferResizer_;
static const nxShaderCompiler				nxShaderCompiler_;
static const nxGLTextureLoader				nxGLTextureLoader_;
static const nxGLAssetLoader				nxGLAssetLoader_;
static const nxGLBufferedAssetLoader		nxGLBufferedAssetLoader_;
static const nxGLDebugAssetLoader			nxGLDebugAssetLoader_;
static const nxProgramLinker				nxProgramLinker_;
static const nxProgramLinker				nxProgramRelinker_;

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
    nxShaderReloader_,
    nxProgramAdder_,
    nxResourceLooper_,
	nxProgramSwapper_,
	nxExtensionInitializer_,
	nxFramebufferInitializer_,
	nxVoxelizerInitializer_,
	nxDistanceFieldInitializer_,
	nxRayMarcherInitializer_,
	nxRSMInitializer_,
	nxFramebufferResizer_,
	nxShaderCompiler_,
    nxGLTextureLoader_,
    nxGLAssetLoader_,
    nxGLBufferedAssetLoader_,
	nxGLDebugAssetLoader_,
    nxProgramLinker_,
    nxProgramRelinker_
};

nxJob* nxJobFactory::CreateJob(nxJobID id, void* data, bool later, nxUInt64 after)
{	

	return new nxJob(data, nxJobDispatchTable[id], later, after);

}

bool nxDummyJob::operator()(void* data) {
	//wxThread::Sleep(200);
	std::cout << "dummy here ";

	return true;
}

nxJob::nxJob(void* data, nxJobCallback cb, bool later, nxUInt64 after)
{

	m_pData = data;
	m_Callback = cb;
	m_Timer = nullptr;
	m_Delay = 0;
	if (later) {
		m_Timer = new nxTimer();
		m_Delay = after;
		m_Timer->start();
	}
}

nxGLJob::nxGLJob(void* data, nxJobCallback cb, bool later, nxUInt64 after)
	: nxJob(data, cb, later, after) {

}