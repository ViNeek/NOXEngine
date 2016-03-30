#include <boost/log/trivial.hpp>
#include <boost/multi_array.hpp>

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
	m_CaptureGrid = false;
	m_resolution = dim;
	m_ssbo = -1;
	m_DummyLayeredBuffer = -1;
}

nxVoxelizer::nxVoxelizer(nxEngine* eng, nxUInt32 dimX, nxUInt32 dimY, nxUInt32 dimZ) : nxVoxelizerBase("dumyy") {
	m_pEngine = eng;
	m_dimensions = glm::uvec3(dimX, dimY, dimZ);
	m_initialized = false;
	m_CaptureGrid = false;
	m_resolution = dimX;
	m_ssbo = -1;
	m_DummyLayeredBuffer = -1;
}

static const int g_WorkGroupSize = 4;
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
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_resolution * m_resolution * m_resolution * 4, NULL, GL_DYNAMIC_COPY);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, m_resolution * m_resolution * m_resolution / 8, NULL, GL_DYNAMIC_COPY);

	int l_InvocationCount = m_resolution * m_resolution * m_resolution;
	glm::uvec3 l_GroupSize(
		(m_resolution - 1) / g_WorkGroupSize + 1,
		(m_resolution - 1) / g_WorkGroupSize + 1,
		(m_resolution - 1) / g_WorkGroupSize + 1
	);
	glm::uvec3 l_OldGroupSize(
		m_resolution / g_WorkGroupSize,
		m_resolution / g_WorkGroupSize,
		m_resolution / g_WorkGroupSize
	);

	BOOST_LOG_TRIVIAL(info) << "\n\n\n\n\nDims : " << l_GroupSize.x << "\n\n\n\n";
	BOOST_LOG_TRIVIAL(info) << "\n\n\n\n\nDims : " << l_OldGroupSize.x << "\n\n\n\n";

	//glDispatchComputeGroupSize( NUMGROUPS, 1, 1, 
	//							WORK_GROUP_SIZE, 1, 1);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//int l_GroupSize = g_WorkGroupSize * g_WorkGroupSize * g_WorkGroupSize;
	int l_GroupCount = (l_InvocationCount - 1) / ( l_GroupSize.x * l_GroupSize.y * l_GroupSize.z ) + 1;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	BOOST_LOG_TRIVIAL(info) << "\n\n\n\n\nDims : " << m_resolution << "\n\n\n\n";

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
	m_view_proj_axis[0] = m_proj_axis[0] * m_view_axis[0];
	m_view_proj_axis[1] = m_proj_axis[1] * m_view_axis[1];
	m_view_proj_axis[2] = m_proj_axis[2] * m_view_axis[2];
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

void nxVoxelizer::PrintGridMeshF(GLuint ssbo) {
	FILE* fp = fopen("voxels.asc", "w");
	glm::vec3 size = GridSize();
	glm::vec3 half_size = size * 0.5f;
	glm::vec3 voxel = size / glm::vec3(m_dimensions);

	printf("Grid Size %f %f %f\n", size.x, size.y, size.z);
	printf("Voxel Size %f %f %f\n", voxel.x, voxel.y, voxel.z);
	printf("Dims %d %d %d\n", m_dimensions.x, m_dimensions.y, m_dimensions.z);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	nxFloat32* p = (nxFloat32*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	//if (error) Utils::GL::CheckGLState("Frame");
	int countVoxels = 0;
	int xCount = 0;
	int yCount = 0;
	int zCount = 0;
	if (p) {
		typedef boost::multi_array_ref<nxFloat32, 3> array_type;
		typedef array_type::index index;
		array_type ip(p, boost::extents[m_dimensions.x][m_dimensions.y][m_dimensions.z]);

		for (int i = 0; i < m_dimensions.x; i++) {
			for (int j = 0; j < m_dimensions.y; j++) {
				for (int k = 0; k < m_dimensions.z; k++) {
					if (ip[i][j][k] >= 0 && ip[i][j][k] < 1) {
						countVoxels++;
						xCount++;
						yCount++;
						zCount++;
						fprintf(fp, "%g ", i * voxel.x);
						fprintf(fp, "%g ", j * voxel.y);
						fprintf(fp, "%g\n", k * voxel.z);
					}
				}
			}
		}

		printf("Voxel Count %d\n", countVoxels);
		printf("X Count %d\n", xCount);
		printf("Y Count %d\n", yCount);
		printf("Z Count %d\n", zCount);

		fclose(fp);

		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT 1 : " << ( ip[0][0][0] );
		//if (error) Utils::GL::CheckGLState("Frame");
		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT 2 : " << ( ip[127][127][127]);
		//if (error) Utils::GL::CheckGLState("Frame");
		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT 3 : " << ( ip[1][0][0]);
		//if (error) Utils::GL::CheckGLState("Frame");
		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT 4 : " << ( ip[0][0][0]);
		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT : " << "done";
		//if (error) Utils::GL::CheckGLState("Frame");
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void nxVoxelizer::PrintGridMesh(GLuint ssbo) {
	FILE* fp = fopen("voxels.asc", "w");
	glm::vec3 size = GridSize();
	glm::vec3 half_size = size * 0.5f;
	glm::vec3 voxel = size / glm::vec3( m_dimensions );

	printf("Grid Size %f %f %f\n", size.x, size.y, size.z);
	printf("Voxel Size %f %f %f\n", voxel.x, voxel.y, voxel.z);
	printf("Dims %d %d %d\n", m_dimensions.x, m_dimensions.y, m_dimensions.z);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	nxUInt32* p = (nxUInt32*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	//if (error) Utils::GL::CheckGLState("Frame");
	int countVoxels = 0;
	int xCount = 0;
	int yCount = 0;
	int zCount = 0;
	if (p) {
		typedef boost::multi_array_ref<nxUInt32, 3> array_type;
		typedef array_type::index index;
		array_type ip(p, boost::extents[m_dimensions.x][m_dimensions.y][m_dimensions.z]);

		for (int i = 0; i < m_dimensions.x; i++) {
			for (int j = 0; j < m_dimensions.y; j++) {
				for (int k = 0; k < m_dimensions.z; k++) {
					printf("%d ", ip[i][j][k]);
					if (ip[i][j][k] == 1) {
						countVoxels++;
						xCount++;
						yCount++;
						zCount++;
						fprintf(fp, "%g ", i * voxel.x);
						fprintf(fp, "%g ", j * voxel.y);
						fprintf(fp, "%g\n", k * voxel.z);
					}
				}
			}
		}

		printf("Voxel Count %d\n", countVoxels);
		printf("X Count %d\n", xCount);
		printf("Y Count %d\n", yCount);
		printf("Z Count %d\n", zCount);

		fclose(fp);

		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT 1 : " << ( ip[0][0][0] );
		//if (error) Utils::GL::CheckGLState("Frame");
		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT 2 : " << ( ip[127][127][127]);
		//if (error) Utils::GL::CheckGLState("Frame");
		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT 3 : " << ( ip[1][0][0]);
		//if (error) Utils::GL::CheckGLState("Frame");
		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT 4 : " << ( ip[0][0][0]);
		//BOOST_LOG_TRIVIAL(info) << "PRINTING BINARY SHIT : " << "done";
		//if (error) Utils::GL::CheckGLState("Frame");
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void nxVoxelizer::SetMatrices() {

	glm::vec3 size = GridSize();
	glm::vec3 half_size = size * 0.5f;

	const glm::vec3& center = GridCenter();
	const glm::vec3 bmax = GridMax();
	const glm::vec3 bmin = GridMin();

	BOOST_LOG_TRIVIAL(info) << "\n\n Max " << bmax.x << " " << bmax.y << " " << bmax.z << " " << "\n\n ";
	BOOST_LOG_TRIVIAL(info) << "\n\n Min " << bmin.x << " " << bmin.y << " " << bmin.z << " " << "\n\n ";
	BOOST_LOG_TRIVIAL(info) << "\n\n Center " << center.x << " " << center.y << " " << center.z << " " << "\n\n ";
	BOOST_LOG_TRIVIAL(info) << "\n\n Half " << half_size.x << " " << half_size.y << " " << half_size.z << " " << "\n\n ";
	BOOST_LOG_TRIVIAL(info) << "\n\n Size " << size.x << " " << size.y << " " << size.z << " " << "\n\n ";
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