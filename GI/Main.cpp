#ifndef _glpane_
#define _glpane_

#include "Window.h"
#include "Array.h"

// Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

// Boost 
#include <boost/log/trivial.hpp>
#include <boost/lockfree/queue.hpp>

// Bullet Linear Math
#define BT_USE_SSE

#include "bullet/btVector3.h"
#include "bullet/btMatrix3x3.h"
#include "bullet/btTransform.h"
#include "bullet/btTransformUtil.h"

// NOX Engine
#include "Utils.h"
#include "GLUtils.h"

#endif
#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/glcanvas.h"

// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

// FreeImage
#include <freeimage/FreeImage.h>

class nxApp : public wxApp
{
	virtual bool OnInit();

	// Pointer to main window frame
	nxFrame*			m_pFrame;

public:

};

IMPLEMENT_APP(nxApp)

bool nxApp::OnInit()
{
	Utils::Debug::init_debug_console();


	FreeImage_Initialise();
	std::cout << FreeImage_GetVersion();
	std::cout << FreeImage_GetCopyrightMessage();
	//FreeImage_DeInitialise();

	/*
	using boost::timer::cpu_timer;
	using boost::timer::cpu_times;
	using boost::timer::nanosecond_type;
	
	nanosecond_type const twenty_seconds(5 * 1000000000LL);
	nanosecond_type last(0);
	cpu_timer* timer = new cpu_timer();
	timer->start();
	while (1)
	{
		if (timer->is_stopped())
			timer->start();
		auto elapsed_times(timer->elapsed());
		auto elapsed(elapsed_times.system
			+ elapsed_times.user);
		if (elapsed >= twenty_seconds)
		{
			std::cout << "twenty passed" << std::endl;
			last = elapsed;
			timer->stop();
		}
		
	}
	*/

	m_pFrame = new nxFrame(wxT("NOXEngine Viewer"), 50, 50, wxSystemSettings::GetMetric(wxSYS_SCREEN_X)-100, wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)-100);

	int i = m_pFrame->GetMinWidth();

	/*
	nxArray<float, 3> floats;
	nxArray<float, i, 4> floats2;

	floats[0] = 1.5f;
	floats2[0][0] = 2.5f;
	printf("%g\n", floats[0]);
	printf("%g\n", floats2[0][0]);
	//printf("%d\n", nxArray<float, 3, i, 5, 6 ,7>);
	*/

	// Show frame
	m_pFrame->Show();

	// Fire the engine
	// This needs to be after Frame->Show() inorder
	// for the wxGLContext to be safely initialised
	m_pFrame->EngineStart();
	
	return true;
}

/*
BasicGLPane::BasicGLPane(wxFrame* parent, int* args) :
wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	m_context = new wxGLContext(this);
	// prepare a simple cube to demonstrate 3D render
	// source: http://www.opengl.org/resources/code/samples/glut_examples/examples/cube.c
	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

	// To avoid flashing on MSW
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

BasicGLPane::~BasicGLPane()
{
	delete m_context;
}

void BasicGLPane::resized(wxSizeEvent& evt)
{
	//	wxGLCanvas::OnSize(evt);

	Refresh();
}

void BasicGLPane::prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
	glClearDepth(1.0f);	// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST); // Enables Depth Testing
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_COLOR_MATERIAL);

	glViewport(topleft_x, topleft_y, bottomrigth_x - topleft_x, bottomrigth_y - topleft_y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float ratio_w_h = (float)(bottomrigth_x - topleft_x) / (float)(bottomrigth_y - topleft_y);
	//gluPerspective(45 , ratio_w_h, 0.1, 200 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void BasicGLPane::prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
	glEnable(GL_TEXTURE_2D);   // textures
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(topleft_x, topleft_y, bottomrigth_x - topleft_x, bottomrigth_y - topleft_y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//gluOrtho2D(topleft_x, bottomrigth_x, bottomrigth_y, topleft_y);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int BasicGLPane::getWidth()
{
	return GetSize().x;
}

int BasicGLPane::getHeight()
{
	return GetSize().y;
}


void BasicGLPane::render(wxPaintEvent& evt)
{
	if (!IsShown()) return;

	wxGLCanvas::SetCurrent(*m_context);
	wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ------------- draw some 2D ----------------
	prepare2DViewport(0, 0, getWidth() / 2, getHeight());
	glLoadIdentity();

	// white background
	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(getWidth(), 0, 0);
	glVertex3f(getWidth(), getHeight(), 0);
	glVertex3f(0, getHeight(), 0);
	glEnd();

	// red square
	glColor4f(1, 0, 0, 1);
	glBegin(GL_QUADS);
	glVertex3f(getWidth() / 8, getHeight() / 3, 0);
	glVertex3f(getWidth() * 3 / 8, getHeight() / 3, 0);
	glVertex3f(getWidth() * 3 / 8, getHeight() * 2 / 3, 0);
	glVertex3f(getWidth() / 8, getHeight() * 2 / 3, 0);
	glEnd();

	// ------------- draw some 3D ----------------
	prepare3DViewport(getWidth() / 2, 0, getWidth(), getHeight());
	glLoadIdentity();

	glColor4f(0, 0, 1, 1);
	glTranslatef(0, 0, -5);
	glRotatef(50.0f, 0.0f, 1.0f, 0.0f);

	glColor4f(1, 0, 0, 1);
	for (int i = 0; i < 6; i++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glEnd();
	}

	glFlush();
	SwapBuffers();
}
*/