#include "Scene.h"

#include "Engine.h"
#include "Renderer.h"
#include "Entity.h"
#include "Job.h"
#include "JobFactory.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>

#include "Scheduler.h"

namespace pt = boost::property_tree;

nxScene::nxScene(nxEngine* eng) {
	m_pEngine = eng;
}

nxScene::nxScene(std::string& path) {
	m_SceneFilename = path;
}

void nxScene::Init() {
	try {
		// Create empty property tree object
		pt::ptree tree;
		BOOST_LOG_TRIVIAL(info) << "SceneCount : " << m_SceneFilename;

		// Parse the XML into the property tree.
		pt::read_json(m_SceneFilename, tree);

		// Use the default-value version of get to find the debug level.
		// Note that the default value is used to deduce the target type.
		m_EntitiesCount = tree.get("Scene.count", 0);
		m_SceneName = tree.get<std::string>("Scene.name", "unknown");

		BOOST_LOG_TRIVIAL(info) << "SceneCount : " << m_EntitiesCount;
		BOOST_LOG_TRIVIAL(info) << "Scene Name : " << m_SceneName;

		// Use get_child to find the node containing the modules, and iterate over
		// its children. If the path cannot be resolved, get_child throws.
		// A C++11 for-range loop would also work.
		BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("Scene.shaders")) {
			// The data function is used to access the data stored in a node.
			BOOST_FOREACH(pt::ptree::value_type &v1, v.second) {
				std::string* shaderPath = new std::string(v1.second.data());
				m_pEngine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_LOAD_SHADER, shaderPath));
				BOOST_LOG_TRIVIAL(info) << "Shader : " << v1.second.data();
			}
			//m_modules.insert(v.second.data());
			}
		/*
		BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("Scene.entities")) {
			// The data function is used to access the data stored in a node.
			BOOST_FOREACH(pt::ptree::value_type &v, v.second.get_child("f")) {
			}
			BOOST_LOG_TRIVIAL(info) << "Entity : " << v.second.data().at(0);
			//m_modules.insert(v.second.data());
		}
		*/
	} catch (std::exception const& e)
	{
		BOOST_LOG_TRIVIAL(error) << "Scene parse error : " << e.what();
	}
}

void nxScene::PushEntity(nxEntity* ent) {

}

bool nxSceneLoader::operator()(void* data) {
	nxScene* scene = NOX_ENGINE_GET(Scene);

	scene->Init();

	return true;
}