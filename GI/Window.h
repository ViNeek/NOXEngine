#pragma once

#include "Utils.h"

#include <wx/wx.h>

#include <GL/glew.h>

#include <wx/glcanvas.h>

// Forward declares
class nxEngine;
class nxStatusBar;
class nxGLPanel;

/* GL Rendering Window */
class nxFrame : public wxFrame
{
public:

	/* Constructor. Creates a new window for gl rendering */
	nxFrame(const wxChar *title, int xpos, int ypos, int width, int height);

	bool					IsRendererFinished();
	bool					IsSchedulerFinished();

	nxEngine*				Engine() { return m_EngineState; }

	/*
	const nxRenderer*		Renderer() { return m_pRenderer; };
	const nxScheduler*		Scheduler() { return m_pScheduler; };
	const nxScene*			Scene() { return m_Scene; }
	*/

private:

	// UI Widgets
	wxMenuBar*				m_pMenuBar;
	wxMenu*					m_pPrefMenu;
	wxMenu*					m_pSceneMenu;
	wxMenu*					m_pShaderMenu;
	wxMenu*					m_pVoxelMenu;
	wxMenu*					m_pHelpMenu;

	// Custom UI widgets
	nxStatusBar*			m_pStatusBar;
	nxGLPanel*				m_pGLPanel;

	// NOX Engine State
	nxEngine*				m_EngineState;

	// Handlers
	void					OnClose(wxCloseEvent& evt);
	void					OnResize(wxSizeEvent& evt);
	void					OnRendererExit(wxCommandEvent& evt);
	void					OnSchedulerExit(wxCommandEvent& evt);
	void					OnLoadScene(wxCommandEvent& evt);

	// StatusBar
	virtual void			PositionStatusBar();
};

/* GL Panel */
class nxGLPanel : public wxGLCanvas
{
public:
							nxGLPanel(nxFrame* parent, int* args);

	// events
	void					OnLeftClickPressed(wxMouseEvent& evt);
	void					OnLeftClickReleased(wxMouseEvent& evt);
	void					OnMouseWheelRolled(wxMouseEvent& evt);
	void					OnMouseMoved(wxMouseEvent& evt);

private:

	// NOX Engine State
	nxEngine*				m_pEngine;

	wxGLContext*			m_pGLCtx;

};

/* Progress/Status Bar */
class nxStatusBar : public wxStatusBar
{
public:

	/* Constructor. Creates a new status bar with progress guage */
	nxStatusBar(wxWindow* parent);

	void Resize();

private:

	wxGauge*			m_pProgressGauge;

};