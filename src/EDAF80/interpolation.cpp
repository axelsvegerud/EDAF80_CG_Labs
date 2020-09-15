#include "interpolation.hpp"


// Linear interpolation:

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const x) {
	//! \todo Implement this function

	glm::vec2 vec2(1, x);

	glm::mat2 mat2x2(glm::vec2(1, -1), glm::vec2(0, 1)); // [col], [row]

	glm::mat3x2 mat3x2( glm::vec2(p0[0], p1[0]),
						glm::vec2(p0[1], p1[1]),
						glm::vec2(p0[2], p1[2]));

	glm::vec3 p = vec2 * mat2x2 * mat3x2;

	return p;
}


// Cubic interpolation:

glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const& p3, float const t, float const x) {
	//! \todo Implement this function



	return glm::vec3();
}
