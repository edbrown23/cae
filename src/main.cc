#include <string>
#include <vector>
#include <variant>
#include <iostream>
#include <cstring>
#include <csignal>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "graphics/opengl/index.hh"
#include "graphics/opengl/drawing.hh"
#include "graphics/opengl/shaders.hh"
#include "graphics/opengl/primitives.hh"
#include "graphics/window.hh"
#include "unit.hh"
#include "config.hh"
#include "color.hh"
#include "fonts.hh"
#include "defer.hh"
#include "macros.hh"
#include "resources/shaders/opengl/basic.frag.hh"
#include "resources/shaders/opengl/basic.vert.hh"
#include "resources/shaders/opengl/basic_color.vert.hh"

using namespace config;
using namespace color;
using namespace fonts;
using namespace err;
using namespace defer;
using namespace window;
using namespace graphics::primitives;
using namespace graphics::opengl::drawing;
using namespace graphics::opengl::shaders;
using namespace graphics::opengl::primitives;

static const Config conf(
	{"Iosevka Term"},
	RGBColor(255, 255, 255),
	RGBColor(30, 30, 30),
	2,
	16
);

DEBUG_ONLY(
void GLAPIENTRY message_cb(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
) {
	(void)source;
	(void)userParam;
	(void)length;

	std::cerr << "GL Message: "
		<< (type == GL_DEBUG_TYPE_ERROR ? "!! ERROR !! " : " ")
		<< "severity: " << severity
		<< " error_id: " << id
		<< " message: " << message << std::endl;
}
)

Result<Unit> run() {
	using namespace std::literals;

	try(auto font_path, get_closest_font_match(conf.fonts));

	Window window(800, 600, "cae");

	DEBUG_ONLY(
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(message_cb, 0);
	)

	VertShader<XYZVertInputs> vert(basic_vert);
	VertShader<XYZRGBVertInputs> vert_color(basic_color_vert);
	FragShader<> frag(basic_frag);
	Program const basic_shdr(vert, frag);
	Program const basic_color_shdr(vert_color, frag);

	auto const pixel = make_pixel(basic_shdr);
	auto const mc_pixel = make_mc_pixel(basic_color_shdr);

	while (!glfwWindowShouldClose(window.handle)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		mc_pixel.draw();
		glfwSwapBuffers(window.handle);
	}

	return unit;
}

int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

	auto out = run();
	if (std::holds_alternative<Err>(out)) {
		std::cerr << "Error: " << std::get<Err>(out) << std::endl;
	}
	return 0;
}
