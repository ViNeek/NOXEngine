#pragma once

#include <string>

class nxRenderer;
class nxScheduler;
class nxScene;
class nxSynchronizer;

class nxEngine {
public:

							nxEngine();

	nxRenderer*				Renderer() { return m_pRenderer; };
	nxScheduler*			Scheduler() { return m_pScheduler; };
	nxScene*				Scene() { return m_Scene; }
	
	void					InitRenderer();
	void					InitScheduler();
	void					InitScene(std::string& path);

	void					Start() { InitRenderer(); InitScheduler(); }

	bool					IsRendererFinished() { return m_RendererFinished; };
	bool					IsSchedulerFinished() { return m_SchedulerFinished; };
	void					SetRendererFinished(bool val) { m_RendererFinished = val; }
	void					SetSchedulerFinished(bool val) { m_SchedulerFinished = val; }

private:

	nxRenderer*				m_pRenderer;
	bool					m_RendererFinished;

	nxSynchronizer*			m_SchedulerSync;
	nxScheduler*			m_pScheduler;
	bool					m_SchedulerFinished;

	nxScene*				m_Scene;

};