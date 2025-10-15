//
// Created by Wiktor on 11.10.2025.
//
#include "headers/TexBinder.h"

#define STB_IMAGE_IMPLEMENTATION
#include "headers/stb_image.h"
#include <algorithm>
#include <cctype>
#include <GL/glew.h>
#include <Gl/gl.h>

void TexBinder::LoadTexture(std::string path) {
	// Construct absolute path
	std::string abs_path = base_path + "\\" + path + ".png";

	printf("Loading texture %s\n", abs_path.c_str());
	int width, height, channels;
	auto texture = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);

	if (texture) {
		assign:
		GLuint texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		auto format = channels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture);
		textures[path] = texture_id;
		stbi_image_free(texture);
	} else {
		std::string path_2 = "./textures/" + path + ".png";
		texture = stbi_load(path_2.c_str(), &width, &height, &channels, 0);
		if (!texture)
			printf("Load failed %s\n", path.c_str());
		else {
			goto assign;
		}
	}
}

GLuint TexBinder::GetTexture(const char* texture_name) {
	size_t actual_len = strnlen(texture_name, 8);
	std::string texname_cmp = std::string(texture_name, actual_len);

	if (textures.find(texname_cmp) != textures.end()) {
		return textures[texname_cmp];
	}
	LoadTexture(texname_cmp);
	return textures[texname_cmp];
	return 0;
}


TexBinder::~TexBinder() {
	for (auto& texture : textures) {
		glDeleteTextures(1, &texture.second);
	}
}