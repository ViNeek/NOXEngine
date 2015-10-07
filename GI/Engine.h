#pragma once

#include <string>

// Forward declares
class nxRenderer;
class nxScheduler;
class nxScene;
class nxSynchronizer;
class nxResourceManager;

class nxEngine {
public:

							nxEngine();

	nxRenderer*				Renderer() { return m_pRenderer; };
	nxScheduler*			Scheduler() { return m_pScheduler; };
	nxScene*				Scene() { return m_Scene; }
	nxResourceManager*		ResourceManager() { return m_ResourceMgr; }

	void					InitRenderer();
	void					InitScheduler();
	void					InitScene(std::string& path);

	void					Start() { InitRenderer(); InitScheduler(); }

	bool					IsRendererFinished() { return m_RendererFinished; };
	bool					IsSchedulerFinished() { return m_SchedulerFinished; };
	void					SetRendererFinished(bool val) { m_RendererFinished = val; }
	void					SetSchedulerFinished(bool val) { m_SchedulerFinished = val; }

private:

	// Renderer State
	nxRenderer*				m_pRenderer;
	bool					m_RendererFinished;

	// Scheduler state
	nxSynchronizer*			m_SchedulerSync;
	nxScheduler*			m_pScheduler;
	bool					m_SchedulerFinished;

	// Scene State
	nxScene*				m_Scene;

	// Resource Manager
	nxResourceManager*		m_ResourceMgr;
};