#include "Scheduler.h"
#include "Job.h"

#define BASE_THREAD_COUNT 3

nxScheduler::nxScheduler()
{
	m_IsActive = true;
	m_pCommandQueue = new nxJobQueue(0);
	m_WorkerCount = 0;
}

void* nxScheduler::Entry()
{
	Init();

	nxJob* currentJob;
	while (m_IsActive) {

		if (m_pCommandQueue->pop(currentJob))
			m_IsActive = currentJob->Execute();

	}

	return NULL;
}

void nxScheduler::Init() {
	m_WorkerCount = wxThread::GetCPUCount() - BASE_THREAD_COUNT;
	if (m_WorkerCount <= 3)
		m_WorkerCount = 2;
}