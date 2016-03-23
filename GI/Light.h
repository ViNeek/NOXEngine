#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class nxLight {

private:

	glm::vec3 m_Center;
	glm::mat4 m_Transformation;

public:

	nxLight();
	nxLight(glm::vec3 position, glm::mat4 transform);

	glm::mat4 View() { return m_Transformation; }
	glm::vec3 Center() { return m_Center; }

};

class nxSpotlight : public nxLight {

private:


public:

	nxSpotlight();
	nxSpotlight(glm::vec3 position, glm::mat4 transform) : nxLight(position, transform) { };

};