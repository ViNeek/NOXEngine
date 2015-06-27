#pragma once

#include <GL/glew.h>

#include "wx/wx.h"
#include "wx/glcanvas.h"

#include <boost/lockfree/queue.hpp>

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

private:

	bool				m_IsActive;
	wxGLCanvas*			m_pParent;
	wxGLContext*		m_pGLCtx;
	nxGLJobQueue*		m_pGLCommandQueue;

	nxEngine*			m_pEngine;

	void				Init();
};