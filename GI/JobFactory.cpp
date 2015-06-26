#include "JobFactory.h"

#include "Job.h"
#include "CustomEvents.h"

#include <GL/glew.h>

#include <iostream>

nxJob* nxJobFactory::CreateJob(nxJobID id, void* data)
{
	nxJob* j = NULL;
	switch (id) {
	case NX_JOB_DUMMY:
	{
		//nxJob* j = new nxJob();
	}
		break;
	case NX_JOB_EXIT:
	{
		nxRendererTerminator cb;
		j = new nxJob(data, cb);
	}
		break;
	case NX_GL_JOB_EXTENSION_INIT:
	{
		nxExtensionInitializer cb;
		j = new nxGLJob(data, cb);
	}
		break;
	case NX_GL_JOB_SHADER_LOAD:
		//j = new nxJob();
		break;
	default:
		break;
	}

	return j;
}

bool nxExtensionInitializer::operator()(void* data) {
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

		return false;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	return true;
}

bool nxRendererTerminator::operator()(void* data) {
	return false;
}

bool nxSchedulerTerminator::operator()(void* data) {
	wxFrame* evtHandler = (wxFrame*)data;
	wxCommandEvent* evt = new wxCommandEvent(nxSCHEDULER_EXIT_EVENT); // Still keeping it simple, don't give a specific event ID
	wxQueueEvent(evtHandler, evt); // This posts to ourselves: it'll be caught and sent to a different method

	return false;
}

nxJob::nxJob(void* data, nxJobCallback cb)
{
	m_pData = data;
	m_Callback = cb;
}

nxGLJob::nxGLJob(void* data, nxJobCallback cb)
	: nxJob(data, cb) {

}