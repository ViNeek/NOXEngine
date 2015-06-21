#pragma once

#include "wx/wx.h"

#include "GL/glew.h"

#include "wx/glcanvas.h"

// Forward declares
class nxRenderer;
class nxStatusBar;
class nxGLPanel;

/* GL Rendering Window */
class nxFrame : public wxFrame
{
public:

	/* Constructor. Creates a new window for gl rendering */
	nxFrame(const wxChar *title, int xpos, int ypos, int width, int height);

	void OnResize(wxSizeEvent evt);

	void InitRenderer();

	nxRenderer* Renderer() { return m_pRenderer; };

private:

	wxMenuBar*			m_pMenuBar;
	wxMenu*				m_pPrefMenu;
	wxMenu*				m_pSceneMenu;
	wxMenu*				m_pShaderMenu;
	wxMenu*				m_pVoxelMenu;
	wxMenu*				m_pHelpMenu;

	nxStatusBar*		m_pStatusBar;
	nxGLPanel*			m_pGLPanel;
	nxRenderer*			m_pRenderer;

	virtual void PositionStatusBar();
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