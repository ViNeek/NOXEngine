#include "JobFactory.h"

#include "Job.h"
#include "CustomEvents.h"
#include "Scheduler.h"
#include "Worker.h"

#include <GL/glew.h>

#include <iostream>

#include <boost/timer/timer.hpp>

using boost::timer::cpu_timer;
using boost::timer::cpu_times;
using boost::timer::nanosecond_type;

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

}

bool nxDummyJob::operator()(void* data) {
	//wxThread::Sleep(200);
	std::cout << "dummy here ";

	return true;
}

nxJob::nxJob(void* data, nxJobCallback cb, bool timed = false)
{
	m_pData = data;
	m_Callback = cb;
	m_Timer = new nxTimer();
}

nxGLJob::nxGLJob(void* data, nxJobCallback cb)
	: nxJob(data, cb) {

}