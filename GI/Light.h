#pragma once

#include <glm/vec3.hpp>

class nxLight {

private:

	glm::vec3 m_Center;

public:

	nxLight();
	nxLight(glm::vec3 position);

};

class nxSpotlight : nxLight {

private:

	glm::vec3 m_Orientation;

public:

	nxSpotlight();
	nxSpotlight(glm::vec3 position, glm::vec3 orient_axis) : nxLight(position), m_Orientation(orient_axis);

};