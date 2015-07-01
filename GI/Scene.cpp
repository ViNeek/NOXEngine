#include "Scene.h"

#include "Engine.h"
#include "Renderer.h"
#include "Entity.h"
#include "Job.h"
#include "Program.h"
#include "JobFactory.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/log/trivial.hpp>
#include <boost/assign/list_of.hpp> // for 'map_list_of()'

#include <map>
#include <string>
#include <set>
#include <exception>
#include <iostream>

#include "Scheduler.h"
#include "Camera.h"

namespace pt = boost::property_tree;

nxScene::nxScene(nxEngine* eng) {
	m_pEngine = eng;
}

nxScene::nxScene(std::string& path) {
	m_SceneFilename = path;
}

std::map<std::string, GLenum> gc_TypeMappings =
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

		BOOST_LOG_TRIVIAL(info) << "SceneCount : " << m_EntitiesCount;
		BOOST_LOG_TRIVIAL(info) << "Scene Name : " << m_SceneName;

		// Use get_child to find the node containing the modules, and iterate over
		// its children. If the path cannot be resolved, get_child throws.
		// A C++11 for-range loop would also work.
		BOOST_LOG_TRIVIAL(info) << "Program shader count : " << tree.get_child("Scene.Programs").size();
		BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("Scene.Programs")) {
			nxProgram* prog = new nxProgram(v.second.get_child("Shaders").size());
			BOOST_FOREACH(pt::ptree::value_type &v1, v.second.get_child("Shaders")) {
				BOOST_FOREACH(pt::ptree::value_type &v2, v1.second) {
					nxShaderLoaderBlob* data = new nxShaderLoaderBlob(m_pEngine, prog, v2.second.data(), gc_TypeMappings.at(v2.first.data()));
					m_pEngine->Scheduler()->ScheduleJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_LOAD_SHADER, data));
					BOOST_LOG_TRIVIAL(info) << "Shader : " << v2.second.data();
				}
			}
			//m_modules.insert(v.second.data());
		}
		
		BOOST_LOG_TRIVIAL(info) << "Entities : " << tree.get_child("Scene.Entities").size();
		BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("Scene.Entities")) {
			BOOST_LOG_TRIVIAL(info) << "STUFF : " << v.second.get("ModelName", "unknkown");
			nxAssetLoaderBlob* data = new nxAssetLoaderBlob(
				m_pEngine,
				v.second.get("ModelName", "unknkown"),
				v.second.get("ModelType", "unknkown"),
				glm::vec3(stof(v.second.get("CenterX", "0.0f")),
							stof(v.second.get("CenterY", "0.0f")),
							stof(v.second.get("CenterZ", "0.0f"))));

			m_pEngine->Scheduler()->ScheduleJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_LOAD_ASSET, data));
		}

		m_Camera = new nxArcballCamera();
		BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("Scene.Camera")) {
			m_Camera->SetPosition(stof(v.second.get("PositionX", "0.0f")),
				stof(v.second.get("PositionY", "0.0f")),
				stof(v.second.get("PositionZ", "0.0f")));
		}

		//SetProjection();
	} catch (std::exception const& e)
	{
		BOOST_LOG_TRIVIAL(error) << "Scene parse error : " << e.what();
	}
}

void nxScene::Draw() {
	for (size_t i = 0; i < m_Entities.size(); i++) {
		m_MState.m_MMatrix = glm::translate(View(), m_Entities[i]->ModelTransform());
		m_Entities[i]->Draw();
	}
}

void nxScene::SetProjection(float angle, float fov, float zNear, float zFar) {
	m_MState.m_PMatrix = glm::perspective(angle, fov, zNear, zFar);
}

bool nxSceneLoader::operator()(void* data) {
	nxScene* scene = NOX_ENGINE_GET(Scene);

	scene->Init();

	return true;
}