#pragma once

#include "wx/wx.h"

#include <boost/lockfree/queue.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

class nxJob;
class nxSynchronizer;

typedef boost::lockfree::queue< nxJob* > nxJobQueue;

class nxWorker : public wxThread
{
public:

						nxWorker(nxJobQueue* q, nxSynchronizer* sync);
	void*				Entry();

private:

	bool					m_IsActive;
	nxJobQueue*				m_pCommandQueue;
	nxSynchronizer*			m_WorkersSync;
	void					Init();

};