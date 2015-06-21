#include "Worker.h"

nxWorker::nxWorker(nxJobQueue* q) {
	m_pJobQueue = q;
}

void* nxWorker::Entry() {

}