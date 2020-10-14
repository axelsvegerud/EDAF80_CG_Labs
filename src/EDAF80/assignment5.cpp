#include "assignment5.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/ShaderProgramManager.hpp"

#include <imgui.h>
#include <tinyfiledialogs.h>

#include <windows.h>
#include <stdlib.h>
#include <ctime>
#include <array>
#include <clocale>
#include <stdexcept>
#include <glm\gtc\type_ptr.hpp>
#include <core\node.hpp>
#include <EDAF80\parametric_shapes.hpp>
#include <EDAF80\player.hpp>

edaf80::Assignment5::Assignment5(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
		static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
		0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0 };

	window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}
}

void
edaf80::Assignment5::run()
{
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 10.0f));
	mCamera.mMouseSensitivity = 0.003f;
	mCamera.mMovementSpeed = 3.0f; // 3 m/s => 10.8 km/h

	// Create the shader programs
	ShaderProgramManager program_manager;
	GLuint fallback_shader = 0u;
	program_manager.CreateAndRegisterProgram("Fallback",
		{ { ShaderType::vertex, "EDAF80/fallback.vert" },
		  { ShaderType::fragment, "EDAF80/fallback.frag" } },
		fallback_shader);
	if (fallback_shader == 0u) {
		LogError("Failed to load fallback shader");
		return;
	}

	//
	// Todo: Insert the creation of other shader programs.
	//       (Check how it was done in assignment 3.)
	//

	//Phong shader:
	GLuint phong_shader = 0u;
	program_manager.CreateAndRegisterProgram("Phong",
		{ { ShaderType::vertex, "EDAF80/phong_game.vert" },
		{ ShaderType::fragment, "EDAF80/phong_game.frag" } },
		phong_shader);
	if (phong_shader == 0u) {
		LogError("Failed to load phong shader");
		return;
	}

	// Skybox shader:
	GLuint skybox_shader = 0u;
	program_manager.CreateAndRegisterProgram("Skybox",
		{ { ShaderType::vertex, "EDAF80/skybox.vert" },
		{ ShaderType::fragment, "EDAF80/skybox.frag" } },
		skybox_shader);
	if (skybox_shader == 0u) {
		LogError("Failed to load skybox shader");
		return;
	}

	// Water shader:
	GLuint water_shader = 0u;
	program_manager.CreateAndRegisterProgram("Water",
		{ { ShaderType::vertex, "EDAF80/water.vert" },
		  { ShaderType::fragment, "EDAF80/water.frag" } },
		water_shader);
	if (water_shader == 0u) {
		LogError("Failed to load water shader");
		return;
	}

	// Default shader:
	GLuint default_shader = 0u;
	program_manager.CreateAndRegisterProgram("default",
		{ { ShaderType::vertex, "EDAF80/default.vert" },
		  { ShaderType::fragment, "EDAF80/default.frag" } },
		default_shader);
	if (default_shader == 0u) {
		LogError("Failed to load default shader");
		return;
	}

	//
	// Todo: Load your geometry
	//

	// Create the shapes:
	auto torus_shape = parametric_shapes::createTorus(10.0f, 2.5f, 50u, 50u);
	auto skybox_shape = parametric_shapes::createSphere(500.0f, 1250u, 1250u);
	auto player_shape = parametric_shapes::createSphere(0.5f, 50u, 50u);
	//auto water_shape = parametric_shapes::createTessQuad(1010.0f, 1010.0f, 1010u, 1010u);

	// Load the textures:
	auto skybox_id = bonobo::loadTextureCubeMap(config::resources_path("cubemaps/cloudy/bluecloud_ft.jpg"),
		config::resources_path("cubemaps/cloudy/bluecloud_bk.jpg"),
		config::resources_path("cubemaps/cloudy/bluecloud_up.jpg"),
		config::resources_path("cubemaps/cloudy/bluecloud_dn.jpg"),
		config::resources_path("cubemaps/cloudy/bluecloud_rt.jpg"),
		config::resources_path("cubemaps/cloudy/bluecloud_lf.jpg"));
	//auto water_normal_id = bonobo::loadTexture2D(config::resources_path("textures/waves.png"));

	// Set the uniforms:
	float ellapsed_time_s = 0.0f;
	auto light_position = glm::vec3(-16000.0f, 4000.0f, 16000.0f);
	auto camera_position = mCamera.mWorld.GetTranslation();

	// Colors:
	auto ambient = glm::vec3(0.45f, 0.1f, 0.1f);
	auto specular = glm::vec3(0.4f, 0.4f, 0.4f);
	auto ambient_red = glm::vec3(0.45f, 0.0f, 0.0f);
	auto diffuse_red = glm::vec3(1.0f, 0.2f, 0.2f);
	auto ambient_green = glm::vec3(0.0f, 0.45f, 0.0f);
	auto diffuse_green = glm::vec3(0.2f, 1.0f, 0.2f);
	auto ambient_yellow = glm::vec3(1.0f, 1.0f, 0.0f);
	auto diffuse_yellow = glm::vec3(0.2f, 1.0f, 0.2f);

	// Uniforms for the waves:
	float amplitude[2] = { 1.0, 0.5 };
	float frequency[2] = { 0.2, 0.4 };
	float phase[2] = { 0.5, 1.3 };
	float sharpness[2] = { 2.0, 2.0 };
	auto shininess = 200.0f;
	glm::vec2 direction[2] = { glm::vec2(-1.0, 0.0), glm::vec2(-0.7, 0.7) };

	auto const set_uniforms = [&light_position](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
	};

	auto const phong_set_uniforms_red = [&light_position, &camera_position, &ambient_red, &diffuse_red, &specular, &shininess](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform3fv(glGetUniformLocation(program, "ambient"), 1, glm::value_ptr(ambient_red));
		glUniform3fv(glGetUniformLocation(program, "diffuse"), 1, glm::value_ptr(diffuse_red));
		glUniform3fv(glGetUniformLocation(program, "specular"), 1, glm::value_ptr(specular));
		glUniform1f(glGetUniformLocation(program, "shininess"), shininess);
	};

	auto const phong_set_uniforms_green = [&light_position, &camera_position, &ambient_green, &diffuse_green, &specular, &shininess](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform3fv(glGetUniformLocation(program, "ambient"), 1, glm::value_ptr(ambient_green));
		glUniform3fv(glGetUniformLocation(program, "diffuse"), 1, glm::value_ptr(diffuse_green));
		glUniform3fv(glGetUniformLocation(program, "specular"), 1, glm::value_ptr(specular));
		glUniform1f(glGetUniformLocation(program, "shininess"), shininess);
	};

	auto const phong_set_uniforms_yellow = [&light_position, &camera_position, &ambient_yellow, &diffuse_yellow, &specular, &shininess](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform3fv(glGetUniformLocation(program, "ambient"), 1, glm::value_ptr(ambient_yellow));
		glUniform3fv(glGetUniformLocation(program, "diffuse"), 1, glm::value_ptr(diffuse_yellow));
		glUniform3fv(glGetUniformLocation(program, "specular"), 1, glm::value_ptr(specular));
		glUniform1f(glGetUniformLocation(program, "shininess"), shininess);
	};

	auto const plane_set_uniforms = [&light_position, &camera_position, &ambient, &specular, &shininess](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform3fv(glGetUniformLocation(program, "ambient"), 1, glm::value_ptr(ambient));
		glUniform3fv(glGetUniformLocation(program, "specular"), 1, glm::value_ptr(specular));
		glUniform1f(glGetUniformLocation(program, "shininess"), shininess);
	};

	/*
	auto const water_set_uniforms = [&amplitude, &frequency, &sharpness, &phase, &direction, &ellapsed_time_s, &camera_position, &light_position](GLuint program) {
		glUniform1fv(glGetUniformLocation(program, "amplitude"), 2, amplitude);
		glUniform1fv(glGetUniformLocation(program, "frequency"), 2, frequency);
		glUniform1fv(glGetUniformLocation(program, "sharpness"), 2, sharpness);
		glUniform1fv(glGetUniformLocation(program, "phase"), 2, phase);
		glUniform2fv(glGetUniformLocation(program, "direction"), 2, glm::value_ptr(direction[0]));
		glUniform1f(glGetUniformLocation(program, "ellapsed_time"), ellapsed_time_s);
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
	};


	// Create the water:
	Node water;
	water.set_geometry(water_shape);
	water.set_program(&water_shader, water_set_uniforms);
	water.add_texture("skybox", skybox_id, GL_TEXTURE_CUBE_MAP);
	water.add_texture("normal_map", water_normal_id, GL_TEXTURE_2D);
	water.get_transform().SetTranslate(glm::vec3(-500.0f, -40.0f, -965.0f));
	*/

	// Create the sky:
	Node skybox;
	skybox.set_geometry(skybox_shape);
	skybox.set_program(&skybox_shader, set_uniforms);
	skybox.add_texture("skybox", skybox_id, GL_TEXTURE_CUBE_MAP);
	skybox.get_transform().SetTranslate(glm::vec3(0.0f, 0.0f, -460.0f));


	// Create the player:
	Player player = Player(&default_shader, plane_set_uniforms);

	// Create the path for the rings:
	std::vector<glm::vec3> torus_point_locations(9);

	float x_next = 0.0f;
	float z_dist = -100.0f;

	auto Time = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::microseconds>(Time.time_since_epoch()).count();

	std::srand(t);
	for (int i = 0; i < torus_point_locations.size(); i++) {
		torus_point_locations[i] = glm::vec3(x_next, 0.0f, z_dist + (z_dist * i));

		float x_prev = x_next;
		x_next = -30 + (std::rand() % (60 + 1));
		std::cout << x_next << "\n"; // Show the random values for x_next in console.
	}

	// Create the rings:
	std::vector<Node> torus_points(torus_point_locations.size());
	for (std::size_t i = 0; i < torus_points.size(); i++) {
		torus_points[i].set_geometry(torus_shape);
		torus_points[i].set_program(&phong_shader, phong_set_uniforms_yellow);
		torus_points[i].get_transform().SetTranslate(torus_point_locations[i]);
	}

	torus_points[0].set_program(&phong_shader, phong_set_uniforms_red);


	glClearDepthf(1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Enable face culling to improve performance:
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//glCullFace(GL_BACK);

	bool show_logs = false; // Set to true for testing!
	bool show_gui = true;
	bool shader_reload_failed = false;

	std::int32_t torus_program_index = 0;
	auto polygon_mode = bonobo::polygon_mode_t::fill;

	auto lastTime = std::chrono::high_resolution_clock::now();
	int rend_size = 1;
	int next_node = 0;

	while (!glfwWindowShouldClose(window)) {
		auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		lastTime = nowTime;
		ellapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();

		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(deltaTimeUs, inputHandler);
		camera_position = mCamera.mWorld.GetTranslation();
		player.update(inputHandler, ellapsed_time_s / 1000.0f);
		glm::vec3 direction = player.get_direction();
		mCamera.mWorld.SetTranslate(player.get_position() - 6.0f * player.get_direction());
		mCamera.mWorld.LookAt(player.get_position(), glm::vec3(0.0, 1.0, 0.0));

		// Rotate the rings to face the player:
		for (int i = 0; i < torus_points.size(); i++) {
			torus_points[i].get_transform().LookAt(player.get_position(), glm::vec3(0.0, 1.0, 0.0));
		}

		// Change color of the ring as the player passes through:
		glm::vec3 distance_vec = torus_points[next_node].get_transform().GetTranslation() - player.get_position();
		float distance = sqrt(dot(distance_vec, distance_vec));

		if (distance < 10.0 - (2 * 0.5)) {

			torus_points[next_node].set_program(&phong_shader, phong_set_uniforms_green);

			next_node++;

			if (next_node > torus_points.size() - 1) {

				// Display MessageBox when the player has passed through all of the rings: 
				MessageBox(NULL, " You reached the last ring! \n Well played!", "Congratulations!",
					MB_OK);

				return; // Exit the game.
			}

			torus_points[next_node].set_program(&phong_shader, phong_set_uniforms_red);
			rend_size++;
		}


		if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_PRESSED) {
			shader_reload_failed = !program_manager.ReloadAllPrograms();
			if (shader_reload_failed)
				tinyfd_notifyPopup("Shader Program Reload Error",
					"An error occurred while reloading shader programs; see the logs for details.\n"
					"Rendering is suspended until the issue is solved. Once fixed, just reload the shaders again.",
					"error");
		}
		if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
			mWindowManager.ToggleFullscreenStatusForWindow(window);


		// Retrieve the actual framebuffer size: for HiDPI monitors,
		// you might end up with a framebuffer larger than what you
		// actually asked for. For example, if you ask for a 1920x1080
		// framebuffer, you might get a 3840x2160 one instead.
		// Also it might change as the user drags the window between
		// monitors with different DPIs, or if the fullscreen status is
		// being toggled.
		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);


		//
		// Todo: If you need to handle inputs, you can do it here
		//


		mWindowManager.NewImGuiFrame();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		bonobo::changePolygonMode(polygon_mode);


		if (!shader_reload_failed) {
			//
			// Todo: Render all your geometry here.
			//
			player.render(mCamera.GetWorldToClipMatrix());
			skybox.render(mCamera.GetWorldToClipMatrix());
			for (int i = 0; i < rend_size; i++) {
				torus_points[i].render(mCamera.GetWorldToClipMatrix());
			}
			//water.render(mCamera.GetWorldToClipMatrix());
		}


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//
		// Todo: If you want a custom ImGUI window, you can set it up
		//       here
		//

		// Setup for the ImGUI:
		bool opened = ImGui::Begin("Time", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		if (opened) {
			// Display time:
			ImGui::Text("%.3f s", ellapsed_time_s);

			// Change Polygon mode (for testing!):
			//bonobo::uiSelectPolygonMode("Polygon mode", polygon_mode);
			ImGui::End();
		}

		ImGui::Begin("How to play:", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text(
			"Fly through the red rings. \n \nControls: \n  SPACE: Move forward. \n  Q: Break. \n  A/D: Fly to the left/right. \n  W/S: Fly up/down."
		);
		ImGui::End();

		if (show_logs)
			Log::View::Render();
		if (show_gui)
			mWindowManager.RenderImGuiFrame();

		glfwSwapBuffers(window);
	}
}

int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		edaf80::Assignment5 assignment5(framework.GetWindowManager());
		assignment5.run();
	}
	catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
