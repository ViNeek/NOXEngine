#include "Window.h"

#include <wx/wupdlock.h>

#include "Job.h"
#include "JobFactory.h"
#include "CustomEvents.h"

#include "Scene.h"
#include "Renderer.h"
#include "Scheduler.h"
#include "Engine.h"

#define LOAD_SCENE_ID (wxID_HIGHEST + 1)

nxFrame::nxFrame(const wxChar *title, int xpos, int ypos, int width, int height)
	: wxFrame((wxFrame *)NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height) )
{
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	int args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

	m_pGLPanel = new nxGLPanel((wxFrame*)this, args);
	sizer->Add(m_pGLPanel, 1, wxEXPAND);

	m_pMenuBar = new wxMenuBar(0);
	m_pSceneMenu = new wxMenu();
	m_pMenuBar->Append(m_pSceneMenu, wxT("Scene"));

	wxMenuItem* m_LoadSceneMenu;
	m_LoadSceneMenu = new wxMenuItem(m_pSceneMenu, LOAD_SCENE_ID, wxString(wxT("Load Scene...")), wxEmptyString, wxITEM_NORMAL);
	m_pSceneMenu->Append(LOAD_SCENE_ID, wxT("LoadSceneMenu"));

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

	m_EngineState = new nxEngine;

	Engine()->Renderer()->SetDrawingCanvas(m_pGLPanel);
	Engine()->Scheduler()->SetFrame(this);

	Engine()->Start();

	Engine()->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_EXTENSION_INIT));
	
	// Setup callbacks
	Bind(wxEVT_SIZE, &nxFrame::OnResize, this, wxID_ANY);
	Bind(wxEVT_CLOSE_WINDOW, &nxFrame::OnClose, this, wxID_ANY);
	Bind(wxEVT_CLOSE_WINDOW, &nxFrame::OnClose, this, wxID_ANY);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &nxFrame::OnLoadScene, this, LOAD_SCENE_ID);

	Bind(nxRENDERER_EXIT_EVENT, &nxFrame::OnRendererExit, this, wxID_ANY);
	Bind(nxSCHEDULER_EXIT_EVENT, &nxFrame::OnSchedulerExit, this, wxID_ANY);

}
 
bool nxFrame::IsRendererFinished()
{ 
	return Engine()->IsRendererFinished();
}

bool nxFrame::IsSchedulerFinished() 
{ 
	return Engine()->IsRendererFinished();
}

void nxFrame::OnClose(wxCloseEvent& evt) {
	Engine()->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_JOB_RENDERER_EXIT, this));
	Engine()->Scheduler()->ScheduleOwnJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_SCHEDULER_EXIT, Engine()->Scheduler()));
}

void nxFrame::OnRendererExit(wxCommandEvent& evt) {
	Engine()->SetRendererFinished( true );
	std::cout << "would exit" << std::endl;
	if (Engine()->IsRendererFinished() && Engine()->IsSchedulerFinished()) {
		Destroy();
		_CrtDumpMemoryLeaks();
	}
}

void nxFrame::OnSchedulerExit(wxCommandEvent& evt) {
	std::cout << "would exit scheduler" << std::endl;
	Engine()->SetSchedulerFinished(true);
	if (Engine()->IsRendererFinished() && Engine()->IsSchedulerFinished()) {
		Destroy();
		_CrtDumpMemoryLeaks();
	}
}

void nxFrame::OnLoadScene(wxCommandEvent& WXUNUSED(evt))
{
	/*
	{
	if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
	wxICON_QUESTION | wxYES_NO, this) == wxNO)
	return;
	//else: proceed asking to the user the new file to open
	}
	*/

	wxFileDialog
		openFileDialog(this, _("Load Scene"), "", "",
		"NXSCENE files (*.nxscene)|*.nxscene", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	// proceed loading the file chosen by the user;
	// this can be done with e.g. wxWidgets input streams:
	// openFileDialog.GetPath().ToStdString()
	Engine()->Scene()->SetFilename(openFileDialog.GetPath().ToStdString());
	Engine()->Scheduler()->ScheduleJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_LOAD_SCENE, Engine()));

	std::cout << Engine()->Scene()->Filename() << std::endl;
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