//
// Created by Wiktor on 11.10.2025.
//
#include "headers/TexBinder.h"

#define STB_IMAGE_IMPLEMENTATION
#include "headers/stb_image.h"
#include <algorithm>
#include <cctype>
#include <GL/glew.h>
#include <GL/gl.h>


void TexBinder::BindOriginalTextureRenderer(OriginalTextureRenderer *original_texture_renderer) {
	this->original_texture_renderer = original_texture_renderer;
}

void TexBinder::LoadTexture(std::string path, TextureType type) {
	// Construct absolute path

	std::string abs_path = "./textures";
	OriginalTextureType ttype;
	if (type == TextureType::FlatTexture) {
		abs_path = base_path + "\\" + path + ".png";
		ttype = OriginalTextureType::Flat;
	}
	else if (type == TextureType::WallTexture) {
		abs_path = base_path + "\\" + path + ".png";
		ttype = OriginalTextureType::Texture;
	}
	else if (type == TextureType::ObstacleTexture) {
		abs_path = "./DECORATIONS/" + path + ".png";
		ttype = OriginalTextureType::Sprite;
	}
	else if (type == TextureType::ItemTexture) {
		abs_path = "./ITEMS/" + path + ".png";
		ttype = OriginalTextureType::Sprite;
	}else if (type == TextureType::WeaponTexture) {
		abs_path = "./WEAPONS/" + path + ".png";
		ttype = OriginalTextureType::Sprite;
	}
	
	printf("Loading texture %s\n", abs_path.c_str());
	int width, height, channels;
	auto texture = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);

	if (texture) {
		try {

			GLuint texture_id;
			glGenTextures(1, &texture_id);
			glBindTexture(GL_TEXTURE_2D, texture_id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			auto format = channels == 4 ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture);
			auto size = original_texture_renderer->getTextureSize(path.c_str(), ttype);
			textures[path] = {size.x, size.y, texture_id};
			stbi_image_free(texture);
		}
		catch (std::exception& e) {
			printf("Failed to load texture %s, trying to load from WAD\n", abs_path.c_str());
			goto assign;
		}
	} else {
		assign:
		printf("Texture failed to load at %s, proceeding to load from WAD\n", abs_path.c_str());
		GLuint texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		auto rendered_texture = original_texture_renderer->RenderTexture(path.c_str(), ttype);
		auto format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, rendered_texture.w, rendered_texture.h, 0, format, GL_UNSIGNED_BYTE, rendered_texture.data);
		auto size = original_texture_renderer->getTextureSize(path.c_str(), ttype);
		textures[path] = {size.x, size.y, texture_id};
		raw_texture_destroy(&rendered_texture);
	}
}

gl_texture TexBinder::GetTexture(const char* texture_name, TextureType type) {
	size_t actual_len = strnlen(texture_name, 8);
	std::string texname_cmp = std::string(texture_name, actual_len);

	if (textures.find(texname_cmp) != textures.end()) {
		return textures[texname_cmp];
	}
	LoadTexture(texname_cmp, type);
	return textures[texname_cmp];
}

TexBinder::~TexBinder() {
	for (auto& texture : textures) {
		glDeleteTextures(1, &texture.second.texture_id);
	}
}