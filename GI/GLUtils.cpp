#include "GLUtils.h"

#include <boost/log/trivial.hpp>

void Utils::GL::PerspectiveMatrix(float *matrix, float fovyInDegrees, float aspectRatio,
	float znear, float zfar)
{
	/*float ymax, xmax;
	float temp, temp2, temp3, temp4;
	ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
	//ymin = -ymax;
	//xmin = -ymax * aspectRatio;
	xmax = ymax * aspectRatio;
	Frustum(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);*/
}

btTransform Utils::GL::getOrtho(float left, float right, float bottom, float top,
	float znear, float zfar) {
	btTransform ret;

	float matrix[16];

	float temp, temp2, temp3, temp4;
	temp = 2.0f * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	matrix[0] = 2.0f / temp2;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;
	matrix[4] = 0.0f;
	matrix[5] = 2.0f / temp3;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;
	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = -2.0f / temp4;
	matrix[11] = 0.0f;
	matrix[12] = - (right + left) / temp2;
	matrix[13] = - (top + bottom) / temp3;
	matrix[14] = - (zfar + znear) / temp4;
	matrix[15] = 1.0f;

	ret.setFromOpenGLMatrix(matrix);

	//std::cout << "X" << ret.getBasis().getRow(0).x;
	//std::cout << "Y"  << ret.getBasis().getRow(0).y;
	//std::cout << "Z" <<ret.getBasis().getRow(0).z;

	BOOST_LOG_TRIVIAL(info) << "X" << ret.getBasis().getRow(0).x();
	BOOST_LOG_TRIVIAL(info) << "Y" << ret.getBasis().getRow(0).y();
	BOOST_LOG_TRIVIAL(info) << "Z" << ret.getBasis().getRow(0).z();
	BOOST_LOG_TRIVIAL(info) << "W" << ret.getBasis().getRow(0).w();

	BOOST_LOG_TRIVIAL(info) << "X" << ret.getBasis().getRow(1).x();
	BOOST_LOG_TRIVIAL(info) << "Y" << ret.getBasis().getRow(1).y();
	BOOST_LOG_TRIVIAL(info) << "Z" << ret.getBasis().getRow(1).z();
	BOOST_LOG_TRIVIAL(info) << "W" << ret.getBasis().getRow(1).w();

	BOOST_LOG_TRIVIAL(info) << "X" << ret.getBasis().getRow(2).x();
	BOOST_LOG_TRIVIAL(info) << "Y" << ret.getBasis().getRow(2).y();
	BOOST_LOG_TRIVIAL(info) << "Z" << ret.getBasis().getRow(2).z();
	BOOST_LOG_TRIVIAL(info) << "W" << ret.getBasis().getRow(2).w();
	
	ret.getOpenGLMatrix(matrix);

	for (int i = 0; i < 16; i++)
	{
		BOOST_LOG_TRIVIAL(info) << "A" << matrix[i];
	}

	return ret;
}

void Utils::GL::Frustum(float *matrix, float left, float right, float bottom, float top,
	float znear, float zfar)
{
	float temp, temp2, temp3, temp4;
	temp = 2.0f * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	matrix[0] = temp / temp2;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = temp / temp3;
	matrix[6] = 0.0;
	matrix[7] = 0.0;
	matrix[8] = (right + left) / temp2;
	matrix[9] = (top + bottom) / temp3;
	matrix[10] = (-zfar - znear) / temp4;
	matrix[11] = -1.0;
	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = (-temp * zfar) / temp4;
	matrix[15] = 0.0;
}
