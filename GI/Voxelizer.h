#pragma once

#include <string>

#include <boost/align/align.hpp>
#include <boost/align/aligned_alloc.hpp>
#include <boost/align/is_aligned.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <GL/glew.h>

class nxEngine;

class nxVoxelizer {

public:

										nxVoxelizer(nxEngine* eng, unsigned int dim);

										void*						operator new(size_t i)
										{
											//void *p = _mm_malloc(i, 16);
											void *p = boost::alignment::aligned_alloc(16, i);
											assert(boost::alignment::is_aligned(16, p));
											return p;
										}

											void operator delete(void* p)
										{
											boost::alignment::aligned_free(p);
											//_mm_free(p);
										}

	void								Init();
	bool								IsInitialized() { return m_initialized; };
	
	glm::uvec3							Dimesions() { return m_dimensions; }
	void								SetMatrices();
	glm::mat4*							Views() { return m_view_axis; };
	glm::vec4*							Viewports() { return m_viewport; };
	glm::mat4*							Projections() { return m_proj_axis; };
	glm::mat4*							ViewProjections() { return m_view_proj_axis; };
	void								CalculateViewProjection();
	void								GeneratePreviewGrid();

	glm::vec3							GridSize();
	glm::vec3							GridCenter();
	glm::vec3							GridMax();
	glm::vec3							GridMin();

	GLuint								VoxelizerFramebuffer() { return m_fbo_3_axis; };

	void								PrintGrid();

private:

	nxEngine*							m_pEngine;

	GLuint								m_texture_3_axis_id;
	GLuint								m_fbo_3_axis;
	glm::uvec3							m_dimensions;
	float								m_voxel_size;
	unsigned int								m_voxel_grid_size;
	std::string							m_name;
	//box									m_box;
	bool								m_initialized;
	bool								m_uniform_voxel_size;
	unsigned int								m_resolution;
	unsigned int								m_max_resolution;

	glm::vec4							m_viewport[3];
	glm::mat4							m_view_proj_axis[3];
	glm::mat4							m_view_axis[3];
	glm::mat4							m_proj_axis[3];

};