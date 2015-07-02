#pragma once

#include <vector>
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
class nxProgram;

typedef boost::lockfree::queue< nxGLJob* > nxGLJobQueue;

class nxRenderer : public wxThread
{
public:

								nxRenderer(nxEngine* eng);
								nxRenderer(wxGLCanvas* frame);

	void*						Entry();

	void						ScheduleGLJob(nxGLJob* job) { m_pGLCommandQueue->push(job); };
	void						SetDrawingCanvas(wxGLCanvas* frame) { m_pParent = frame; }
	bool						IsFramebufferReady() { return m_FBOInited; }

	void						InitFramebuffer();
	void						ResizeFramebuffer();
	void						UseProgram();
	nxProgram*					Program() { return m_ShaderPrograms[m_ProgramIndex]; }

	bool						InitExtensions();

	void						SetViewportSize(int w, int h) { m_VWidth = w; m_VHeight = h; };

	int							Width() { return m_VWidth; }
	int							Height() { return m_VHeight; }

	void						AddProgram(nxProgram* prog) { m_ShaderPrograms.push_back(prog); }

private:

	bool						m_IsActive;
	wxGLCanvas*					m_pParent;
	wxGLContext*				m_pGLCtx;
	nxGLJobQueue*				m_pGLCommandQueue;

	nxEngine*					m_pEngine;

	GLuint						m_FBO;
	GLuint						m_RBO;
	GLuint						m_DepthTexture;

	bool						m_FBOInited;

	unsigned int				m_State;

	int							m_VWidth;
	int							m_VHeight;
	int							m_ProgramIndex;

	std::vector<nxProgram*>		m_ShaderPrograms;

	void						Init();
	void						RenderFrame();
	void						SwapBuffers() { m_pParent->SwapBuffers(); }

};