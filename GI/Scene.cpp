#include <unordered_map>
#include <string>
#include <set>
#include <exception>
#include <iostream>

#include "Scene.h"

#include "GLUtils.h"
#include "Engine.h"

#include "Renderer.h"
#include "Entity.h"
#include "Job.h"
#include "Program.h"
#include "JobFactory.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/log/trivial.hpp>
#include <boost/assign/list_of.hpp> // for 'map_list_of()'

#include "Scheduler.h"
#include "Camera.h"
#include "Voxelizer.h"
#include "ResourceManager.h"

#include <type_traits>

namespace pt = boost::property_tree;

nxScene::nxScene(nxEngine* eng) {
	m_pEngine = eng;
	m_Camera = NULL;

	m_GMaxX = m_GMaxY = m_GMaxZ = -100000.0f;
	m_GMinX = m_GMinY = m_GMinZ = 100000.0f;

}

nxScene::nxScene(std::string& path) {
	m_SceneFilename = path;
	m_Camera = NULL;
}

std::unordered_map<std::string, GLenum> gc_TypeMappings =
boost::assign::map_list_of("Compute", GL_COMPUTE_SHADER)
("Vertex", GL_VERTEX_SHADER)
("Tess_control", GL_TESS_CONTROL_SHADER)
("Tess_evaluation", GL_TESS_EVALUATION_SHADER)
("Geometry", GL_GEOMETRY_SHADER)
("Fragment", GL_FRAGMENT_SHADER);

void nxScene::Init() {
	try {
		// Create empty property tree object
		pt::ptree tree;
		BOOST_LOG_TRIVIAL(info) << "SceneCount : " << m_SceneFilename;

		// Parse the XML into the property tree.
		pt::read_json(m_SceneFilename, tree);

		// Use the default-value version of get to find the debug level.
		// Note that the default value is used to deduce the target type.
		m_EntitiesCount = tree.get("Scene.Count", 0);
		m_SceneName = tree.get<std::string>("Scene.Name", "unknown");
		
		std::string m_DefaultProgramName = tree.get<std::string>("Scene.Default Program", "Unknown");

		BOOST_LOG_TRIVIAL(info) << "SceneCount : " << m_EntitiesCount;
		BOOST_LOG_TRIVIAL(info) << "Scene Name : " << m_SceneName;

		// Use get_child to find the node containing the modules, and iterate over
		// its children. If the path cannot be resolved, get_child throws.
		// A C++11 for-range loop would also work.
		BOOST_LOG_TRIVIAL(info) << "Number of Program Shaders : " << tree.get_child("Scene.Programs").size();
		BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("Scene.Programs")) {
			nxProgram* prog = new nxProgram(v.second.get_child("Shaders").size());
			//static_assert(nox::interfaces::IsResource<int>::value);
			m_pEngine->ResourceManager()->AddResource(prog);
			//m_pEngine->ResourceManager()->AddResource(new std::string("s"));

			prog->SetName(v.second.get<std::string>("Name"));
			bool use = false;
			if (std::strcmp(m_DefaultProgramName.c_str(), v.second.get<std::string>("Name").c_str()) == 0)
				use = true;
			BOOST_FOREACH(pt::ptree::value_type &v1, v.second.get_child("Shaders")) {
				BOOST_FOREACH(pt::ptree::value_type &v2, v1.second) {
					nxShaderLoaderBlob* data = new nxShaderLoaderBlob(m_pEngine, prog, v2.second.data(), gc_TypeMappings.at(v2.first.data()), use);
					m_pEngine->Scheduler()->ScheduleJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_LOAD_SHADER, data));
					BOOST_LOG_TRIVIAL(info) << "Shader : " << v2.second.data();
				}
			}
		}
		
		BOOST_LOG_TRIVIAL(info) << "Number of Entities : " << tree.get_child("Scene.Entities").size();
		BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("Scene.Entities")) {
			BOOST_LOG_TRIVIAL(info) << "STUFF : " << v.second.get("ModelName", "unknkown");
			BOOST_LOG_TRIVIAL(info) << "Camera X position : " << v.second.get<float>("CenterX", 0.0f);
			BOOST_LOG_TRIVIAL(info) << "Camera Y position : " << v.second.get<float>("CenterY", 0.0f);
			BOOST_LOG_TRIVIAL(info) << "Camera Z position : " << v.second.get<float>("CenterZ", 0.0f);
			nxAssetLoaderBlob* data = new nxAssetLoaderBlob(
				m_pEngine,
				v.second.get("ModelName", "unknkown"),
				v.second.get("ModelType", "unknkown"),
				glm::vec3(v.second.get<float>("CenterX", 0.0f),
						  v.second.get<float>("CenterY", 0.0f),
						  v.second.get<float>("CenterZ", 0.0f)),
						  v.second.get<float>("Scale", 1.0f));
			
			m_pEngine->Scheduler()->ScheduleJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_LOAD_ASSET, data));
		}

		m_Camera = new nxArcballCamera();
		m_Camera->SetPosition(tree.get_child("Scene.Camera").get<float>("PositionX", 0.0f),
			tree.get_child("Scene.Camera").get<float>("PositionY", 0.0f),
			tree.get_child("Scene.Camera").get<float>("PositionZ", 0.0f));

		glm::uvec3 dimensions(tree.get_child("Scene.Voxelizer").get<int>("DimX", 128),
			tree.get_child("Scene.Voxelizer").get<int>("DimY", 128),
			tree.get_child("Scene.Voxelizer").get<int>("DimZ", 128));

		nxVoxelizer* voxel = new nxVoxelizer(m_pEngine, dimensions.x);
		m_pEngine->Renderer()->SetVoxelizer(voxel);

		nxVoxelizerInitializerBlob* voxelData = new nxVoxelizerInitializerBlob(m_pEngine, dimensions);
		m_pEngine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_VOXELIZER_INIT, voxelData));

		SetProjection(45.0f, (float)m_pEngine->Renderer()->Width() / m_pEngine->Renderer()->Height(), 1.0f, 1000.0f);
	
	} catch (std::exception const& e)
	{
		BOOST_LOG_TRIVIAL(error) << "Scene parse error : " << e.what();
	}
}

glm::mat3& nxScene::Normal() {
	m_MState.m_NMatrix = glm::mat3(m_MState.m_MMatrix);
	m_MState.m_NMatrix = glm::inverse(m_MState.m_NMatrix);
	m_MState.m_NMatrix = glm::transpose(m_MState.m_NMatrix);

	return m_MState.m_NMatrix;
}

bool errorGL = true;
void nxScene::Draw() {
	if (CameraReady())
		m_MState.m_RMatrix = Camera()->Update();
	else
		m_MState.m_RMatrix = glm::mat4();

	m_MState.m_VMatrix = glm::mat4();

	m_pEngine->Renderer()->UseProgram();
	if (errorGL) Utils::GL::CheckGLState("Program USE");

	for (size_t i = 0; i < m_Entities.size(); i++) {
		m_MState.m_VMatrix = glm::translate(View(),
			m_Camera->Position());

		m_MState.m_VMatrix = glm::translate(View(), m_Entities[i]->ModelTransform());
		m_MState.m_VMatrix *= m_MState.m_RMatrix;

		//m_MState.m_MMatrix = glm::translate(View(), m_Entities[i]->ModelTransform());
		m_pEngine->Renderer()->Program()->SetUniform("NormalMatrix", Normal());
		if (errorGL) Utils::GL::CheckGLState("Set Normal");

		m_pEngine->Renderer()->Program()->SetUniform("MVP", m_MState.m_PMatrix*m_MState.m_VMatrix);
		//m_pEngine->Renderer()->Program()->SetUniform("MVP", View());
		if (errorGL) Utils::GL::CheckGLState("Set MVP");

		m_Entities[i]->Draw();
		if (errorGL) Utils::GL::CheckGLState("Draw : " + i);

	}

	errorGL = false;
}

void nxScene::DrawVoxelized() {
	if (CameraReady())
		m_MState.m_RMatrix = Camera()->Update();
	else
		m_MState.m_RMatrix = glm::mat4();

	m_MState.m_VMatrix = glm::mat4();

	m_pEngine->Renderer()->SetActiveProgramByName("Voxelize");
	m_pEngine->Renderer()->UseProgram();
	if (errorGL) Utils::GL::CheckGLState("Program USE");

	if (m_pEngine->Renderer()->VoxelizerReady()) {
		//m_MState.m_PMatrix = m_pEngine->Renderer()->Voxelizer()->Projections()[0];
		//m_pEngine->Renderer()->Voxelizer()->CalculateViewProjection();
		glViewportArrayv(0, 3, &m_pEngine->Renderer()->Voxelizer()->Viewports()[0][0]);
	}

	for (size_t i = 0; i < m_Entities.size(); i++) {
		if (!m_pEngine->Renderer()->VoxelizerReady())
			//m_MState.m_VMatrix = m_pEngine->Renderer()->Voxelizer()->Views()[0];
		//else
			m_MState.m_VMatrix = glm::translate(View(),
				m_Camera->Position());

		m_MState.m_VMatrix = glm::translate(View(), m_Entities[i]->ModelTransform());
		//m_MState.m_VMatrix *= m_MState.m_RMatrix;

		// = glm::translate(View(), m_Entities[i]->ModelTransform());
		//m_pEngine->Renderer()->Program()->SetUniform("NormalMatrix", Normal());
		m_pEngine->Renderer()->Program()->SetUniform("uniform_model", View());
		m_pEngine->Renderer()->Program()->SetUniform("uniform_size", m_pEngine->Renderer()->Voxelizer()->Dimesions());
		if (errorGL) Utils::GL::CheckGLState("Set Normal");

		m_pEngine->Renderer()->Program()->SetUniform("uniform_view_proj", 3, m_pEngine->Renderer()->Voxelizer()->ViewProjections());
		//m_pEngine->Renderer()->Program()->SetUniform("MVP", View());
		if (errorGL) Utils::GL::CheckGLState("Set MVP");

		m_Entities[i]->Draw();
		if (errorGL) Utils::GL::CheckGLState("Draw : " + i);

	}

	if (m_pEngine->Renderer()->VoxelizerReady()) 
		m_pEngine->Renderer()->Voxelizer()->PrintGrid();

	errorGL = false;
}

void nxScene::UpdateBounds(nxEntity* ent) {

	if (ent->MinX() < m_GMinX) m_GMinX = ent->MinX();
	if (ent->MinY() < m_GMinY) m_GMinY = ent->MinY();
	if (ent->MinZ() < m_GMinZ) m_GMinZ = ent->MinZ();
	if (ent->MaxX() > m_GMaxX) m_GMaxX = ent->MaxX();
	if (ent->MaxY() > m_GMaxY) m_GMaxY = ent->MaxY();
	if (ent->MaxZ() > m_GMaxZ) m_GMaxZ = ent->MaxZ();

}

void nxScene::SetProjection(float angle, float fov, float zNear, float zFar) {
	m_MState.m_PMatrix = glm::perspective(angle, fov, zNear, zFar);
}

void nxScene::SetProjection(glm::mat4& proj) {
	m_MState.m_PMatrix = proj;
}

bool nxSceneLoader::operator()(void* data) {
	nxScene* scene = NOX_ENGINE_GET(Scene);

	scene->Init();

	return true;
}