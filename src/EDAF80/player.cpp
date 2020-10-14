#include "player.hpp"
#include "parametric_shapes.hpp"
#include <core\node.hpp>
#include "../core/helpers.hpp"
#include <math.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <config.hpp>


Player::Player(const GLuint* program, const std::function<void(GLuint)>& set_uniforms) {

	// Load the object:
	std::vector<bonobo::mesh_data> const objects = bonobo::loadObjects(config::resources_path("plane/11665_Airplane_v1_l3.obj"));
	if (objects.empty()) {
		printf("Failed to load the sphere geometry: exiting.\n");
	}

	// Load the texture:
	auto airplane_texture = bonobo::loadTexture2D(config::resources_path("plane/11665_Airplane_diff.jpg"));

	// Set the shape:
	bonobo::mesh_data const& plane = objects.front();

	baseNode.set_geometry(plane);
	baseNode.set_program(program, set_uniforms);
	baseNode.add_texture("diffuse_texture", airplane_texture, GL_TEXTURE_2D);
}

void Player::render(glm::mat4 worldToClipMatrix) {
	baseNode.get_transform().SetScale(0.07);
	baseNode.get_transform().Rotate(glm::pi<double>() * -0.5, glm::vec3(0.0, 0.0, 1.0));
	baseNode.get_transform().Rotate(glm::pi<double>() * -0.55, glm::vec3(0.0, 1.0, 0.0));
	baseNode.render(worldToClipMatrix);
}

glm::vec3 Player::get_position() {
	return baseNode.get_transform().GetTranslation();
}

glm::vec3 Player::get_direction() {
	return direction;
}

// The controls:
void Player::update(InputHandler inputHandler, float delta) {
	glm::vec3 force_lift = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 force_thrust = glm::vec3(0.0, 0.0, 0.0);
	baseNode.get_transform().LookAt(this->get_position() + direction, glm::vec3(0.0, 1.0, 0.0));


	if (inputHandler.GetKeycodeState(GLFW_KEY_SPACE) & PRESSED) {
		if (abs(velocity) < 1) {
			velocity += 0.005;
		}
	}
	else {
		velocity -= 0.0002;
		if (velocity < 0) {
			velocity = 0;
		}
	}

	if (inputHandler.GetKeycodeState(GLFW_KEY_Q) & PRESSED) {
		if (abs(velocity) < 1) {
			velocity -= 0.005;
		}
	}

	if (inputHandler.GetKeycodeState(GLFW_KEY_S) & PRESSED) {
		angleX -= 0.005;
		if (angleX < -0.5) {
			angleX = -0.5;
		}

	}
	if (inputHandler.GetKeycodeState(GLFW_KEY_W) & PRESSED) {
		angleX += 0.005;
		if (angleX > 0.5) {
			angleX = 0.5;
		}
	}
	angleY = 0;
	bool angleZchanged = false;
	if (inputHandler.GetKeycodeState(GLFW_KEY_D) & PRESSED) {
		angleZ -= 0.01;
		if (angleZ < -0.5) {
			angleZ = -0.5;
		}
		angleZchanged = true;
	}
	if (inputHandler.GetKeycodeState(GLFW_KEY_A) & PRESSED) {
		angleZ += 0.01;
		if (angleZ > 0.5) {
			angleZ = 0.5;
		}
		angleZchanged = true;

	}
	if (!angleZchanged) {
		if (angleZ < 0) {
			angleZ += 0.01;
			if (angleZ > 0) {
				angleZ = 0;
			}
		}
		else if (angleZ > 0) {
			angleZ -= 0.01;
			if (angleZ < 0) {
				angleZ = 0;
			}
		}
	}
	force_thrust.x = direction.x;
	force_thrust.z = direction.z;

	force_thrust.y = glm::sin(glm::pi<double>() * angleX);
	force_lift.x = -glm::sin(glm::pi<double>() * (angleZ / 75.0f));

	force_lift = baseNode.get_transform().GetRotation() * force_lift;
	direction = force_thrust + force_lift;
	direction = normalize(direction);
	baseNode.get_transform().LookAt(this->get_position() + direction, glm::vec3(0.0, 1.0, 0.0));

	baseNode.get_transform().Rotate(glm::pi<double>() * angleZ, glm::vec3(0.0, 0.0, 1.0));

	baseNode.get_transform().Translate(direction * velocity * 60.0f * delta);
}
