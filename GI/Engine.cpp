#include <boost/align/aligned_alloc.hpp>

#include "Engine.h"
#include "Renderer.h"
#include "Scene.h"
#include "Scheduler.h"
#include "ResourceManager.h"

nxEngine::nxEngine() {
	m_RendererFinished = false;
	m_SchedulerFinished = false;

	m_Scene = new nxScene(this);
	//m_Scene = (nxScene*)boost::alignment::aligned_alloc(16, sizeof(nxScene));
	assert(m_Scene);
	m_pScheduler = new nxScheduler(this);
	m_pRenderer = new nxRenderer(this);
	m_ResourceMgr = new nxResourceManager;
}

void nxEngine::InitRenderer() {
	m_pRenderer->Create();
	m_pRenderer->Run();
}

void nxEngine::InitScheduler() {
	m_pScheduler->Create();
	m_pScheduler->Run();
}

void nxEngine::InitScene(std::string& path) {
	m_Scene->SetFilename(path);
}