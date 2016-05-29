#include <boost/log/trivial.hpp>

#include "GLUtils.h"

#include "Program.h"
#include "Renderer.h"
#include "Voxelizer.h"
#include "DistanceField.h"
#include "RayMarcher.h"
#include "ReflectiveShadowMap.h"
#include "Camera.h"
#include "Job.h"
#include "Scheduler.h"
#include "JobFactory.h"
#include "CustomEvents.h"
#include "Engine.h"
#include "Scene.h"

#include <boost/multi_array.hpp>

nxRenderer::nxRenderer(wxGLCanvas* frame)
{
	m_pParent = frame;
	m_pGLCtx = NULL;
	m_IsActive = true;
	m_FBOInited = false;
	m_IsVoxelizerReady = false;
	m_Voxelizing = false;
	m_pGLCommandQueue = new nxGLJobQueue(0);
	m_VWidth = 0;
	m_VHeight = 0;
	m_FBO = -1;
	m_ProgramIndex = 0;
    m_DrawState = NOX_SIMPLE_PASS;
} 

nxRenderer::nxRenderer(nxEngine* eng) {
	m_pEngine = eng;
	m_pParent = NULL;
	m_pGLCtx = NULL;
	m_IsActive = true;
	m_FBOInited = false;
	m_IsVoxelizerReady = false;
	m_Voxelizing = false;
	m_pGLCommandQueue = new nxGLJobQueue(0);
	m_VWidth = 0;
	m_VHeight = 0;
	m_DepthTexture = -1;
	m_FBO = -1;
	m_ProgramIndex = 0;
    m_DrawState = NOX_SIMPLE_PASS;

}

void nxRenderer::UseProgram() {
	
	m_pActiveProgram->Use();
	
	//printf("Active Program %s %d\n", m_pActiveProgram->GetName().c_str(), m_pActiveProgram->GetID());
}

bool error = true;

void *nxRenderer::Entry()
{
	Init();
	 
	float f = 0.0;
	nxGLJob* currentJob;
	m_pParent->SetCurrent(*m_pGLCtx);
	
	while ( m_IsActive ) {

		while ( m_pGLCommandQueue->pop(currentJob) )
			m_IsActive = currentJob->Execute();

		//UseProgram();

		//RenderFrameDemo();
		RenderFrame();
		
		//if (errorGL) Utils::GL::CheckGLState("Program USE");

		if (VoxelizerReady()) {
			glm::vec3 l_TestPosition(0, 10, 0);
			glm::vec3 l_TestPositionA(0, 10, 0);
			glm::vec3 l_TestPositionB(0, 10, 0);
			glm::vec3 l_GridSize = Voxelizer()->GridSize();
			glm::vec3 l_GridCenter = Voxelizer()->GridCenter();
			glm::uvec3 l_Dims = Voxelizer()->Dimesions();
			glm::vec3 l_GridMax = Voxelizer()->GridMax();
			glm::vec3 l_GridMin = Voxelizer()->GridMin();
			glm::vec3 l_Voxel = l_GridSize / glm::vec3(l_Dims);

			l_TestPosition -= l_GridMin;
			glm::ivec3 l_Indexes= l_TestPosition / l_Voxel;
			//std::cout << "Voxelizer : " << l_TestPosition.x << "," << l_TestPosition.y << "," << l_TestPosition.y << "\n";
			//std::cout << "Voxel : " << l_Voxel.x << "," << l_Voxel.y << "," << l_Voxel.y << "\n";
			//std::cout << "Voxel At : " << l_Indexes.x << "," << l_Indexes.y << "," << l_Indexes.y << "\n";
			//std::cout << "Grid Center : " << l_GridCenter.x << "," << l_GridCenter.y << "," << l_GridCenter.y << "\n";
			//std::cout << "Grid Size : " << l_GridSize.x << "," << l_GridSize.y << "," << l_GridSize.y << "\n";

			l_TestPositionA -= l_GridMin;
			//std::cout << "Voxelizer : " << l_TestPositionA.x << "," << l_TestPositionA.y << "," << l_TestPositionA.y << "\n";

			l_TestPositionB += l_GridMin;
			//std::cout << "Voxelizer : " << l_TestPositionB.x << "," << l_TestPositionB.y << "," << l_TestPositionB.y << "\n";

			//std::cout << "Voxelizer : " << l_GridSize.x << "," << l_GridSize.y << "," << l_GridSize.y << "\n";
			//std::cout << "Voxelizer : " << l_Dims.x << "," << l_Dims.y << "," << l_Dims.y << "\n";
			//std::cout << "Voxelizer : " << l_GridMax.x << "," << l_GridMax.y << "," << l_GridMax.y << "\n";
			//std::cout << "Voxelizer : " << l_GridMin.x << "," << l_GridMin.y << "," << l_GridMin.y << "\n";

			if (Voxelizer()->CaptureGrid()) {
				//glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->DistanceField()->DistanceFieldBuffer());
				//glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->Voxelizer()->VoxelBuffer());
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, this->Voxelizer()->VoxelBuffer());
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->DistanceField()->DistanceFieldBuffer());

				printf("Capture \n");
				Voxelizer()->SetCaptureGrid(false);
				//Voxelizer()->PrintGridMesh(m_ssbo);
				Voxelizer()->PrintGridMesh(Voxelizer()->VoxelBuffer());
				//Voxelizer()->PrintGridMeshF(DistanceField()->DistanceFieldBuffer());
				
			}

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_pEngine->Renderer()->Voxelizer()->VoxelBuffer());
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32I, GL_RGBA, GL_UNSIGNED_INT, NULL);
		}
		
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		SwapBuffers();
		
	}

	wxFrame* evtHandler = (wxFrame*)m_pParent->GetParent();
	wxCommandEvent* evt = new wxCommandEvent(nxRENDERER_EXIT_EVENT); // Still keeping it simple, don't give a specific event ID
	wxQueueEvent(evtHandler, evt); // This posts to ourselves: it'll be caught and sent to a different method

	return NULL;
}

void nxRenderer::SetActiveProgramByName(const std::string& name) { 
	nxProgram* prog = m_ShaderPrograms[name];

	nxProgramSwapperBlob* progData = new nxProgramSwapperBlob(this, prog);
	m_pEngine->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_PROGRAM_SWAP, progData));

	//m_ProgramName = name;
}

void nxRenderer::RenderDebugFrame() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_VWidth, m_VHeight);
	//if (error) Utils::GL::CheckGLState("Frame");

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
	//if (error) Utils::GL::CheckGLState("Draw");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_VWidth, m_VHeight);

	m_pEngine->Scene()->Draw();
	//m_pEngine->Scene()->DrawVoxelized();

	//if (error) Utils::GL::CheckGLState("Draw");

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);

	//if (error) Utils::GL::CheckGLState("Swap");

	glBlitFramebuffer(0, 0, m_VWidth - 1, m_VHeight - 1,
		0, 0, m_VWidth - 1, m_VHeight - 1,
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//error = false;
}

void nxRenderer::RenderFrame() {
	//if (error) Utils::GL::CheckGLState("Frame");
	//Utils::GL::CheckGLState("Render");
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//if (error) Utils::GL::CheckGLState("Frame 0");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//if (error) Utils::GL::CheckGLState("Frame 1");
	glViewport(0, 0, m_VWidth, m_VHeight);
	//if (error) Utils::GL::CheckGLState("Frame");

    //if (IsVoxelizing())
	m_pEngine->Scene()->CaptureRSM();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
	//if (error) Utils::GL::CheckGLState("Draw");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_VWidth, m_VHeight);

    if ( (m_DrawState == NOX_SIMPLE_PASS) && !IsVoxelizing())
		m_pEngine->Scene()->Draw();
    else if ((m_DrawState == NOX_GI_PASS) && IsVoxelizing())
		m_pEngine->Scene()->DrawVoxelized();
    else
        m_pEngine->Scene()->DrawPreviewVoxelized();

    //if (error) Utils::GL::CheckGLState("Draw");

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);

	//if (error) Utils::GL::CheckGLState("Swap");

	glBlitFramebuffer(0, 0, m_VWidth - 1, m_VHeight - 1,
				      0, 0, m_VWidth - 1, m_VHeight - 1,
					  GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	error = false;
}

void nxRenderer::RenderFrameDemo() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glViewport(0, 0, m_VWidth, m_VHeight);
	//glViewport(0, 0, 128, 128);
	//if (error) Utils::GL::CheckGLState("Frame");

	if ( VoxelizerReady() )
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Voxelizer()->VoxelizerFramebuffer());

	//if (error) Utils::GL::CheckGLState("Draw");

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_OR);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glViewport(0, 0, m_VWidth, m_VHeight);
	//glViewport(0, 0, 128, 128);

	//m_pEngine->Scene()->Draw();
	m_pEngine->Scene()->DrawVoxelized();

	//if (error) Utils::GL::CheckGLState("Draw");

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);

	//if (error) Utils::GL::CheckGLState("Swap");

	//glBlitFramebuffer(0, 0, m_VWidth - 1, m_VHeight - 1,
	//			      0, 0, m_VWidth - 1, m_VHeight - 1,
	//				  GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	//glBlitFramebuffer(0, 0, 128 - 1, 128 - 1,
	//	0, 0, 128 - 1, 128 - 1,
	//	GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	error = false;
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
		BOOST_LOG_TRIVIAL(error) << "Extension initialization through GLEW failed with error " << glewGetErrorString(err);

		return false;
	}
	BOOST_LOG_TRIVIAL(info) << "GLEW Version : " << glewGetString(GLEW_VERSION);

	m_State |= NX_RENDERER_EXTENSIONS_READY;

	return true;
}

void nxRenderer::InitFramebuffer() {
	
	/* TEMPORARY init a a test Shader Storage Buffer object */
	glGenBuffers(1, &m_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 512 * 512 * 512 * 4, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	Utils::GL::CheckGLState("SSBO");
	glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, m_VWidth, m_VHeight);

	Utils::GL::CheckGLState("RBO");
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// Texture 10 Is depth buffers
	glActiveTexture(GL_TEXTURE10);

	glGenTextures(1, &m_DepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	//Utils::GL::CheckGLState("Texture Creation");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//Utils::GL::CheckGLState("Texture Params");


	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_VWidth, m_VHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	Utils::GL::CheckGLState("Depth Buffer");
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RBO);
	//Utils::GL::CheckGLState("Buffer Attach");

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//Utils::GL::CheckGLState("Framebuffer Creation");
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

	//glEnable(GL_CULL_FACE);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(5);
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	Utils::GL::CheckGLState("\n\n\n\n\n Init Complete \n\nn\n\n");


	m_State |= NX_RENDERER_FRAMEBUFFER_READY;

}

void nxRenderer::ResizeFramebuffer() {
	
	if (m_State > NX_RENDERER_FRAMEBUFFER_READY) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (glIsFramebuffer(m_FBO)) {
			glDeleteRenderbuffers(1, &m_RBO);
			glDeleteFramebuffers(1, &m_FBO);
			glDeleteTextures(1, &m_DepthTexture);

			glGenRenderbuffers(1, &m_RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, m_VWidth, m_VHeight);

			//Utils::GL::CheckGLState("FrameBuffer Creation");
			glGenRenderbuffers(1, &m_RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, m_VWidth, m_VHeight);

			glGenFramebuffers(1, &m_FBO);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

			// Texture 10 Is depth buffers
			glActiveTexture(GL_TEXTURE10);

			glGenTextures(1, &m_DepthTexture);
			glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
			//Utils::GL::CheckGLState("Texture Creation");
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			//Utils::GL::CheckGLState("Texture Params");

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_VWidth, m_VHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RBO);

			
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			//Utils::GL::CheckGLState("Framebuffer Creation");
			switch (status)
			{
			case GL_FRAMEBUFFER_COMPLETE:
				break;
			default:
				BOOST_LOG_TRIVIAL(error) << "Single Layered FBO Incomplete ";
				break;
			}
			
			glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
			glClearDepth(1.0f);									// Depth Buffer Setup
			glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
			glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

			m_State |= NX_RENDERER_FRAMEBUFFER_READY;

		}
		
		m_pEngine->Scene()->SetProjection(45.0f, (GLfloat)m_VWidth / (GLfloat)m_VHeight, 1.0f, 2000.0f);

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