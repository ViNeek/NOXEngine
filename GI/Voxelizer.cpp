#include <boost/log/trivial.hpp>

#include "JobFactory.h"

#include "Voxelizer.h"

#include "Engine.h"
#include "Scene.h"
#include "Renderer.h"

#include "GLUtils.h"
#include "CustomTypes.h"

nxVoxelizer::nxVoxelizer(nxEngine* eng, nxUInt32 dim) : nxVoxelizerBase("dumyy") {
	m_pEngine = eng;
	m_dimensions = glm::uvec3(dim);
	m_initialized = false;
	m_resolution = dim;
	m_ssbo = -1;
	m_DummyLayeredBuffer = -1;
}

bool nxVoxelizer::Init() {
	Utils::GL::CheckGLState("Width Framebuffer Creation");
	glGenFramebuffers(1, &m_DummyLayeredBuffer);
	Utils::GL::CheckGLState("Width Framebuffer Creation");
	glBindFramebuffer(GL_FRAMEBUFFER, m_DummyLayeredBuffer);
	Utils::GL::CheckGLState("Width Framebuffer Creation");
	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, m_resolution);
	Utils::GL::CheckGLState("Width Framebuffer Creation");

	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, m_resolution);
	Utils::GL::CheckGLState("Height Framebuffer Creation");

	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_LAYERS, 3);
	Utils::GL::CheckGLState("Layers Framebuffer Creation");


	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	Utils::GL::CheckGLState("Dummy Framebuffer Creation");
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		BOOST_LOG_TRIVIAL(info) << "Voxel(Dummy) Layered FBO Complete ";
		break;
	default:
		BOOST_LOG_TRIVIAL(error) << "Voxel(Dummy) Layered FBO Incomplete ";
		break;
	}

	glGenFramebuffers(1, &m_fbo_3_axis);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_3_axis);

	Utils::GL::CheckGLState("SSBO Attach");

	glGenBuffers(1, &m_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_resolution * m_resolution * m_resolution / 8, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	glGenTextures(1, &m_texture_3_axis_id);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_3_axis_id);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32UI, m_resolution, m_resolution, 3, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture_3_axis_id, 0);

	Utils::GL::CheckGLState("Voxel Buffer Attach");

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
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

	return true;
}

void nxVoxelizer::PrintGrid() {
	GLuint width = m_dimensions.x;
	GLuint height = m_dimensions.y;
	GLuint depth = m_dimensions.z;
	GLuint voxel_grid_size = width * height * depth;

	GLuint count = width * height * 3;
	int f = sizeof(glm::uvec4);
	glm::uvec4 * data = (glm::uvec4 *)boost::alignment::aligned_alloc(16, count * sizeof(glm::uvec4));
	//glm::uvec4 * data = new glm::uvec4[count];
	GLuint channels = 4;
	GLuint byteSizePerChannel = 4;
	memset(data, 0, width * height * channels * byteSizePerChannel * 3);
	std::cout << "malloc" << std::endl;
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_3_axis_id);
	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, data);
	std::cout << "mapping" << std::endl;

	unsigned int counter = 0;
	for (unsigned int g = 0; g < 3; g++) {
		int occupied_voxels = 0;
		for (unsigned int w = 0; w < width; ++w)
		{
			for (unsigned int h = 0; h < height; ++h)
			{
				unsigned long index = g * width * height + h * width + w;
				glm::uvec4 compressed_uint = data[index];

				//std::cout << " X " << compressed_uint.x << std::endl;
				//std::cout << " Y " << compressed_uint.y << std::endl;
				//std::cout << " Z " << compressed_uint.z << std::endl;
				//std::cout << " W " << compressed_uint.w << std::endl;
				bool occupied_slice = false;
				for (unsigned int i = 0; i < 4; ++i)
					occupied_slice |= compressed_uint[i] > 0u;

				if (!occupied_slice) continue;

				for (unsigned int d = 0; d < depth; ++d)
				{
					glm::uint bitPosIndex = d / 32;

					glm::uvec4 bitPos = glm::uvec4(0u);
					bitPos[bitPosIndex] = 1u << (d % 32);

					bool occupied_voxel = (compressed_uint[bitPosIndex] & bitPos[bitPosIndex]) > 0u;

					if (occupied_voxel)
					{
						occupied_voxels++;
					}
				}
			}
		}
		printf("Occupied Voxels %d : %d\n", g, occupied_voxels);
	}
	boost::alignment::aligned_free(data);
	//delete[] data;
}

void nxVoxelizer::GeneratePreviewGrid() {
	GLuint width = m_dimensions.x;
	GLuint height = m_dimensions.y;
	GLuint depth = m_dimensions.z;
	GLuint voxel_grid_size = width * height * depth;

	GLuint count = width * height * 3;
	int f = sizeof(glm::uvec4);
	glm::uvec4 * data = (glm::uvec4 *)boost::alignment::aligned_alloc(16, count * sizeof(glm::uvec4));
	//glm::uvec4 * data = new glm::uvec4[count];
	GLuint channels = 4;
	GLuint byteSizePerChannel = 4;
	memset(data, 0, width * height * channels * byteSizePerChannel * 3);
	std::cout << "malloc" << std::endl;
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_3_axis_id);
	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, data);
	std::cout << "mapping" << std::endl;

	unsigned int counter = 0;
	for (unsigned int g = 0; g < 3; g++) {
		int occupied_voxels = 0;
		for (unsigned int w = 0; w < width; ++w)
		{
			for (unsigned int h = 0; h < height; ++h)
			{
				unsigned long index = g * width * height + h * width + w;
				glm::uvec4 compressed_uint = data[index];

				bool occupied_slice = false;
				for (unsigned int i = 0; i < 4; ++i)
					occupied_slice |= compressed_uint[i] > 0u;

				if (!occupied_slice) continue;

				for (unsigned int d = 0; d < depth; ++d)
				{
					glm::uint bitPosIndex = d / 32;

					glm::uvec4 bitPos = glm::uvec4(0u);
					bitPos[bitPosIndex] = 1u << (d % 32);

					bool occupied_voxel = (compressed_uint[bitPosIndex] & bitPos[bitPosIndex]) > 0u;

					if (occupied_voxel)
					{
						occupied_voxels++;
					}
				}
			}
		}
		printf("Occupied Voxels %d : %d\n", g, occupied_voxels);
	}
	boost::alignment::aligned_free(data);
	//delete[] data;
}

void nxVoxelizer::CalculateViewProjection() {
	//m_view_proj_axis[0] = m_proj_axis[0] * m_view_axis[0];
	//m_view_proj_axis[1] = m_proj_axis[1] * m_view_axis[1];
	//m_view_proj_axis[2] = m_proj_axis[2] * m_view_axis[2];
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

	BOOST_LOG_TRIVIAL(info) << "\n\n Max " << bmax.x << " " << bmax.y << " " << bmax.z << " " << "\n\n ";
	BOOST_LOG_TRIVIAL(info) << "\n\n Min " << bmin.x << " " << bmin.y << " " << bmin.z << " " << "\n\n ";
	BOOST_LOG_TRIVIAL(info) << "\n\n Center " << center.x << " " << center.y << " " << center.z << " " << "\n\n ";
	BOOST_LOG_TRIVIAL(info) << "Initializing Voxelizer ";

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