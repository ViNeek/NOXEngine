#include <boost/log/trivial.hpp>

#include "GLUtils.h"

#include "Program.h"
#include "Renderer.h"
#include "Job.h"
#include "Scheduler.h"
#include "JobFactory.h"
#include "CustomEvents.h"
#include "Engine.h"

nxRenderer::nxRenderer(wxGLCanvas* frame)
{
	m_pParent = frame;
	m_pGLCtx = NULL;
	m_IsActive = true;
	m_FBOInited = false;
	m_pGLCommandQueue = new nxGLJobQueue(0);
	m_VWidth = 0;
	m_VHeight = 0;
	m_FBO = -1;
	m_ProgramIndex = 0;
} 

nxRenderer::nxRenderer(nxEngine* eng) {
	m_pEngine = eng;
	m_pParent = NULL;
	m_pGLCtx = NULL;
	m_IsActive = true;
	m_pGLCommandQueue = new nxGLJobQueue(0);
	m_VWidth = 0;
	m_VHeight = 0;
	m_FBO = -1;
	m_ProgramIndex = 0;
}

void nxRenderer::UseProgram() {
	if ((size_t)m_ProgramIndex < m_ShaderPrograms.size() )
		m_ShaderPrograms[m_ProgramIndex]->Use();
}

void *nxRenderer::Entry()
{
	Init();
	 
	float f = 0.0;
	nxGLJob* currentJob;
	m_pParent->SetCurrent(*m_pGLCtx);

	while ( m_IsActive ) {

		while ( m_pGLCommandQueue->pop(currentJob) )
			m_IsActive = currentJob->Execute();

		UseProgram();

		RenderFrame();

		SwapBuffers();
		
	}

	wxFrame* evtHandler = (wxFrame*)m_pParent->GetParent();
	wxCommandEvent* evt = new wxCommandEvent(nxRENDERER_EXIT_EVENT); // Still keeping it simple, don't give a specific event ID
	wxQueueEvent(evtHandler, evt); // This posts to ourselves: it'll be caught and sent to a different method

	return NULL;
}

bool error = true;
void nxRenderer::RenderFrame() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, m_VWidth, m_VHeight);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, m_VWidth, m_VHeight);
	if (error) Utils::GL::CheckGLState("Frame");

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);

	if (error) Utils::GL::CheckGLState("Swap");

	glBlitFramebuffer(0, 0, m_VWidth - 1, m_VHeight - 1,
				      0, 0, m_VWidth - 1, m_VHeight - 1,
					  GL_COLOR_BUFFER_BIT , GL_NEAREST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//error = false;
}

void nxRenderer::Init() {
	if (NULL == m_pGLCtx)
		m_pGLCtx = new wxGLContext(m_pParent);

	if ( NULL != m_pGLCtx )
		std::cout << "Context Inited";
}

bool nxRenderer::InitExtensions() {
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

		return false;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	m_State |= NX_RENDERER_EXTENSIONS_READY;

	return true;
}

void nxRenderer::InitFramebuffer() {
	
	glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, m_VWidth, m_VHeight);

	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sl_SceneState.depthSingleLayer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RBO);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	Utils::GL::CheckGLState("Framebuffer Creation");
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		printf("Single Layered FBO Complete %x\n", status);
		BOOST_LOG_TRIVIAL(info) << "Single Layered FBO Complete ";
		break;
	default:
		BOOST_LOG_TRIVIAL(error) << "Single Layered FBO Incomplete ";
		break;
	}

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations


	m_State |= NX_RENDERER_FRAMEBUFFER_READY;

}

void nxRenderer::ResizeFramebuffer() {
	
	if (m_State > NX_RENDERER_FRAMEBUFFER_READY) {
		if (glIsRenderbuffer(m_RBO)) {
			glDeleteRenderbuffers(1, &m_RBO);
			glGenRenderbuffers(1, &m_RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, m_VWidth, m_VHeight);

			//Utils::GL::CheckGLState("FrameBuffer Creation");

			m_State |= NX_RENDERER_FRAMEBUFFER_READY;

			glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
			glClearColor(0.0f, 0.0f, 1.0f, 1.0f);				// Black Background
			glClearDepth(1.0f);									// Depth Buffer Setup
			glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
			glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

		}
	}
	
}

bool nxRendererTerminator::operator()(void* data) {

	return false;
}

bool nxExtensionInitializer::operator()(void* data) {
	nxExtensionInitializerBlob* blob = (nxExtensionInitializerBlob*)data;

	blob->m_Renderer->InitExtensions();
	
	return true;
}

bool nxFramebufferInitializer::operator() (void* data) {
	nxFramebufferInitializerBlob* blob = (nxFramebufferInitializerBlob*)data;

	blob->m_Renderer->InitFramebuffer();

	return true;
}

bool nxFramebufferResizer::operator()(void* data) {
	nxFramebufferResizerBlob* blob = (nxFramebufferResizerBlob*)data;

	//std::cout << "resizing\n";
	blob->m_Renderer->ResizeFramebuffer();

	return true;
} 