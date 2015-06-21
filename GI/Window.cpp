#include "Window.h"

#include "Renderer.h"

#include <wx/wupdlock.h>

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

	m_pRenderer = new nxRenderer(m_pGLPanel);

	// Setup callbacks
	//Bind(wxEVT_SIZE, &nxFrame::onResize, this, wxID_ANY);
}
 
void nxFrame::InitRenderer()
{
	m_pRenderer->Run();
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