#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include <boost/align/align.hpp>
#include <boost/align/aligned_alloc.hpp>
#include <boost/align/is_aligned.hpp>

#define EPSILON 10e-5

class nxArcballCamera
{

public:
	
					//Create/Destroy
					nxArcballCamera(GLfloat NewWidth, GLfloat NewHeight);
					nxArcballCamera() { /* nothing to do */ };

	void*			operator new(size_t i)
	{
		//void *p = _mm_malloc(i, 16);
		void *p = boost::alignment::aligned_alloc(16, i);
		assert(boost::alignment::is_aligned(16, p));
		return p;
	}

	void			operator delete(void* p)
	{
		boost::alignment::aligned_free(p);
		//_mm_free(p);
	}

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
	void					Click();


	void					SetClicked(bool clicked) { m_Clicked = clicked; }
	void					SetDragged(bool dragging) { m_Dragging = dragging; };
	bool					IsClicked() { return m_Clicked; }
	bool					IsDragging() { return m_Dragging; };

	//Mouse drag, calculate rotation
	glm::vec4				Drag();
	glm::mat4				Update();
	void					SetPosition(float x, float y, float z) { m_Position = glm::vec3(-x, -y, -z); };
	void					SetX(float x) { m_Position.x = x; };
	void					SetY(float y) { m_Position.y = y; };
	void					SetZ(float z) { m_Position.z = z; };
	glm::vec3&				Position() { return m_Position; };

	const glm::uvec2&		Cursor() { return m_Cursor; };
	inline void				SetCursor(int x, int y) { m_Cursor.x = x; m_Cursor.y = y; };

private:

	inline void		MapToSphere(glm::vec3& vec) const;

	bool					m_Clicked;
	bool					m_Dragging;
	glm::vec3				m_ClickVec;          //Saved click vector
	glm::vec3				m_DragVec;          //Saved drag vector
	glm::vec3				m_Position;
	glm::mat4				m_ActiveRot;
	glm::mat4				m_LastRot;

	glm::uvec2				m_Cursor;

	GLfloat					m_AdjustWidth;    //Mouse bounds width
	GLfloat					m_AdjustHeight;   //Mouse bounds height

};