#include "Renderer.h"
#include "Job.h"

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
		f += 0.01;
		if (f > 1)
			f = 0;
		m_pParent->SwapBuffers();
	}

	return NULL;
}

void nxRenderer::Init() {
	if (NULL == m_pGLCtx)
		m_pGLCtx = new wxGLContext(m_pParent);

	if ( NULL != m_pGLCtx )
		std::cout << "Context Inited";
}