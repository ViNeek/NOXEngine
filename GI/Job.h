#pragma once

#include <boost/function.hpp>

typedef boost::function<bool(void* data)> nxJobCallback;

class nxJob {
public:

	nxJob(void* data, nxJobCallback cb);
	bool Execute() { return m_Callback(m_pData); };

private:

	void*				m_pData;
	nxJobCallback		m_Callback;

};

class nxGLJob : public nxJob {

public:
	nxGLJob(void* data, nxJobCallback cb);

private:

};