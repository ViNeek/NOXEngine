#include "Worker.h"
#include "Job.h"

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

}