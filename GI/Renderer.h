#pragma once

#include <GL/glew.h>

#include "wx/wx.h"
#include "wx/glcanvas.h"

#include <boost/lockfree/queue.hpp>

enum nxRendererState {
	NX_RENDERER_OFF = (1 << 0),
	NX_RENDERER_EXTENSIONS_READY = (1 << 1),
	NX_RENDERER_FRAMEBUFFER_READY = (1 << 2),
	NX_RENDERER_DRAW_READY = NX_RENDERER_FRAMEBUFFER_READY | NX_RENDERER_EXTENSIONS_READY
};

class nxGLJob;
class nxEngine;

typedef boost::lockfree::queue< nxGLJob* > nxGLJobQueue;

class nxRenderer : public wxThread
{
public:

						nxRenderer(nxEngine* eng);
						nxRenderer(wxGLCanvas* frame);

	void*				Entry();

	void				ScheduleGLJob(nxGLJob* job) { m_pGLCommandQueue->push(job); };
	void				SetDrawingCanvas(wxGLCanvas* frame) { m_pParent = frame; }
	bool				IsFramebufferReady() { return m_FBOInited; }

	void				InitFramebuffer();
	void				ResizeFramebuffer();
	bool				InitExtensions();

private:

	bool				m_IsActive;
	wxGLCanvas*			m_pParent;
	wxGLContext*		m_pGLCtx;
	nxGLJobQueue*		m_pGLCommandQueue;

	nxEngine*			m_pEngine;

	GLuint				m_FBO;
	GLuint				m_RBO;

	bool				m_FBOInited;

	unsigned int		m_State;

	void				Init();
};