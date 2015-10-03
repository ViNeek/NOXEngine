//----------------------------------------------------//
// File:  Voxelizer.cpp                               //
//                                                    //
// Description: Voxelizer Source File                 //
//                                                    //
// Authors:                                           //
// Kostas Vardis                                      //
// Date: 20-Jun-2013                                  //
//                                                    //
// This is a free deferred renderer. The library  and //
// the source code are free. If you use this code as  //
// is or any part of it in any kind of project or     //
// product, please acknowledge the source and its	  //
// authors.		    								  //
//                                                    //
// For manuals, help and instructions, please visit:  //
// http://graphics.cs.aueb.gr/graphics/               //
//                                                    //
//----------------------------------------------------//

// includes ////////////////////////////////////////
#include "VoxelizerBase.h"

// defines /////////////////////////////////////////

//namespace DEFERREDRENDERER
//{
	// Constructor
	nxVoxelizerBase::nxVoxelizerBase(const char* name) :
		m_name(name),
		m_dimensions(128),
		m_voxel_size(1),
		m_voxel_grid_size(128 * 128 * 128),
		m_interior_type(VIT_NONE),
		m_accuracy_type(VA_NONE),
		m_output_type(VOT_NONE),
		m_type(VT_NONE),
#pragma message("TODO: REMOVE (vao should be set externally)")
		m_vao(0),
		//m_box(),
		m_initialized(false),
		m_currentShader(nullptr),
		m_uniform_voxel_size(true),
		m_shaderList(),
		m_resolution(128),
		m_write_file(false)
	{

	}

	// Destructor
	nxVoxelizerBase::~nxVoxelizerBase()
	{

	}

	// other functions
	bool nxVoxelizerBase::Init()
	{
		/*
		if (m_vao < 1)
		{
			// create quad mesh, vbo and vao
			glm::vec2 m_size(1, 1);
			float* vertexdata = new float[20];
			int index = -1;

			glm::vec3 m_pos = glm::vec3(-m_size.x, m_size.y, 0.0f);	vertexdata[++index] = m_pos.x; vertexdata[++index] = m_pos.y; vertexdata[++index] = m_pos.z;
			glm::vec2 m_tex_uv = glm::vec2(0.0f, 1.0f);				vertexdata[++index] = m_tex_uv.x; vertexdata[++index] = m_tex_uv.y;
			m_pos = glm::vec3(-m_size.x, -m_size.y, 0.0f);		vertexdata[++index] = m_pos.x; vertexdata[++index] = m_pos.y; vertexdata[++index] = m_pos.z;
			m_tex_uv = glm::vec2(0.0f, 0.0f);						vertexdata[++index] = m_tex_uv.x; vertexdata[++index] = m_tex_uv.y;
			m_pos = glm::vec3(m_size.x, -m_size.y, 0.0f);			vertexdata[++index] = m_pos.x; vertexdata[++index] = m_pos.y; vertexdata[++index] = m_pos.z;
			m_tex_uv = glm::vec2(1.0f, 0.0f);						vertexdata[++index] = m_tex_uv.x; vertexdata[++index] = m_tex_uv.y;
			m_pos = glm::vec3(m_size.x, m_size.y, 0.0f);			vertexdata[++index] = m_pos.x; vertexdata[++index] = m_pos.y; vertexdata[++index] = m_pos.z;
			m_tex_uv = glm::vec2(1.0f, 1.0f);						vertexdata[++index] = m_tex_uv.x; vertexdata[++index] = m_tex_uv.y;

			int stride = 5 * sizeof(float);
			int size = 4 * stride;

			GLuint err = -1;
			glGenVertexArrays(1, &m_vao);
			glBindVertexArray(m_vao);

			GLuint m_vbo = 0;
			glGenBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferData(GL_ARRAY_BUFFER, size, vertexdata, GL_STATIC_DRAW);

			GLubyte indices[] = { 0, 1, 2, 2, 3, 0 };
			GLuint m_ibo = 0;
			glGenBuffers(1, &m_ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
			glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glBindVertexArray(0);
			err = glGetError();
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			XE_SAFE_DELETE_ARRAY_POINTER(vertexdata)
		}
		*/
		return true;
		
	}

	void nxVoxelizerBase::Reset()
	{
		m_initialized = false;
	}

	void nxVoxelizerBase::WriteCube(FILE* fp, glm::vec3& pos, glm::vec3& size, nxUInt32& counter)
	{
		/*
		glm::vec3 cube[8];
		glm::vec4 cube_indices_quad[6];
		//front
		// bl
		cube[0] = pos + glm::vec3(-size.x, -size.y, size.z);
		// br
		cube[1] = pos + glm::vec3(size.x, -size.y, size.z);
		// tl
		cube[2] = pos + glm::vec3(-size.x, size.y, size.z);
		// tr
		cube[3] = pos + glm::vec3(size.x, size.y, size.z);
		// back
		// bl
		cube[4] = pos + glm::vec3(-size.x, -size.y, -size.z);
		// br
		cube[5] = pos + glm::vec3(size.x, -size.y, -size.z);
		// tl
		cube[6] = pos + glm::vec3(-size.x, size.y, -size.z);
		// tr
		cube[7] = pos + glm::vec3(size.x, size.y, -size.z);

		// front face
		cube_indices_quad[0] = glm::vec4(0, 1, 3, 2);
		//back face
		cube_indices_quad[1] = glm::vec4(5, 4, 6, 7);
		//right face
		cube_indices_quad[2] = glm::vec4(1, 5, 7, 3);
		//left face
		cube_indices_quad[3] = glm::vec4(4, 0, 2, 6);
		//top face
		cube_indices_quad[4] = glm::vec4(2, 3, 7, 6);
		//bottom face
		cube_indices_quad[5] = glm::vec4(4, 5, 1, 0);

		// write vertices
		for (unsigned int i = 0; i<8; ++i)
			fprintf(fp, "v %.5f %.5f %.5f\n", cube[i].x, cube[i].y, cube[i].z);

		fprintf(fp, "#8 vertices\n");
		fprintf(fp, "\n");
		fprintf(fp, "g\n");
		// obj expects the face counter to start at 1 instead of 0
		int obj_face_offset = 1;
		for (unsigned int i = 0; i<6; ++i)
			fprintf(fp, "f %i %i %i %i\n",
			counter + obj_face_offset + (int)(cube_indices_quad[i].x),
			counter + obj_face_offset + (int)(cube_indices_quad[i].y),
			counter + obj_face_offset + (int)(cube_indices_quad[i].z),
			counter + obj_face_offset + (int)(cube_indices_quad[i].w));
		fprintf(fp, "#6 faces\n");
		fprintf(fp, "\n");
		counter += 8;
		*/
	}

	void nxVoxelizerBase::WriteCubeVertices(FILE* fp, glm::vec3& pos, glm::vec3& size, nxUInt32& counter)
	{
		/*
		glm::vec3 cube[8];
		//front
		// bl
		cube[0] = pos + glm::vec3(-size.x, -size.y, size.z);
		// br
		cube[1] = pos + glm::vec3(size.x, -size.y, size.z);
		// tl
		cube[2] = pos + glm::vec3(-size.x, size.y, size.z);
		// tr
		cube[3] = pos + glm::vec3(size.x, size.y, size.z);
		// back
		// bl
		cube[4] = pos + glm::vec3(-size.x, -size.y, -size.z);
		// br
		cube[5] = pos + glm::vec3(size.x, -size.y, -size.z);
		// tl
		cube[6] = pos + glm::vec3(-size.x, size.y, -size.z);
		// tr
		cube[7] = pos + glm::vec3(size.x, size.y, -size.z);

		// write vertices
		for (unsigned int i = 0; i<8; ++i)
			fprintf(fp, "v %.5f %.5f %.5f\n", cube[i].x, cube[i].y, cube[i].z);

		counter += 8;
		*/
	}

	void nxVoxelizerBase::WriteCubeIndices(FILE* fp, const char* group_name, nxUInt32 counter)
	{
		/*
		glm::vec4 cube_indices_quad[6];

		// front face
		cube_indices_quad[0] = glm::vec4(0, 1, 3, 2);
		//back face
		cube_indices_quad[1] = glm::vec4(5, 4, 6, 7);
		//right face
		cube_indices_quad[2] = glm::vec4(1, 5, 7, 3);
		//left face
		cube_indices_quad[3] = glm::vec4(4, 0, 2, 6);
		//top face
		cube_indices_quad[4] = glm::vec4(2, 3, 7, 6);
		//bottom face
		cube_indices_quad[5] = glm::vec4(4, 5, 1, 0);

		fprintf(fp, "#%d vertices\n", counter);
		fprintf(fp, "\n");

		fprintf(fp, "#vn 0.0000 0.0000 1.0000\n");
		fprintf(fp, "#vn 0.0000 0.0000 -1.0000\n");
		fprintf(fp, "#vn 1.0000 0.0000 -0.0000\n");
		fprintf(fp, "#vn -1.0000 0.0000 -0.0000\n");
		fprintf(fp, "#vn 0.0000 1.0000 -0.0000\n");
		fprintf(fp, "#vn 0.0000 -1.0000 -0.0000\n");
		fprintf(fp, "## 6 vertex normals\n");
		fprintf(fp, "\n");

		fprintf(fp, "g %s\n", group_name);

		// obj expects the face counter to start at 1 instead of 0
		int32 obj_face_offset = 1;

		uint32 total_cubes = counter / 8;
		X_ASSERT_MSG(counter % 8 == 0, "Error in writing voxelizer indices to file.");

		uint32 total_faces = total_cubes * 6;
		for (uint32 i = 0; i < total_cubes; ++i)
		{
			int32 current_offset = i * 8 + obj_face_offset;
			for (uint32 j = 0; j < 6; ++j)
			{
				fprintf(fp, "f %i//%i %i//%i %i//%i %i//%i\n",
					current_offset + (int)(cube_indices_quad[j].x), (j + 1),
					current_offset + (int)(cube_indices_quad[j].y), (j + 1),
					current_offset + (int)(cube_indices_quad[j].z), (j + 1),
					current_offset + (int)(cube_indices_quad[j].w), (j + 1));
			}
		}

		fprintf(fp, "#%d polygons\n", total_faces);
		fprintf(fp, "\n");
		*/
	}

//} // namespace DEFERREDRENDERER

// eof ///////////////////////////////// class Voxelizer