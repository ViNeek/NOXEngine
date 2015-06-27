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
	void					OnResize(wxSizeEvent& evt) { evt.Skip(); }
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
	nxGLPanel(wxFrame* parent, int* args);

	int getWidth();
	int getHeight();

	void render(wxPaintEvent& evt);
	void prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);
	void prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);

	// events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

private:

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