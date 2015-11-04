#pragma once

#include <boost/function.hpp>

typedef boost::function<bool(void* data)> nxJobCallback;

class cpu_timer;

typedef cpu_timer nxTimer;

class nxJob {
public:

	nxJob(void* data, nxJobCallback cb);
	bool Execute() { return m_Callback(m_pData); };

private:

	void*				m_pData;
	nxJobCallback		m_Callback;
	nxTimer*			m_Timer;

};

class nxGLJob : public nxJob {

public:

	nxGLJob(void* data, nxJobCallback cb, bool timed = false);

private:

};