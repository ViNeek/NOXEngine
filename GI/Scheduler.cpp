#include "Scheduler.h"
#include "Job.h"
#include "Worker.h"
#include "Synchronizer.h"
#include "CustomEvents.h"
#include "JobFactory.h"

#define BASE_THREAD_COUNT 3

#include <boost/log/trivial.hpp>

nxScheduler::nxScheduler(nxEngine* eng)
{
	m_pEngine = eng;
	m_pParent = NULL;
	m_IsActive = true;
	m_pCommandQueue = new nxJobQueue(0);
	m_pWorkersCommandQueue = new nxJobQueue(0);
	m_WorkerCount = 0;
	m_SchedulerSync = new nxSynchronizer();
	m_WorkersSync = new nxSynchronizer();
}

nxScheduler::nxScheduler(wxFrame* parent)
{
	m_pParent = parent;
	m_IsActive = true;
	m_pCommandQueue = new nxJobQueue(0);
	m_pWorkersCommandQueue = new nxJobQueue(0);
	m_WorkerCount = 0;
	m_SchedulerSync = new nxSynchronizer();
	m_WorkersSync = new nxSynchronizer();
}

void* nxScheduler::Entry()
{
	Init();

	nxJob* currentJob;

	// First time no CV to avoid missed notify
	while (m_IsActive) {
		if (m_pCommandQueue->pop(currentJob))
			m_IsActive = currentJob->Execute();
		else
			break;
	}

	// blocking, lock-free queue loop
	while (m_IsActive) {
		//std::cout << "waiting" << std::endl;
		boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(1);
		boost::unique_lock<boost::mutex> lock( m_SchedulerSync->Mutex() );
		m_SchedulerSync->ConditionVariable().timed_wait(lock, timeout);
		m_WorkersSync->ConditionVariable().notify_all();
		//std::cout << "released" << std::endl;
		while (m_pCommandQueue->pop(currentJob) ) {
			//std::cout << "lock free ";
			m_IsActive = currentJob->Execute();
		}
	}

	wxFrame* evtHandler = m_pParent;
	wxCommandEvent* evt = new wxCommandEvent(nxSCHEDULER_EXIT_EVENT); // Still keeping it simple, don't give a specific event ID
	wxQueueEvent(evtHandler, evt); // This posts to ourselves: it'll be caught and sent to a different method

	return NULL;
}

void nxScheduler::ScheduleJob(nxJob* job) {
	m_pWorkersCommandQueue->push(job);
	m_WorkersSync->ConditionVariable().notify_all();
}

void nxScheduler::ScheduleJobBatch(std::vector<nxJob*> jobs) {
	for (size_t i = 0; i < jobs.size(); i++) {
		m_pWorkersCommandQueue->push(jobs[i]);
	}
	m_WorkersSync->ConditionVariable().notify_all();
}

void nxScheduler::ScheduleOwnJob(nxJob* job) {
	m_pCommandQueue->push(job);
	m_SchedulerSync->ConditionVariable().notify_one();
}

void nxScheduler::Init() {
	m_WorkerCount = wxThread::GetCPUCount() - BASE_THREAD_COUNT;
	if (m_WorkerCount <= 3)
		m_WorkerCount = 2;

	m_WorkerCount *= 2;

	BOOST_LOG_TRIVIAL(info) << "CPU Count : " << wxThread::GetCPUCount;
	BOOST_LOG_TRIVIAL(info) << "Worker Count : " << m_WorkerCount;

	for (int i = 0; i < m_WorkerCount; i++) {
		nxWorker* worker = new nxWorker(m_pWorkersCommandQueue, m_WorkersSync);
		worker->Create();
		if ( worker->Run() != wxTHREAD_NO_ERROR )
		{
			delete worker;
			worker = NULL;

			BOOST_LOG_TRIVIAL(error) << "Worker thread num "<< i <<" failed to started";

			continue;
		}

		m_vWorkers.push_back(worker);
	}

	BOOST_LOG_TRIVIAL(info) << "Worker Vector Size : " << m_vWorkers.size();
	BOOST_LOG_TRIVIAL(info) << "Worker Vector Size : " << Workers().size();

}

bool nxSchedulerTerminator::operator()(void* data) {
	nxScheduler* scheduler = (nxScheduler*)data;

	nxJob* dummy_j;
	while (scheduler->WorkerQueue()->pop(dummy_j));
	std::cout << "Size : " << scheduler->Workers().size();
	for ( size_t i = 0; i < scheduler->Workers().size(); i++) {
		std::cout << " firing ";
		scheduler->ScheduleJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_WORKER_EXIT, scheduler));
	}

	return true;
}