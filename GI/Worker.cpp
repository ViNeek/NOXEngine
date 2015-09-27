#include "Worker.h"
#include "Job.h"
#include "JobFactory.h"
#include "Synchronizer.h"
#include "Scheduler.h"

#include <boost/log/trivial.hpp>

nxWorker::nxWorker(nxJobQueue* q, nxSynchronizer* sync ) {
	m_IsActive = true;
	m_pCommandQueue = q;
	m_WorkersSync = sync;
}

void* nxWorker::Entry() {
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
		//std::cout << "worker waiting" << std::endl;
		//boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(1000);
		boost::unique_lock<boost::mutex> lock(m_WorkersSync->Mutex());
		m_WorkersSync->ConditionVariable().wait(lock);
		lock.unlock();
		//std::cout << "Thread " << wxThread::GetId() << " worker released" << std::endl;
		while (m_pCommandQueue->pop(currentJob) ) {
			std::cout << "Thread "<< wxThread::GetId() <<"\n";
			
			m_IsActive = currentJob->Execute();
		}
	}

	return NULL;
}

void nxWorker::Init() {
	BOOST_LOG_TRIVIAL(info) << "Worker with ID " << GetId() << " running";
}

bool nxWorkerTerminator::operator()(void* data) {
	nxScheduler* scheduler = (nxScheduler*)data;
	std::cout << "worker ending\n";
	scheduler->ScheduleOwnJob(nxJobFactory::CreateJob(NX_JOB_WORKER_FINISHED, scheduler));

	return false;
}

bool nxWorkerNotifier::operator()(void* data) {
	nxScheduler* scheduler = (nxScheduler*)data;
	std::cout << "going out\n";
	if (scheduler->KillWorker())
		return false;

	return true;
}