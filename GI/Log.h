#pragma once

#include "Constants.h"

#include <boost/function.hpp>
#include <boost/timer/timer.hpp>
#include <boost/log/trivial.hpp>

typedef boost::function<bool(void* data)> nxJobCallback;

typedef boost::timer::cpu_timer			nxTimer;
typedef boost::timer::nanosecond_type	nxNanoSeconds;

class nxLog {
public:

private:


};