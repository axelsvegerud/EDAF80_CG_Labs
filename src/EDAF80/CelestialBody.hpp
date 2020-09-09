#pragma once

#include "assignment1.hpp"
#include "core/helpers.hpp"
#include "../core/node.hpp"

//Test!

class CelestialBody {
public:

	// - shape contains information about the geometry used to model the celestial body.
	// - program is the shader program used to render the celestial body.
	// - diffuse_texture_id is the identifer of the difuse texture used.

	CelestialBody(bonobo::mesh_data const& shape, GLuint const* program, GLuint diffuse_texture_id);

	// - The returned matrix is the one used as the parent_transform for that body's children.
	// - ellapsed_time is the amount of time (in microseconds) between two frames.
	// - view_projection is the matrix for going from world-space to clip-space.
	// - parent_transform transforms from the local-space of your parent, to world-space, and defaults to the identity matrix if unspecifed.

	glm::mat4 render(std::chrono::microseconds ellapsed_time, glm::mat4 const& view_projection, glm::mat4 const& parent_transform = glm::mat4(1.0f));

	void set_scale(glm::vec3 const& scale);

	// Problem when using struct!!!
	// void set_spin(SpinConfiguration const& configuration);

	// void set_orbit(OrbitConfiguration const& configuration);

	void set_spin(float spin_inclination, float spin_speed);

	void set_orbit(float orbit_radius, float orbit_inclination, float orbit_speed);

	void set_ring(bonobo::mesh_data const& shape, GLuint const* program, GLuint diffust_texture_id, glm::vec2 const& scale = glm::vec2(1.0f));

	/* Will append child to a new private attribute of type
	std::vector<CelestialBody*>, by using the attribute's
	push_back(CelestialBody * item) method. */
	void add_child(CelestialBody* child);

	// "Will simply return the attribute you created in the previous step."
	std::vector<CelestialBody*> const& get_children() const;

	glm::mat4 get_transform() const;

private:
	// private nodes from the class Node.
	Node _body;
	Node _ring_node;

	// Vectors:
	glm::vec3 _body_scale = glm::vec3(1.0f);
	glm::vec2 _ring_scale = glm::vec2(1.0f);
	std::vector<CelestialBody*> _child_nodes;

	// Matrices:
	glm::mat4 _transform_matrix;

	// Floats:
	float _spin_angle = 0.0f;
	float _spin_inclination = 0.0f;
	float _spin_speed = 0.0f;
	float _orbit_angle = 0.0f;
	float _orbit_inclination = 0.0f;
	float _orbit_speed = 0.0f;
	float _orbit_radius = 0.0f;

};
