#include "Window.h"

#include <wx/wupdlock.h>

#include "Job.h"
#include "JobFactory.h"
#include "CustomEvents.h"

#include "Camera.h"
#include "Scene.h"
#include "Renderer.h"
#include "Scheduler.h"
#include "Engine.h"

nxFrame::nxFrame(const wxChar *title, int xpos, int ypos, int width, int height)
	: wxFrame((wxFrame *)NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height) )
{
	// Create Engine
	m_EngineState = new nxEngine;

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	// OpenGL args for wxWidgets
	int args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

	m_pGLPanel = new nxGLPanel(this, args);
	sizer->Add(m_pGLPanel, 1, wxEXPAND);

	m_pMenuBar = new wxMenuBar(0);
	m_pSceneMenu = new wxMenu();
	m_pMenuBar->Append(m_pSceneMenu, wxT("Scene"));

	wxMenuItem* m_LoadSceneMenu;
	m_LoadSceneMenu = new wxMenuItem(m_pSceneMenu, NX_LOAD_SCENE_ID, wxString(wxT("Load Scene...")), wxEmptyString, wxITEM_NORMAL);
	m_pSceneMenu->Append(NX_LOAD_SCENE_ID, wxT("Load Scene"));

	wxMenuItem* m_ExitMenu;
	m_ExitMenu = new wxMenuItem(m_pSceneMenu, NX_PROGRAM_EXIT_ID, wxString(wxT("Exit.")), wxEmptyString, wxITEM_NORMAL);
	m_pSceneMenu->Append(NX_PROGRAM_EXIT_ID, wxT("Exit..."));

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
	
	this->SetMenuBar(m_pMenuBar);

	m_pStatusBar = new nxStatusBar(this);
	this->SetStatusBar( m_pStatusBar );

	this->Centre(wxBOTH);

	this->SetSizer(sizer);
	this->SetAutoLayout(true);

	// Init NOX Engine state

	Engine()->Renderer()->SetDrawingCanvas(m_pGLPanel);
	Engine()->Renderer()->SetViewportSize(width, height);
	Engine()->Scheduler()->SetFrame(this);

	// Schedule the renderer' s intial jobs

	// Initialize Default Shader
	nxProgram* prog = new nxProgram(0);
	std::string defaultName("Default");
	prog->SetName(defaultName);
	Engine()->Renderer()->SetProgram(prog);
	Engine()->Renderer()->AddProgram(prog);

	//nxProgramLinkerBlob* progData = new nxProgramLinkerBlob(Engine(), prog);
	//Engine()->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_LINK_PROGRAM, progData));

	// Initialize Extentions
	nxExtensionInitializerBlob* dataExt = new nxExtensionInitializerBlob(Engine(), Engine()->Renderer());
	Engine()->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_EXTENSION_INIT, dataExt));
	
	// Initialize Framebuffer
	nxFramebufferInitializerBlob* dataFBO = new nxFramebufferInitializerBlob(Engine(), Engine()->Renderer());
	Engine()->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_FRAMEBUFFER_INIT, dataFBO));

	// Start Resource reload loop
	//Engine()->Scheduler()->ScheduleJob(nxJobFactory::CreateJob(NX_JOB_RESOURCE_LOOP, Engine()->Scheduler(), true, 4 * NOXConstants::NANOS_IN_SECONDS));

	// Setup callbacks
	Bind(wxEVT_SIZE, &nxFrame::OnResize, this, wxID_ANY);
	Bind(wxEVT_CLOSE_WINDOW, &nxFrame::OnClose, this, wxID_ANY);
	Bind(wxEVT_CLOSE_WINDOW, &nxFrame::OnClose, this, wxID_ANY);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &nxFrame::OnLoadScene, this, NX_LOAD_SCENE_ID);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &nxFrame::OnExit, this, NX_PROGRAM_EXIT_ID);

	// Custom Event Callbacks
	Bind(nxRENDERER_EXIT_EVENT, &nxFrame::OnRendererExit, this, wxID_ANY);
	Bind(nxSCHEDULER_EXIT_EVENT, &nxFrame::OnSchedulerExit, this, wxID_ANY);
	Bind(nxPROGRAM_ADDED_EVENT, &nxFrame::OnProgramAdded, this, wxID_ANY);

}
 
void nxFrame::EngineStart() {
	Engine()->Start();
}

bool nxFrame::IsRendererFinished()
{ 
	return Engine()->IsRendererFinished();
}

bool nxFrame::IsSchedulerFinished() 
{ 
	return Engine()->IsRendererFinished();
}

void nxFrame::OnProgramAdded(wxCommandEvent& evt) {
	std::cout << "Adding Program " << evt.GetString();
	wxWindowID newID = wxWindow::NewControlId();
	m_pShaderMenu->Append(newID, evt.GetString(), "Simple Shader", true);
	m_pShaderMenu->Check(newID, true);
	wxCommandEvent* newEvt = new wxCommandEvent();
	newEvt->SetString("User data " + evt.GetString());

	m_pShaderMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, &nxFrame::OnProgramSwitch, this, newID, newID + 1, newEvt);
}

void nxFrame::OnProgramSwitch(wxCommandEvent& evt) {
	wxMessageBox(((wxCommandEvent*)evt.GetEventUserData())->GetString());
}

void nxFrame::OnResize(wxSizeEvent& evt) {
	nxFramebufferResizerBlob* data = new nxFramebufferResizerBlob(Engine(), Engine()->Renderer());
	Engine()->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_GL_JOB_FRAMEBUFFER_RESIZE, data));
	Engine()->Renderer()->SetViewportSize(evt.GetSize().GetX(), evt.GetSize().GetY());

	if (Engine()->Scene()->CameraReady()) {
		Engine()->Scene()->Camera()->SetBounds(evt.GetSize().GetX(), evt.GetSize().GetY());
		
		std::cout << "Width : " << evt.GetSize().GetX();
		std::cout << "Height : " << evt.GetSize().GetY();
		std::cout << "Width : " << Engine()->Scene()->Camera()->Width();
		std::cout << "Height : " << Engine()->Scene()->Camera()->Height();

	}

	evt.Skip();
}

void nxGLPanel::OnLeftClickPressed(wxMouseEvent& evt) {
	//wxMessageBox("left click press");

	// Take focus and Capture mouse
	SetFocus();
	CaptureMouse();

	if (m_pEngine->Scene()->CameraReady()) {
		m_pEngine->Scene()->Camera()->SetCursor(evt.GetPosition().x, evt.GetPosition().y);
		m_pEngine->Scene()->Camera()->SetClicked(true);
	}

}

void nxGLPanel::OnLeftClickReleased(wxMouseEvent& evt) {
	//wxMessageBox("left click release");

	// Release previously captured mouse
	if (HasCapture())
		ReleaseMouse();

	if (m_pEngine->Scene()->CameraReady()) {
		m_pEngine->Scene()->Camera()->SetCursor(evt.GetPosition().x, evt.GetPosition().y);
		m_pEngine->Scene()->Camera()->SetClicked(false);
	}
}

void nxGLPanel::OnMouseWheelRolled(wxMouseEvent& evt) {
	//wxMessageBox("mouse roll");
	if (m_pEngine->Scene()->CameraReady()) {
		float moved = (float)evt.GetWheelDelta() / 80;
		
		if (evt.GetWheelRotation() < 0)
			m_pEngine->Scene()->Camera()->SetZ(m_pEngine->Scene()->Camera()->Position().z - moved);
		else
			m_pEngine->Scene()->Camera()->SetZ(m_pEngine->Scene()->Camera()->Position().z + moved);
	}
}

void nxGLPanel::OnMouseMoved(wxMouseEvent& evt) {

	if (m_pEngine->Scene()->CameraReady()) {
		glm::vec2 tempPoint = m_pEngine->Scene()->Camera()->Cursor();
		m_pEngine->Scene()->Camera()->SetCursor(evt.GetPosition().x, evt.GetPosition().y);
		m_pEngine->Scene()->Camera()->SetClicked(evt.LeftIsDown());

		if (evt.MiddleIsDown()) {
			std::cout << " Delta X : " << (m_pEngine->Scene()->Camera()->Cursor().x - tempPoint.x) << std::endl;
			std::cout << " Delta X : " << (m_pEngine->Scene()->Camera()->Cursor().y - tempPoint.y) << std::endl;
			m_pEngine->Scene()->Camera()->SetX(m_pEngine->Scene()->Camera()->Position().x + ((m_pEngine->Scene()->Camera()->Cursor().x - tempPoint.x)*5));
			m_pEngine->Scene()->Camera()->SetY(m_pEngine->Scene()->Camera()->Position().y - ((m_pEngine->Scene()->Camera()->Cursor().y - tempPoint.y)*5));
		}
	}
}

void nxFrame::OnClose(wxCloseEvent& evt) {
	Engine()->Renderer()->ScheduleGLJob((nxGLJob*)nxJobFactory::CreateJob(NX_JOB_RENDERER_EXIT, this));
	Engine()->Scheduler()->ScheduleOwnJob((nxJob*)nxJobFactory::CreateJob(NX_JOB_SCHEDULER_EXIT, Engine()->Scheduler()));
}

void nxFrame::OnExit(wxCommandEvent& evt) {
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
		"NXSCENE files (*.json)|*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

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

	if (status)
		lock.reset(new wxWindowUpdateLocker(status));
	else
		return;

	status->Resize();

	wxFrame::PositionStatusBar();
}

nxGLPanel::nxGLPanel(nxFrame* parent, int* args)
	: wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	//m_pGLCtx = new wxGLContext(this);

	// Keyboard Input Callbacks
	//Bind(wxEVT_KEY_DOWN, wxKeyEventHandler(nxFrame::OnKeyPressed), NULL, this);
	//Bind(wxEVT_KEY_UP, wxKeyEventHandler(nxFrame::OnKeyReleased), NULL, this);
	Bind(wxEVT_LEFT_DOWN, wxMouseEventHandler(nxGLPanel::OnLeftClickPressed), this, wxID_ANY);
	Bind(wxEVT_LEFT_UP, wxMouseEventHandler(nxGLPanel::OnLeftClickReleased), this, wxID_ANY);
	Bind(wxEVT_MOUSEWHEEL, wxMouseEventHandler(nxGLPanel::OnMouseWheelRolled), this, wxID_ANY);
	Bind(wxEVT_MOTION, wxMouseEventHandler(nxGLPanel::OnMouseMoved), this, wxID_ANY);

	m_pEngine = parent->Engine();

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