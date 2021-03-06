#pragma once

#include <vector>
#include <tuple>

#include "index.hh"
#include "shaders.hh"
#include "primitives.hh"

namespace graphics { namespace opengl { namespace drawing {

struct GLResource {
	typedef void (*create_func)(GLsizei, GLuint*);
	typedef void (*destroy_func)(GLsizei, const GLuint*);

	GLuint id;
	destroy_func destroy;

	GLResource() = delete;
	GLResource(create_func create, destroy_func destroy);
	GLResource(GLResource&& other);
	GLResource& operator=(GLResource&& other);
	~GLResource();

	GLResource(GLResource const& other) = delete;
	GLResource& operator=(GLResource const& other) = delete;
};

struct VAO {
	GLResource vao;

	VAO();

	void activate() const;
	void deactivate() const;

	template<typename Callable>
	void use(Callable c) const {
		this->activate();
		c();
		this->deactivate();
	}
};

struct VBO {
	GLResource vbo;

	void activate() const;
	void deactivate() const;

	template <typename Data>
	VBO(std::vector<Data> const& data) : vbo(glGenBuffers, glDeleteBuffers) {
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo.id);
		glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(Data) * data.size(),
			data.data(),
			GL_STATIC_DRAW
		);
	}
};

struct EBO {
	GLResource ebo;
	GLsizei n_indices;

	void activate() const;

	EBO(std::vector<GLuint> const& indices);
};

struct DrawInfo {
	VAO vao;
	GLsizei n_indices;

	void draw() const;
	void draw(shaders::Program const& shdr) const;

	template <typename Uniforms>
	void draw(shaders::Program const& shdr, Uniforms const& uniforms) const {
		this->vao.use([&]() {
			shdr.activate();
			uniforms.activate(shdr.program);
			glDrawElements(GL_TRIANGLES, this->n_indices, GL_UNSIGNED_INT, 0);
		});
	}

	template <typename Data>
	static std::tuple<VBO, EBO, DrawInfo> make(
		std::vector<Data> const& data,
		std::vector<GLuint> const& indices
	) {
		VAO vao;
		vao.activate();

		VBO vbo(data);
		EBO ebo(indices);

		primitives::attrib_setup<Data>();
		vao.deactivate();

		DrawInfo res { std::move(vao), ebo.n_indices };
		return std::make_tuple(std::move(vbo), std::move(ebo), std::move(res));
	}

	template <typename Data>
	static std::tuple<VBO, DrawInfo> make(
		std::vector<Data> const& data,
		EBO const& ebo
	) {
		VAO vao;
		vao.activate();

		VBO vbo(data);
		ebo.activate();

		primitives::attrib_setup<Data>();
		vao.deactivate();

		DrawInfo res { std::move(vao), ebo.n_indices };
		return std::make_tuple(std::move(vbo), std::move(res));
	}
};

} } }
