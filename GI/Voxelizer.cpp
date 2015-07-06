#include <boost/log/trivial.hpp>

#include "JobFactory.h"

#include "Voxelizer.h"

#include "Engine.h"
#include "Scene.h"
#include "Renderer.h"

#include "GLUtils.h"

nxVoxelizer::nxVoxelizer(nxEngine* eng, unsigned int dim) {
	m_pEngine = eng;
	m_dimensions = glm::uvec3(dim);
	m_initialized = false;
	m_resolution = dim;
}

void nxVoxelizer::Init() {

	glGenFramebuffers(1, &m_fbo_3_axis);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_3_axis);

	glGenTextures(1, &m_texture_3_axis_id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_3_axis_id);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32UI, m_resolution, m_resolution, 3, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture_3_axis_id, 0);

	Utils::GL::CheckGLState("Voxel Buffer Attach");

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//Utils::GL::CheckGLState("Framebuffer Creation");
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		printf("Single Layered FBO Complete %x\n", status);
		BOOST_LOG_TRIVIAL(info) << "Voxel Layered FBO Complete ";
		break;
	default:
		BOOST_LOG_TRIVIAL(error) << "Voxel Layered FBO Incomplete ";
		break;
	}
}

glm::vec3 nxVoxelizer::GridSize() {
	if (m_pEngine->Scene()->GMaxX() > 0) {
		return glm::vec3(
			m_pEngine->Scene()->GMaxX() - m_pEngine->Scene()->GMinX(),
			m_pEngine->Scene()->GMaxY() - m_pEngine->Scene()->GMinY(),
			m_pEngine->Scene()->GMaxZ() - m_pEngine->Scene()->GMinZ()
		);
	} else {
		return glm::vec3(
			-(m_pEngine->Scene()->GMaxX() + m_pEngine->Scene()->GMinX()),
			-(m_pEngine->Scene()->GMaxY() + m_pEngine->Scene()->GMinY()),
			-(m_pEngine->Scene()->GMaxZ() + m_pEngine->Scene()->GMinZ())
		);
	}
	
}

glm::vec3 nxVoxelizer::GridCenter() {
	glm::vec3 ret;

	ret.x = m_pEngine->Scene()->GMaxX() + m_pEngine->Scene()->GMinX();
	ret.y = m_pEngine->Scene()->GMaxY() + m_pEngine->Scene()->GMinY();
	ret.z = m_pEngine->Scene()->GMaxZ() + m_pEngine->Scene()->GMinZ();

	ret *= 0.5f;

	return ret;

}

glm::vec3 nxVoxelizer::GridMin() {
	return glm::vec3(
		m_pEngine->Scene()->GMinX(),
		m_pEngine->Scene()->GMinY(),
		m_pEngine->Scene()->GMinZ()
	);
}

glm::vec3 nxVoxelizer::GridMax() {
	return glm::vec3(
		m_pEngine->Scene()->GMaxX(),
		m_pEngine->Scene()->GMaxY(),
		m_pEngine->Scene()->GMaxZ()
	);
}

void nxVoxelizer::SetMatrices() {

	glm::vec3 size = GridSize();
	glm::vec3 half_size = size * 0.5f;

	const glm::vec3& center = GridCenter();
	const glm::vec3& bmax = GridMax();
	const glm::vec3& bmin = GridMin();

	glm::vec3 eye;
	glm::vec3 up;

	// for convenience, it is good practice to locate the origin of each new layer to point
	// to the same voxel (i.e. WCS bottom left)

	// X axis
	// set the eye for the X axis camera to be the WCS coordinates of the box's center
	// and set the x coordinate to the most positive x value of the box 
	// (also add one since the near clipping plane is 1.0)
	eye = center;
	eye.x = bmin.x;
	up = glm::vec3(0, 1, 0);
	m_view_axis[0] = glm::lookAt(eye, center, up);

	// Set the projection matrix
	m_proj_axis[0] = glm::ortho(-half_size.z, half_size.z, -half_size.y, half_size.y, 0.0f, size.x);

	// Set the viewport for this axis view (with the appropriate width and height)
	m_viewport[0] = glm::vec4(0.0f, 0.0f, m_dimensions.z, m_dimensions.y);

	// Y axis
	// set the eye for the Y axis camera to be the WCS coordinates of the box's center
	// and set the y coordinate to the most positive y value of the box
	// (also add one since the near clipping plane is 1.0)
	eye = center;
	eye.y = bmax.y;
	up = glm::vec3(-1, 0, 0);
	m_view_axis[1] = glm::lookAt(eye, center, up);

	// Set the projection matrix
	m_proj_axis[1] = glm::ortho(-half_size.z, half_size.z, -half_size.x, half_size.x, 0.0f, size.y);

	// Set the viewport for this axis view (with the appropriate width and height)
	m_viewport[1] = glm::vec4(0.0f, 0.0f, m_dimensions.z, m_dimensions.x);

	// Z axis
	// set the eye for the Z axis camera to be the WCS coordinates of the box's center
	// and set the z coordinate to the most positive z value of the box
	// (also add one since the near clipping plane is 1.0)
	eye = center;
	eye.z = bmax.z;
	up = glm::vec3(0, 1, 0);
	m_view_axis[2] = glm::lookAt(eye, center, up);

	// Set the projection matrix
	m_proj_axis[2] = glm::ortho(-half_size.x, half_size.x, -half_size.y, half_size.y, 0.0f, size.z);

	// Set the viewport for this axis view (with the appropriate width and height)
	m_viewport[2] = glm::vec4(0.0f, 0.0f, m_dimensions.x, m_dimensions.y);

	for (int i = 0; i < 3; ++i)
		m_view_proj_axis[i] = m_proj_axis[i] * m_view_axis[i];
}

bool nxVoxelizerInitializer::operator()(void* data) {
	nxVoxelizerInitializerBlob* blob = (nxVoxelizerInitializerBlob*)data;

	BOOST_LOG_TRIVIAL(info) << "Initializing Voxelizer ";

	blob->m_Engine->Renderer()->Voxelizer()->SetMatrices();
	blob->m_Engine->Renderer()->Voxelizer()->Init();
	blob->m_Engine->Renderer()->SetVoxelizerReady(true);

	return true;
}