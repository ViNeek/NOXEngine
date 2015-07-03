#include "Camera.h"

//Constructor
nxArcballCamera::nxArcballCamera(GLfloat NewWidth, GLfloat NewHeight)
{
	//Clear initial values
	m_ClickVec = glm::vec3(0);
	m_DragVec = glm::vec3(0);
	m_Cursor = glm::uvec2(NewWidth / 2, NewHeight / 2);
	
	//Set initial bounds
	SetBounds(NewWidth, NewHeight);
}

inline void nxArcballCamera::MapToSphere(glm::vec3& vec) const {
	glm::vec2 TempPt;
	GLfloat length;

	//Copy parameter into temp point
	TempPt = m_Cursor;
	//printf("Point %d %d\n",TempPt.x,TempPt.y);
	//Adjust point coords and scale down to range of [-1 ... 1]
	TempPt.x = (TempPt.x * m_AdjustWidth) - 1.0f;
	TempPt.y = 1.0f - (TempPt.y * m_AdjustHeight);

	//Compute the square of the length of the vector to the point from the center
	length = (TempPt.x * TempPt.x) + (TempPt.y * TempPt.y);
	//printf("Length %f \n",length);
	//If the point is mapped outside of the sphere... (length > radius squared)
	if (length > 1.0f)
	{
		GLfloat norm;

		//Compute a normalizing factor (radius / sqrt(length))
		norm = 1.0f / glm::sqrt(length);

		//Return the "normalized" vector, a point on the sphere
		vec.x = TempPt.x * norm;
		vec.y = TempPt.y * norm;
		vec.z = 0.0f;
	}
	else    //Else it's on the inside
	{
		//Return a vector to a point mapped inside the sphere sqrt(radius squared - length)
		vec.x = TempPt.x;
		vec.y = TempPt.y;
		vec.z = glm::sqrt(1.0f - length);
	}

}

//Mouse down
void nxArcballCamera::Click()
{
	//Map the point to the sphere
	MapToSphere(m_ClickVec);
}

//Mouse drag, calculate rotation
glm::vec4 nxArcballCamera::Drag()
{
	//Map the point to the sphere
	MapToSphere(m_DragVec);
	//printf("NewVec %f %f %f\n",EnVec.s.X,EnVec.s.Y,EnVec.s.Z);
	//Return the quaternion equivalent to the rotation
	glm::vec4 rotQuat;
	glm::vec3 Perp;

	//Compute the vector perpendicular to the begin and end vectors
	Perp = glm::cross(m_DragVec, m_ClickVec);

	//Compute the length of the perpendicular vector
	if (glm::length(Perp) > EPSILON)    //if its non-zero
	{
		//We're ok, so return the perpendicular vector as the transform after all
		rotQuat.x = Perp.x;
		rotQuat.y = Perp.y;
		rotQuat.z = Perp.z;
		//In the quaternion values, w is cosine (theta / 2), where theta is rotation angle
		rotQuat.w = glm::dot(m_DragVec, m_ClickVec);
	}

	return rotQuat;

}

static inline void Matrix3fSetRotationFromQuat4f(glm::mat4* NewObj, const glm::vec4* q1)
{
	GLfloat n, s;
	GLfloat xs, ys, zs;
	GLfloat wx, wy, wz;
	GLfloat xx, xy, xz;
	GLfloat yy, yz, zz;

	assert(NewObj && q1);

	n = (q1->x * q1->x) + (q1->y * q1->y) + (q1->z * q1->z) + (q1->w * q1->w);
	s = (n > 0.0f) ? (2.0f / n) : 0.0f;

	xs = q1->x * s;  ys = q1->y * s;  zs = q1->z * s;
	wx = q1->w * xs; wy = q1->w * ys; wz = q1->w * zs;
	xx = q1->x * xs; xy = q1->x * ys; xz = q1->x * zs;
	yy = q1->y * ys; yz = q1->y * zs; zz = q1->z * zs;

	(*NewObj)[0][0] = 1.0f - (yy + zz); (*NewObj)[0][1] = xy - wz;  (*NewObj)[0][2] = xz + wy; (*NewObj)[0][3] = 0.0f;
	(*NewObj)[1][0] = xy + wz;  (*NewObj)[1][1] = 1.0f - (xx + zz); (*NewObj)[1][2] = yz - wx; (*NewObj)[1][3] = 0.0f;
	(*NewObj)[2][0] = xz - wy;  (*NewObj)[2][1] = yz + wx;  (*NewObj)[2][2] = 1.0f - (xx + yy); (*NewObj)[2][3] = 0.0f;
	(*NewObj)[3][0] = 0.0f;				(*NewObj)[3][1] = 0.0f;				(*NewObj)[3][2] = 0.0f;			   (*NewObj)[3][3] = 1.0f;
}

glm::mat4 nxArcballCamera::Update() {
	if ( !IsDragging() )												// Not Dragging
	{
		if (IsClicked())												// First Click
		{
			SetDragged( true );										// Prepare For Dragging
			m_LastRot = m_ActiveRot;										// Set Last Static Rotation To Last Dynamic One
			Click();								// Update Start Vector And Prepare For Dragging
		}
	}
	else
	{
		if (IsClicked())												// Still Clicked, So Still Dragging
		{
			glm::vec4     ThisQuat;

			ThisQuat = Drag();						// Update End Vector And Get Rotation As Quaternion
			Matrix3fSetRotationFromQuat4f(&m_ActiveRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
			m_ActiveRot *= m_LastRot;
			//Transform = ThisRot;
			//printf("Quater %f %f %f %f\n",ThisQuat.s.X,ThisQuat.s.Y,ThisQuat.s.Z,ThisQuat.s.W);
			//Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
			//Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
		}
		else														// No Longer Dragging
			SetDragged( false );
	}

	return m_ActiveRot;
}