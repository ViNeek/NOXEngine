#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class nxLight {

private:

    glm::vec3 m_Center;
    glm::vec3 m_Color;
    nxFloat32 m_LinearAtt;
    glm::mat4 m_Transformation;

public:

	nxLight();
    nxLight(glm::vec3 position, glm::mat4 transform) : m_Center(position), m_Transformation(transform), m_Color(glm::vec3(1, 1, 1)), m_LinearAtt(0.1f) { };

	glm::mat4 View() { return m_Transformation; }
	glm::vec3 Center() { return m_Center; }

};

class nxSpotlight : public nxLight {

private:

    nxFloat32   m_ConeAngle;

public:

	nxSpotlight();
	nxSpotlight(glm::vec3 position, glm::mat4 transform) : nxLight(position, transform), m_ConeAngle(45.0f) { };

    nxFloat32 ConeAngle() { return m_ConeAngle; }
};