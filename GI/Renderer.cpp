#include "Renderer.h"
#include "Job.h"

#include "CustomEvents.h"

nxRenderer::nxRenderer(wxGLCanvas* frame)
{
	m_pParent = frame;
	m_pGLCtx = NULL;
	m_IsActive = true;
	m_pGLCommandQueue = new nxGLJobQueue(0);
} 

void *nxRenderer::Entry()
{
	Init();
	 
	float f = 0.0;
	nxGLJob* currentJob;
	m_pParent->SetCurrent(*m_pGLCtx);

	while ( m_IsActive ) {

		if (m_pGLCommandQueue->pop(currentJob))
			m_IsActive = currentJob->Execute();

		m_pParent->SetCurrent(*m_pGLCtx);
		glClearColor(f, f, f, 0);
		glViewport(0,0,this->m_pParent->GetSize().GetX(), this->m_pParent->GetSize().GetY());

	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		f += 0.01f;
		if (f > 1)
			f = 0;
		m_pParent->SwapBuffers();
	}

	wxFrame* evtHandler = (wxFrame*)m_pParent->GetParent();
	wxCommandEvent* evt = new wxCommandEvent(nxRENDERER_EXIT_EVENT); // Still keeping it simple, don't give a specific event ID
	wxQueueEvent(evtHandler, evt); // This posts to ourselves: it'll be caught and sent to a different method

	return NULL;
}

void nxRenderer::Init() {
	if (NULL == m_pGLCtx)
		m_pGLCtx = new wxGLContext(m_pParent);

	if ( NULL != m_pGLCtx )
		std::cout << "Context Inited";
}