#pragma once

#include <vector>
#include <map>

#include <boost/lockfree/queue.hpp>

#include <GL/glew.h>

#include "Program.h"
#include "wx/wx.h"
#include "wx/glcanvas.h"

enum nxRendererState {
	NX_RENDERER_OFF = (1 << 0),
	NX_RENDERER_EXTENSIONS_READY = (1 << 1),
	NX_RENDERER_FRAMEBUFFER_READY = (1 << 2),
	NX_RENDERER_DRAW_READY = NX_RENDERER_FRAMEBUFFER_READY | NX_RENDERER_EXTENSIONS_READY
};

class nxGLJob;
class nxEngine;
class nxProgram;
class nxVoxelizer;

typedef boost::lockfree::queue< nxGLJob* > nxGLJobQueue;

class nxRenderer : public wxThread
{
public:

											nxRenderer(nxEngine* eng);
											nxRenderer(wxGLCanvas* frame);

	void*									Entry();

	void									ScheduleGLJob(nxGLJob* job) { m_pGLCommandQueue->push(job); };
	void									SetDrawingCanvas(wxGLCanvas* frame) { m_pParent = frame; }
	bool									IsFramebufferReady() { return m_FBOInited; }

	void									InitFramebuffer();
	void									ResizeFramebuffer();
	void									UseProgram();
	//nxProgram*							Program() { return m_ShaderPrograms[m_ProgramIndex]; }
	//nxProgram*								Program() { return m_ShaderPrograms[m_ProgramName]; }
	nxProgram*								Program() { return m_pActiveProgram; }
	nxVoxelizer*							Voxelizer() { return m_Voxelizer; }

	bool									InitExtensions();
	bool									VoxelizerReady() { return m_IsVoxelizerReady; };
	void									SetVoxelizerReady(bool val) { m_IsVoxelizerReady = val; };

	void									SetViewportSize(int w, int h) { m_VWidth = w; m_VHeight = h; };
	void									SetVoxelizer(nxVoxelizer* voxel) { m_Voxelizer = voxel; };

	int										Width() { return m_VWidth; }
	int										Height() { return m_VHeight; }

	void									AddProgram(nxProgram* prog) { m_ShaderPrograms[prog->GetName()] = prog; }

	void									SetActiveProgramByName(const std::string& name);
	nxProgram*								GetActiveProgramByName(const std::string& name) { return m_ShaderPrograms[name]; }
	void									SetProgram(nxProgram* prog) { m_pActiveProgram = prog; }
	void									SetProgramName(const std::string& name) { m_ProgramName = name; }
	void									SetActiveProgram(int index) { m_ProgramIndex = 0; }

	std::map<std::string, nxProgram*>		GetPrograms() { return m_ShaderPrograms; }
private:

	bool									m_IsActive;
	wxGLCanvas*								m_pParent;
	wxGLContext*							m_pGLCtx;
	nxGLJobQueue*							m_pGLCommandQueue;

	nxEngine*								m_pEngine;
	nxVoxelizer*							m_Voxelizer;

	GLuint									m_FBO;
	GLuint									m_RBO;
	GLuint									m_DepthTexture;
	GLuint									m_ssbo;

	bool									m_FBOInited;
	bool									m_IsVoxelizerReady;

	unsigned int							m_State;

	int										m_VWidth;
	int										m_VHeight;
	int										m_ProgramIndex;
	std::string								m_ProgramName;

	nxProgram*								m_pActiveProgram;

	//std::vector<nxProgram*>				m_ShaderPrograms;
	std::map<std::string, nxProgram*>		m_ShaderPrograms;

	void									Init();
	void									RenderFrame();
	void									RenderFrameDemo();
	void									RenderDebugFrame();
	void									SwapBuffers() { m_pParent->SwapBuffers(); }

};