#include "Camera.h"

//Constructor
nxArcballCamera::nxArcballCamera(GLfloat NewWidth, GLfloat NewHeight)
{
	//Clear initial values
	m_ClickVec.x =
	m_ClickVec.y =
	m_ClickVec.z =
	
	m_DragVec.x =
	m_DragVec.y =
	m_DragVec.z = 0.0f;

	//Set initial bounds
	SetBounds(NewWidth, NewHeight);
}