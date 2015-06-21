#include "JobFactory.h"

#include "Job.h"

nxJob* nxJobFactory::CreateJob(nxJobID id, void* data)
{
	nxJob* j = NULL;
	switch (id) {
	case NX_JOB_DUMMY:
		//nxJob* j = new nxJob();
		break;
	case NX_GL_JOB_EXTENSION_INIT:
		nxExtensionInitializer cb;
		j = new nxJob(data, cb);
		break;
	case NX_GL_JOB_SHADER_LOAD:
		//j = new nxJob();
		break;
	default:
		break;
	}

	return j;
}

void nxExtensionInitializer::operator()(void* data) {
	return;
}

nxJob::nxJob(void* data, nxJobCallback cb)
{
	m_pData = data;
	m_Callback = cb;
}