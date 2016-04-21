#pragma once

#include "Constants.h"

#include <boost/function.hpp>
#include <boost/timer/timer.hpp>
#include <boost/log/trivial.hpp>

typedef boost::function<bool(void* data)> nxJobCallback;

typedef boost::timer::cpu_timer			nxTimer;
typedef boost::timer::nanosecond_type	nxNanoSeconds;

class nxJob {
public:

	nxJob(void* data, nxJobCallback cb, bool later = false, nxUInt64 after = NOXConstants::nxTimeNow);
	bool Execute() { return m_Callback(m_pData); };
	bool AboutTime() { 

		using boost::timer::cpu_times;
		//BOOST_LOG_TRIVIAL(info) << "Check Time ";

		if (m_Timer == nullptr) {
			return true;
		} else {
			cpu_times const elapsed_times(m_Timer->elapsed());
			nxNanoSeconds const elapsed(elapsed_times.system
				+ elapsed_times.user);

			if (elapsed >= m_Delay) {
		        //BOOST_LOG_TRIVIAL(info) << "Resource Loop NOW ";

				return true;
			} else {
				//BOOST_LOG_TRIVIAL(info) << "Resource Loop NOT yet ";
				return false;
			}
		}
	}

private:

	void*				m_pData;
	nxJobCallback		m_Callback;
	nxTimer*			m_Timer;
	nxNanoSeconds		m_Delay;

};

class nxGLJob : public nxJob {

public:

	nxGLJob(void* data, nxJobCallback cb, bool later = false, nxUInt64 after = NOXConstants::nxTimeNow);

private:

};