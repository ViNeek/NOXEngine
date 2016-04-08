#pragma once

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "CustomTypes.h"

#include <string>

namespace Utils {

	namespace Debug {
		void init_debug_console();
	};


	nxInt32 RandomInRange(nxInt32 min, nxInt32 max);

    std::string GetParentDirectory(const std::string& file);
    std::string GetFilename(const std::string& path_to_file);
};