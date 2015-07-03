#pragma once

#include <wx/wx.h>

#include <vector>

#include <boost/lockfree/queue.hpp>

class nxWorker;
class nxJob;
class nxSynchronizer;
class nxEngine;

typedef boost::lockfree::queue< nxJob* > nxJobQueue;

class nxScheduler : public wxThread {
public:
									nxScheduler(nxEngine* eng);
									nxScheduler(wxFrame* parent);
	void*							Entry();

	// Assign to workers
	void							ScheduleJob(nxJob* j);
	void							ScheduleJobBatch(std::vector<nxJob*> jobs);
	void							SetFrame(wxFrame* parent) { m_pParent = parent; };

	// Assign to scheduler
	void							ScheduleOwnJob(nxJob* j);

	int								WorkerCount() { return m_WorkerCount; }
	bool							KillWorker() { return (--m_WorkerCount) == 0; };
	std::vector<nxWorker*>&			Workers() { return m_vWorkers; }
	nxJobQueue*						WorkerQueue() { return m_pWorkersCommandQueue; };

	wxFrame*						EventHandler() { return m_pParent; }

private:

	wxFrame*						m_pParent;

	std::vector<nxWorker*>			m_vWorkers;
	nxJobQueue*						m_pWorkersCommandQueue;
	nxJobQueue*						m_pCommandQueue;

	nxSynchronizer*					m_SchedulerSync;
	nxSynchronizer*					m_WorkersSync;

	nxEngine*						m_pEngine;

	bool							m_IsActive;
	int								m_WorkerCount;

	void							Init();
};