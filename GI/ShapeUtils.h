#pragma once

#include <vector>

#include <glm/vec3.hpp>

namespace Utils {

	namespace Shape {
		std::vector<glm::vec3> generateSphereMesh(int longitude_steps, int latitude_steps);
		std::vector<glm::vec3> generateSphereMeshAt(int longitude_steps, int latitude_steps, glm::vec3 position);
	}
}