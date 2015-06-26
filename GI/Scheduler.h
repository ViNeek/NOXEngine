#pragma once

#include <wx/wx.h>

#include <vector>

#include <boost/lockfree/queue.hpp>

class nxWorker;
class nxJob;

typedef boost::lockfree::queue< nxJob* > nxJobQueue;

class nxScheduler : public wxThread {
public:
								nxScheduler();
	void*						Entry();

	void						ScheduleJob(nxJob* j);

private:

	std::vector<nxWorker*>		m_vWorkers;
	nxJobQueue*					m_pCommandQueue;
	bool						m_IsActive;
	int							m_WorkerCount;

	void						Init();
};