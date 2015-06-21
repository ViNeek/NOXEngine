#pragma once

#include <boost/function.hpp>

typedef boost::function<void(void* data)> nxJobCallback;

class nxJob {
public:

	nxJob(void* data, nxJobCallback cb);

private:

	void*				m_pData;
	nxJobCallback		m_Callback;

};

class nxGLJob : public nxJob {
};