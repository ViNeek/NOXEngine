#include "ShapeUtils.h"

#include <glm/trigonometric.hpp>

std::vector<glm::vec3> Utils::Shape::generateSphereMesh(int longitude_steps, int latitude_steps)
{
	// create the sphere
	std::vector<glm::vec3> sphere_data;
	const float PI = 3.141592f;

	if (longitude_steps < 2) longitude_steps = 2;
	if (latitude_steps < 4) latitude_steps = 4;
	float sphere_radius = 1.0f;
	float phi_step = 2 * PI / float(latitude_steps);
	float theta_step = PI / float(longitude_steps);
	float phi = 0;
	float theta = 0;

	for (int j = 0; j < longitude_steps; j++)
	{
		// temp variables to avoid calculating trigonometric values many times
		float costheta = glm::cos(theta);
		float sintheta = glm::sin(theta);
		float costheta_plus_step = glm::cos(theta + theta_step);
		float sintheta_plus_step = glm::sin(theta + theta_step);

		phi = 0;
		for (int i = 0; i < latitude_steps; i++)
		{
			// temp variables to avoid calculating trigonometric values many times
			float cosphi = glm::cos(phi);
			float sinphi = glm::sin(phi);
			float cosphi_plus_step = glm::cos(phi + phi_step);
			float sinphi_plus_step = glm::sin(phi + phi_step);

			glm::vec3 top_left_vertex(1);
			top_left_vertex.x = sphere_radius * sintheta * cosphi;
			top_left_vertex.z = sphere_radius * sintheta * sinphi;
			top_left_vertex.y = sphere_radius * costheta;

			glm::vec3 top_right_vertex(1);
			top_right_vertex.x = sphere_radius * sintheta * cosphi_plus_step;
			top_right_vertex.z = sphere_radius * sintheta * sinphi_plus_step;
			top_right_vertex.y = sphere_radius * costheta;

			glm::vec3 bottom_left_vertex(1);
			bottom_left_vertex.x = sphere_radius * sintheta_plus_step * cosphi;
			bottom_left_vertex.z = sphere_radius * sintheta_plus_step * sinphi;
			bottom_left_vertex.y = sphere_radius * costheta_plus_step;

			glm::vec3 bottom_right_vertex(1);
			bottom_right_vertex.x = sphere_radius * sintheta_plus_step * cosphi_plus_step;
			bottom_right_vertex.z = sphere_radius * sintheta_plus_step * sinphi_plus_step;
			bottom_right_vertex.y = sphere_radius * costheta_plus_step;

			sphere_data.push_back(bottom_left_vertex);
			sphere_data.push_back(bottom_right_vertex);
			sphere_data.push_back(top_right_vertex);
			sphere_data.push_back(top_right_vertex);
			sphere_data.push_back(top_left_vertex);
			sphere_data.push_back(bottom_left_vertex);

			phi += phi_step;
		}

		theta += theta_step;
	}

	return sphere_data;
}

std::vector<glm::vec3> Utils::Shape::generateSphereMeshAt(int longitude_steps, int latitude_steps, glm::vec3 position)
{
	// create the sphere
	std::vector<glm::vec3> sphere_data;
	const float PI = 3.141592f;

	if (longitude_steps < 2) longitude_steps = 2;
	if (latitude_steps < 4) latitude_steps = 4;
	float sphere_radius = 1.0f;
	float phi_step = 2 * PI / float(latitude_steps);
	float theta_step = PI / float(longitude_steps);
	float phi = 0;
	float theta = 0;

	for (int j = 0; j < longitude_steps; j++)
	{
		// temp variables to avoid calculating trigonometric values many times
		float costheta = glm::cos(theta);
		float sintheta = glm::sin(theta);
		float costheta_plus_step = glm::cos(theta + theta_step);
		float sintheta_plus_step = glm::sin(theta + theta_step);

		phi = 0;
		for (int i = 0; i < latitude_steps; i++)
		{
			// temp variables to avoid calculating trigonometric values many times
			float cosphi = glm::cos(phi);
			float sinphi = glm::sin(phi);
			float cosphi_plus_step = glm::cos(phi + phi_step);
			float sinphi_plus_step = glm::sin(phi + phi_step);

			glm::vec3 top_left_vertex(1);
			top_left_vertex.x = sphere_radius * sintheta * cosphi;
			top_left_vertex.z = sphere_radius * sintheta * sinphi;
			top_left_vertex.y = sphere_radius * costheta;

			glm::vec3 top_right_vertex(1);
			top_right_vertex.x = sphere_radius * sintheta * cosphi_plus_step;
			top_right_vertex.z = sphere_radius * sintheta * sinphi_plus_step;
			top_right_vertex.y = sphere_radius * costheta;

			glm::vec3 bottom_left_vertex(1);
			bottom_left_vertex.x = sphere_radius * sintheta_plus_step * cosphi;
			bottom_left_vertex.z = sphere_radius * sintheta_plus_step * sinphi;
			bottom_left_vertex.y = sphere_radius * costheta_plus_step;

			glm::vec3 bottom_right_vertex(1);
			bottom_right_vertex.x = sphere_radius * sintheta_plus_step * cosphi_plus_step;
			bottom_right_vertex.z = sphere_radius * sintheta_plus_step * sinphi_plus_step;
			bottom_right_vertex.y = sphere_radius * costheta_plus_step;

			sphere_data.push_back(bottom_left_vertex);
			sphere_data.push_back(bottom_right_vertex);
			sphere_data.push_back(top_right_vertex);
			sphere_data.push_back(top_right_vertex);
			sphere_data.push_back(top_left_vertex);
			sphere_data.push_back(bottom_left_vertex);

			phi += phi_step;
		}

		theta += theta_step;
	}

	for (auto& vert : sphere_data) {
		vert += position;
	}

	return sphere_data;
}