#pragma once

#include "wx/wx.h"

#include <boost/lockfree/queue.hpp>

class nxJob;

typedef boost::lockfree::queue< nxJob* > nxJobQueue;

class nxWorker : public wxThread
{
public:

	nxWorker(nxJobQueue* q);
	void *Entry();

private:

	bool				m_IsActive;
	nxJobQueue*			m_pJobQueue;

	void Init();
};