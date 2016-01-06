#pragma once

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "CustomTypes.h"

namespace Utils {

	namespace Debug {
		void init_debug_console();
	};


	nxInt32 RandomInRange(nxInt32 min, nxInt32 max);

};