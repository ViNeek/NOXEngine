#include "Utils.h"

#include <GL/glew.h>

// Transformation
#include "bullet/btTransform.h"

#include "JobFactory.h"

namespace Utils {

	namespace GL {
		void CheckGLState(const std::string context);
		btTransform getPerspective(float fovyInDegrees,
			float aspectRatio,
			float znear, float zfar);

		btTransform getOrtho(float left, float right,
			float bottom, float top,
			float znear, float zfar);

		void PerspectiveMatrix(float *matrix,
			float fovyInDegrees,
			float aspectRatio,
			float znear, float zfar);

		void OrthoMatrix(float *matrix,
			float left, float right,
			float bottom, float top,
			float znear, float zfar);

		void Frustum(float *matrix,
			float left, float right,
			float bottom, float top,
			float znear, float zfar);
	};

};