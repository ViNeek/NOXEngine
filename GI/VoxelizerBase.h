//----------------------------------------------------//
// File:  Voxelizer.h                                 //
//                                                    //
// Description: Voxelizer Header File                 //
//                                                    //
// Authors:                                           //
// Kostas Vardis                                      //
// Date: 20-Jun-2013                                  //
//                                                    //
// This is a free deferred renderer. The library  and //
// the source code are free. If you use this code as  //
// is or any part of it in any kind of project or     //
// product, please acknowledge the source and its	  //
// authors.			     							  //
//                                                    //
// For manuals, help and instructions, please visit:  //
// http://graphics.cs.aueb.gr/graphics/               //
//                                                    //
//----------------------------------------------------//
#ifndef VOXELIZER_H
#define VOXELIZER_H

#pragma once
//using namespace

// includes ////////////////////////////////////////
#include <string>
#include <vector>

// Math
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#include "CustomTypes.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////
class nxProgram;

// class declarations //////////////////////////////

//namespace DEFERREDRENDERER
//{
	enum VOXELIZER_OUTPUT_TYPE
	{
		VOT_BINARY,
		VOT_SCALAR,
		VOT_VECTOR,
		VOT_NONE
	};

	enum VOXELIZER_INTERIOR_TYPE
	{
		VIT_SURFACE,
		VIT_SOLID,
		VIT_NONE
	};

	enum VOXELIZER_ACCURACY
	{
		VA_BOUNDARY,
		VA_CONSERVATIVE,
		VA_NONE
	};

	enum VOXELIZER_TYPE
	{
		VT_THREE_WAY_BINARY,
		VT_SINGLE_VIEW_BINARY,
		VT_NONE
	};

	enum VOXELIZER_EXPORT_TYPE
	{
		VET_OBJ,
		VET_POINT_CLOUD,
		VET_ALL,
	};

	class nxVoxelizerBase
	{
	public:

		typedef std::vector<nxProgram*>	SHADER_LIST;

	protected:
		// protected variable declarations
		glm::ivec3							m_dimensions;
		nxFloat32							m_voxel_size;
		nxUInt32							m_voxel_grid_size;
		VOXELIZER_INTERIOR_TYPE				m_interior_type;
		VOXELIZER_ACCURACY					m_accuracy_type;
		VOXELIZER_OUTPUT_TYPE				m_output_type;
		VOXELIZER_TYPE						m_type;
		std::string							m_name;
		//box									m_box;
		bool								m_initialized;
		bool								m_uniform_voxel_size;
		nxUInt32							m_resolution;
		nxUInt32							m_max_resolution;

		SHADER_LIST							m_shaderList;
		nxProgram*							m_currentShader;
		bool								m_write_file;

		std::vector<std::string>			m_debug_text;

		// TODO: REMOVE (vao should be set externally)
		nxUInt32							m_vao;

		// protected function declarations

	private:
		// private variable declarations


		// private function declarations


	public:
		// Constructor
		nxVoxelizerBase(const char* name);

		// Destructor
		~nxVoxelizerBase(void);

		// public function declarations
		virtual void						Create(void)								{ ; }
		virtual void						Create(nxUInt32 resolution)					{ ; }
		virtual bool						Init(void);
		//virtual void						Update(box& _box)							{ ; }
		virtual void						Update(void)								{ ; }
		virtual void						Draw(class DRVisual& visual)				{ ; }
		virtual void						Reset(void);
		virtual void						Destroy(void)								{ ; }
		virtual void						SetVoxelizationOutput(bool status)			{ ; }

		static void							WriteCube(FILE* fp, glm::vec3& pos, glm::vec3& size, nxUInt32& counter);
		static void							WriteCubeVertices(FILE* fp, glm::vec3& pos, glm::vec3& size, nxUInt32& counter);
		static void							WriteCubeIndices(FILE* fp, const char* group_name, nxUInt32 counter);

		// get functions
		std::string&						GetName(void)								{ return m_name; }
		nxUInt32							GetWidth(void)								{ return m_dimensions.x; }
		nxUInt32							GetHeight(void)								{ return m_dimensions.y; }
		nxUInt32							GetDepth(void)								{ return m_dimensions.z; }
		//box									GetBBox(void)								{ return m_box; }
		VOXELIZER_INTERIOR_TYPE				GetInteriorType(void)						{ return m_interior_type; }
		VOXELIZER_ACCURACY					GetAccuracyType(void)						{ return m_accuracy_type; }
		VOXELIZER_OUTPUT_TYPE				GetOutputType(void)							{ return m_output_type; }
		VOXELIZER_TYPE						GetType(void)								{ return m_type; }
		glm::ivec3&							GetResolution(void)							{ return m_dimensions; }
		nxFloat32							GetVoxelSize(void)							{ return m_voxel_size; }

		// set functions
	};

//} // namespace DEFERREDRENDERER

#endif //VOXELIZER_H

// eof ///////////////////////////////// class Voxelizer