#pragma once

// Transformation
#include "bullet/btTransform.h"

#define BYTE unsigned char

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

namespace Utils {

	namespace Debug {
		void init_debug_console();
	};
};