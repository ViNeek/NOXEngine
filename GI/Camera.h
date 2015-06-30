#pragma once

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"

#define EPSILON 10e-5

class nxArcballCamera
{

public:
	
	//Create/Destroy
	nxArcballCamera(GLfloat NewWidth, GLfloat NewHeight);
	nxArcballCamera() { /* nothing to do */ };

	//Set new bounds
	inline void    SetBounds(GLfloat NewWidth, GLfloat NewHeight)
	{
		if (!((NewWidth > 1.0f) && (NewHeight > 1.0f))) {
			NewWidth = 1.0f;
			NewHeight = 1.0f;
		}

		//Set adjustment factor for width/height
		m_AdjustHeight = 1.0f / ((NewWidth - 1.0f) * 0.5f);
		m_AdjustWidth = 1.0f / ((NewHeight - 1.0f) * 0.5f);
	}

	//Mouse down
	void			Click(const glm::uvec2* NewPt);

	//Mouse drag, calculate rotation
	void			Drag(const glm::uvec2* NewPt, glm::vec4* NewRot);
	glm::mat4		Update(glm::uvec2 &cursor);
	void			SetPosition(float x, float y, float z) { m_Position = glm::vec3(x, y, -z); };
	glm::vec3&		Position(float x, float y, float z) { return m_Position; };

private:

	inline void		MapToSphere(const glm::uvec2* NewPt, glm::vec3* NewVec) const;

	bool			m_Clicked;
	bool			m_Dragging;
	glm::vec3		m_ClickVec;          //Saved click vector
	glm::vec3		m_DragVec;          //Saved drag vector
	glm::vec3		m_Position;
	glm::mat4		m_ActiveRot;
	glm::mat4		m_LastRot;
	GLfloat			m_AdjustWidth;    //Mouse bounds width
	GLfloat			m_AdjustHeight;   //Mouse bounds height

};