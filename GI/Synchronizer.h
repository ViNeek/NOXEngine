#pragma once

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

class nxSynchronizer {
public:
									nxSynchronizer(){};
	boost::condition_variable&		ConditionVariable() { return m_Cond; };
	boost::mutex&					Mutex() { return m_Mutex; };
	
private:

	boost::condition_variable		m_Cond;
	boost::mutex					m_Mutex;

};