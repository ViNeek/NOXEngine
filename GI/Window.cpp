#include "Window.h"

#include "Renderer.h"
#include "Scheduler.h"

#include <wx/wupdlock.h>

#include "JobFactory.h"
#include "CustomEvents.h"
#include "Scene.h"

nxFrame::nxFrame(const wxChar *title, int xpos, int ypos, int width, int height)
	: wxFrame((wxFrame *)NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height) )
{
	m_RendererFinished = false;
	m_SchedulerFinished = false;

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	int args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

	m_pGLPanel = new nxGLPanel((wxFrame*)this, args);
	sizer->Add(m_pGLPanel, 1, wxEXPAND);

	m_pMenuBar = new wxMenuBar(0);
	m_pSceneMenu = new wxMenu();
	m_pMenuBar->Append(m_pSceneMenu, wxT("Scene"));

	wxMenuItem* m_LoadSceneMenu;
	m_LoadSceneMenu = new wxMenuItem(m_pSceneMenu, wxID_ANY, wxString(wxT("Load Scene...")), wxEmptyString, wxITEM_NORMAL);
	m_pSceneMenu->Append(m_LoadSceneMenu);

	wxMenuItem* m_ExitMenu;
	m_ExitMenu = new wxMenuItem(m_pSceneMenu, wxID_ANY, wxString(wxT("Exit.")), wxEmptyString, wxITEM_NORMAL);
	m_pSceneMenu->Append(m_ExitMenu);

	m_pPrefMenu = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem(m_pPrefMenu, wxID_ANY, wxString(wxT("MyMenuItem")), wxEmptyString, wxITEM_NORMAL);
	m_pPrefMenu->Append(m_menuItem1);

	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem(m_pPrefMenu, wxID_ANY, wxString(wxT("MyMenuItem")), wxEmptyString, wxITEM_NORMAL);
	m_pPrefMenu->Append(m_menuItem2);

	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem(m_pPrefMenu, wxID_ANY, wxString(wxT("MyMenuItem")), wxEmptyString, wxITEM_NORMAL);
	m_pPrefMenu->Append(m_menuItem3);

	m_pMenuBar->Append(m_pPrefMenu, wxT("Preferences"));

	m_pShaderMenu = new wxMenu();
	m_pMenuBar->Append(m_pShaderMenu, wxT("Shader"));

	m_pVoxelMenu = new wxMenu();
	m_pMenuBar->Append(m_pVoxelMenu, wxT("Voxelization"));

	m_pHelpMenu = new wxMenu();
	m_pMenuBar->Append(m_pHelpMenu, wxT("Help"));
	
	//m_pRenderer
	this->SetMenuBar(m_pMenuBar);

	m_pStatusBar = new nxStatusBar(this);
	this->SetStatusBar( m_pStatusBar );

	this->Centre(wxBOTH);

	this->SetSizer(sizer);
	this->SetAutoLayout(true);

	// Init NOX Engine state

	m_Scene = new nxScene;
	m_pScheduler = new nxScheduler(this);
	m_pRenderer = new nxRenderer(m_pGLPanel);
	Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_EXTENSION_INIT));
	
	// Setup callbacks
	Bind(wxEVT_SIZE, &nxFrame::OnResize, this, wxID_ANY);
	Bind(wxEVT_CLOSE_WINDOW, &nxFrame::OnClose, this, wxID_ANY);
	Bind(nxRENDERER_EXIT_EVENT, &nxFrame::OnRendererExit, this, wxID_ANY);
	Bind(nxSCHEDULER_EXIT_EVENT, &nxFrame::OnSchedulerExit, this, wxID_ANY);
}
 
void nxFrame::OnClose(wxCloseEvent& evt) {
	Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_JOB_RENDERER_EXIT, this));
	Scheduler()->ScheduleOwnJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_SCHEDULER_EXIT, Scheduler()));
}

void nxFrame::OnRendererExit(wxCommandEvent& evt) {
	m_RendererFinished = true;
	std::cout << "would exit" << std::endl;
	if ( m_RendererFinished && m_SchedulerFinished )
		Destroy();
}

void nxFrame::OnSchedulerExit(wxCommandEvent& evt) {
	std::cout << "would exit scheduler" << std::endl;
	m_SchedulerFinished = true;
	if ( m_RendererFinished && m_SchedulerFinished )
		Destroy();
}

void nxFrame::InitRenderer()
{
	m_pRenderer->Create();
	m_pRenderer->Run();
}

void nxFrame::InitScheduler()
{
	m_pScheduler->Create();
	m_pScheduler->Run();
	for (int i = 0; i < 100; i++) {
		//m_pScheduler->ScheduleOwnJob(nxJobFactory::CreateJob(NX_JOB_DUMMY));
		//m_pScheduler->ScheduleJob(nxJobFactory::CreateJob(NX_JOB_DUMMY));
	}
}

void nxFrame::InitScene(std::string path)
{
	m_Scene->InitFromFile(path);
}

void nxFrame::PositionStatusBar()
{
	nxStatusBar* status = (nxStatusBar*)GetStatusBar();
	wxScopedPtr<wxWindowUpdateLocker> lock;

	if ( status ) 
		lock.reset(new wxWindowUpdateLocker(status));
	
	status->Resize();

	wxFrame::PositionStatusBar();
}

nxGLPanel::nxGLPanel(wxFrame* parent, int* args)
	: wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	//m_pGLCtx = new wxGLContext(this);

	// To avoid flashing on MSW
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

nxStatusBar::nxStatusBar( wxWindow* parent )
	: wxStatusBar(parent, wxID_ANY, wxST_SIZEGRIP, "MainStatusbar")
{
	m_pProgressGauge = new wxGauge(this, wxID_ANY, 100, wxPoint(0, 0), wxSize(parent->GetSize().GetWidth(), GetSize().GetHeight() ), wxGA_SMOOTH, wxDefaultValidator);
	m_pProgressGauge->SetValue(50);
}

void nxStatusBar::Resize()
{
	m_pProgressGauge->SetSize(wxSize(this->GetParent()->GetSize().GetWidth(), GetSize().GetHeight()));
}