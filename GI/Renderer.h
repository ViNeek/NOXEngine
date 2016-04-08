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
class nxDistanceField;
class nxRayMarcher;
class nxReflectiveShadowMap;
typedef nxReflectiveShadowMap nxRSM;

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
	//nxProgram*							Program() { return m_ShaderPrograms[m_ProgramName]; }
	nxProgram*								Program() { return m_pActiveProgram; }
	nxVoxelizer*							Voxelizer() { return m_Voxelizer; }
	nxDistanceField*						DistanceField() { return m_DistanceField; }
	nxRayMarcher*							RayMarcher() { return m_RayMarcher; }
	nxRSM*									RSM() { return m_RSM; }

	bool									InitExtensions();
	bool									VoxelizerReady() { return m_IsVoxelizerReady; };
	void									SetVoxelizerReady(bool val) { m_IsVoxelizerReady = val; };
	bool									IsVoxelizing() { return m_Voxelizing; };
	void									SetVoxelizing(bool val) { m_Voxelizing = val; };

	void									SetViewportSize(int w, int h) { m_VWidth = w; m_VHeight = h; };
	void									SetVoxelizer(nxVoxelizer* voxel) { m_Voxelizer = voxel; };
	void									SetDistanceField(nxDistanceField* df) { m_DistanceField = df; };
	void									SetRSM(nxRSM* rsm) { m_RSM = rsm; };
	void									SetRayMarcher(nxRayMarcher* rm) { m_RayMarcher = rm; };

	int										Width() { return m_VWidth; }
	int										Height() { return m_VHeight; }

	void									AddProgram(nxProgram* prog) { m_ShaderPrograms[prog->GetName()] = prog; }

	void									SetActiveProgramByName(const std::string& name);
	nxProgram*								GetActiveProgramByName(const std::string& name) { return m_ShaderPrograms[name]; }
	void									SetProgram(nxProgram* prog) { m_pActiveProgram = prog; }
	void									SetProgramName(const std::string& name) { m_ProgramName = name; }
	void									SetActiveProgram(int index) { m_ProgramIndex = 0; }

	std::map<std::string, nxProgram*>		GetPrograms() { return m_ShaderPrograms; }

	GLuint									m_ssbo;

private:

	wxGLCanvas*								m_pParent;
	wxGLContext*							m_pGLCtx;
	nxGLJobQueue*							m_pGLCommandQueue;

	nxEngine*								m_pEngine;
	nxVoxelizer*							m_Voxelizer;
	nxDistanceField*						m_DistanceField;
	nxRayMarcher*							m_RayMarcher;
	nxRSM*									m_RSM;

	GLuint									m_FBO;
	GLuint									m_RBO;
	GLuint									m_DepthTexture;

	bool									m_FBOInited;
	bool									m_IsVoxelizerReady;
	bool									m_IsActive;
	bool									m_Voxelizing;
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