#pragma once

// Transformation
#include "bullet/btTransform.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

namespace Utils {

	namespace Debug {
		void init_debug_console();
	};

	namespace GL {
		btTransform getPerspective(float fovyInDegrees,
			float aspectRatio,
			float znear, float zfar);

		void Perspective(float *matrix,
			float fovyInDegrees, 
			float aspectRatio,
			float znear, float zfar);

		void Frustum(float *matrix,
			float left, float right,
			float bottom, float top,
			float znear, float zfar);
	};
};