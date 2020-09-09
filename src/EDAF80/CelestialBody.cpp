#include "CelestialBody.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"


CelestialBody::CelestialBody(bonobo::mesh_data const& shape, GLuint const* program, GLuint diffuse_texture_id) {

	_body.set_geometry(shape);
	_body.set_program(program);
	_body.add_texture("diffuse_texture", diffuse_texture_id, GL_TEXTURE_2D);
}

glm::mat4 CelestialBody::render(std::chrono::microseconds ellapsed_time, glm::mat4 const& view_projection, glm::mat4 const& parent_transform) {

	std::chrono::duration<float> const ellapsed_time_s = ellapsed_time;
	_spin_angle += ellapsed_time_s.count() * _spin_speed;

	_orbit_angle += ellapsed_time_s.count() * _orbit_speed;


	// ***SCALING:***

	// Scaling matrix
	glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _body_scale);

	// ***SPINNING:***

	// Spin matrix
	glm::mat4 spin_matrix = glm::rotate(glm::mat4(1.0f), _spin_angle, glm::vec3(0, 1, 0));

	// Tilt spin matrix
	// "rotation by the inclination of the spin around the z-axis."
	glm::mat4 tilt_spin_matrix = glm::rotate(glm::mat4(1.0f), _spin_inclination, glm::vec3(0, 0, 1));

	// ***ORBITING:***

	// Orbit matrix
	// "rotate around the y-axis."
	glm::mat4 orbit_matrix = glm::rotate(glm::mat4(1.0f), _orbit_angle, glm::vec3(0, 1, 0));

	// Translation matrix
	// "offset the planet by the radius of the orbit along the x-axis."
	glm::mat4 translation_matrix = glm::mat4(1.0f);
	translation_matrix[3][0] = _orbit_radius;

	// Tilt orbit matrix
	// "Tilt according to th orbit inclination angle => rotation around z-axis
	glm::mat4 tilt_orbit_matrix = glm::rotate(glm::mat4(1.0f), _orbit_inclination, glm::vec3(0, 0, 1));

	// Transform matrix
	_transform_matrix = tilt_orbit_matrix * orbit_matrix * translation_matrix;

	// Combination of the matrices
	// Rotate before scaling
	glm::mat4 matrix = _transform_matrix * tilt_spin_matrix * spin_matrix * scale_matrix;

	// Combination of the matrices for rings.
	// Rings should be rotated 90 deg around x-axis.
	glm::mat4 matrix_rings =  _transform_matrix * tilt_spin_matrix * spin_matrix * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0)) * scale_matrix;

	_body.render(view_projection, matrix);

	_ring_node.render(view_projection, matrix_rings);

	return parent_transform * _transform_matrix;
}

void CelestialBody::set_scale(glm::vec3 const& scale) {
	_body_scale = scale;
}

/*
Problem when using struct!!!
void CelestialBody::set_spin(SpinConfiguration const& configuration) {
	_spin_configuration = configuration;
}
*/

void CelestialBody::set_spin(float spin_inclination, float spin_speed) {
	_spin_inclination = spin_inclination;
	_spin_speed = spin_speed;
}

void CelestialBody::set_orbit(float orbit_radius, float orbit_inclination, float orbit_speed) {
	_orbit_radius = orbit_radius;
	_orbit_inclination = orbit_inclination;
	_orbit_speed = orbit_speed;
}

void CelestialBody::set_ring(bonobo::mesh_data const& shape, GLuint const* program, GLuint diffust_texture_id, glm::vec2 const& scale) {
	_ring_node.set_geometry(shape);
	_ring_node.set_program(program);
	_ring_node.add_texture("diffuse_texture", diffust_texture_id, GL_TEXTURE_2D);
	_ring_scale = scale;
}

void CelestialBody::add_child(CelestialBody* child) {
	_child_nodes.push_back( child );
}

std::vector<CelestialBody*> const& CelestialBody::get_children() const {
	return _child_nodes;
}

glm::mat4 CelestialBody::get_transform() const {
	return _transform_matrix;
}

