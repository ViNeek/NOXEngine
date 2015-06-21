#pragma once

#include "wx/wx.h"
#include "wx/glcanvas.h"

#include <boost/lockfree/queue.hpp>

class nxGLJob;

typedef boost::lockfree::queue< nxGLJob* > nxGLJobQueue;

class nxRenderer : public wxThread
{
public:

	nxRenderer(wxGLCanvas* frame);
	void *Entry();

	void AddGLJob(nxGLJob* job);

private:

	bool				m_IsActive;
	wxGLCanvas*			m_pParent;
	wxGLContext*		m_pGLCtx;
	nxGLJobQueue*		m_pGLCommandQueue;

	void Init();
};