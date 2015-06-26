#include "Worker.h"
#include "Job.h"

#include <boost/log/trivial.hpp>

nxWorker::nxWorker(nxJobQueue* q) {
	m_pCommandQueue = q;
}

void* nxWorker::Entry() {
	Init();

	nxJob* currentJob;
	while (m_IsActive) {

		if (m_pCommandQueue->pop(currentJob))
			m_IsActive = currentJob->Execute();

	}

	return NULL;
}

void nxWorker::Init() {
	BOOST_LOG_TRIVIAL(info) << "Worker with ID " << GetId() << " running";
}