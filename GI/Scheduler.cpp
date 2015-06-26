#include "Scheduler.h"
#include "Job.h"
#include "Worker.h"

#define BASE_THREAD_COUNT 3

#include <boost/log/trivial.hpp>

nxScheduler::nxScheduler(wxFrame* parent)
{
	m_IsActive = true;
	m_pCommandQueue = new nxJobQueue(0);
	m_pWorkersCommandQueue = new nxJobQueue(0);
	m_WorkerCount = 0;
}

void* nxScheduler::Entry()
{
	Init();

	nxJob* currentJob;
	while (m_IsActive) {

		if (m_pCommandQueue->pop(currentJob))
			m_pWorkersCommandQueue->push(currentJob);

	}

	return NULL;
}

void nxScheduler::Init() {
	m_WorkerCount = wxThread::GetCPUCount() - BASE_THREAD_COUNT;
	if (m_WorkerCount <= 3)
		m_WorkerCount = 2;

	BOOST_LOG_TRIVIAL(info) << "CPU Count : " << wxThread::GetCPUCount;
	BOOST_LOG_TRIVIAL(info) << "Worker Count : " << m_WorkerCount;

	for (int i = 0; i < m_WorkerCount; i++) {
		nxWorker* worker = new nxWorker(m_pWorkersCommandQueue);
		if ( worker->Run() != wxTHREAD_NO_ERROR )
		{
			delete worker;
			worker = NULL;

			BOOST_LOG_TRIVIAL(error) << "Worker thread num "<< i <<" failed to started";

			continue;
		}

		m_vWorkers.push_back(worker);
		worker->Run();
	}
}