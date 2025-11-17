//
// Created by Wiktor on 17.11.2025.
//

#include "headers/OriginalTextureRenderer.h"
#include <string.h>

const maptexture_t* OriginalTextureRenderer::FindMapTexture(const char* texName) const {
	size_t actual_len = strnlen(texName, 8);
	std::string name(texName, actual_len);
	for (const auto& mt : maptextures) {
		if (strncmp(mt.name, name.c_str(), 8) == 0) {
			return &mt;
		}
	}
	return nullptr;
}

void OriginalTextureRenderer::LoadTextureData() {
	assert(raw_lump_keeper!=nullptr);
	// Load PNAMES
	const raw_lump *pnames_lump = raw_lump_keeper->GetRawLump("PNAMES");
	if (!pnames_lump || pnames_lump->size < 4) {
		// Error handling: log or throw
		return;
	}
	uint32_t num_pnames = *reinterpret_cast<uint32_t*>(pnames_lump->data);
	patchNames.resize(num_pnames);
	memcpy(patchNames.data(), pnames_lump->data + 4, num_pnames * sizeof(pnames_t));

	// Load TEXTURE1 (assuming no TEXTURE2 for simplicity; merge if needed)
	const raw_lump *tex1_lump = raw_lump_keeper->GetRawLump("TEXTURE1");
	if (!tex1_lump || tex1_lump->size < 4) {
		return;
	}

	uint32_t num_textures = *reinterpret_cast<uint32_t*>(tex1_lump->data);
	uint32_t* offsets = new uint32_t[num_textures];
	memcpy(offsets, tex1_lump->data + 4, num_textures * sizeof(uint32_t));

	for (uint32_t i = 0; i < num_textures; ++i) {
		maptextures.push_back({});

		uint8_t* tex_data = tex1_lump->data + offsets[i];
		maptexture_t& mt = maptextures[i];
		memset(mt.name, 0, sizeof(char)*8);
		memcpy(mt.name, tex_data, 8);
		mt.masked = *reinterpret_cast<uint32_t*>(tex_data + 8);
		mt.width = *reinterpret_cast<uint16_t*>(tex_data + 12);
		mt.height = *reinterpret_cast<uint16_t*>(tex_data + 14);
		mt.column_directory = *reinterpret_cast<uint32_t*>(tex_data + 16);  // Ignored
		mt.patch_count = *reinterpret_cast<uint16_t*>(tex_data + 20);
		mt.patches = new mappatch_t[mt.patch_count];
		memcpy(mt.patches, tex_data + 22, mt.patch_count * sizeof(mappatch_t));
	}

	delete [] offsets;
	uint16_t last_texture_index = num_textures - 1;
	tex1_lump = raw_lump_keeper->GetRawLump("TEXTURE2");
	if (!tex1_lump || tex1_lump->size < 4) {
		return;
	}

	num_textures = *reinterpret_cast<uint32_t*>(tex1_lump->data);
	offsets = new uint32_t[num_textures];
	memcpy(offsets, tex1_lump->data + 4, num_textures * sizeof(uint32_t));

	for (uint32_t i = 0; i < num_textures; ++i) {

		maptextures.push_back({});
		uint8_t* tex_data = tex1_lump->data + offsets[i];
		maptexture_t& mt = maptextures[i + last_texture_index];
		memcpy(mt.name, tex_data, 8);
		mt.masked = *reinterpret_cast<uint32_t*>(tex_data + 8);
		mt.width = *reinterpret_cast<uint16_t*>(tex_data + 12);
		mt.height = *reinterpret_cast<uint16_t*>(tex_data + 14);
		mt.column_directory = *reinterpret_cast<uint32_t*>(tex_data + 16);  // Ignored
		mt.patch_count = *reinterpret_cast<uint16_t*>(tex_data + 20);
		mt.patches = new mappatch_t[mt.patch_count];
		memcpy(mt.patches, tex_data + 22, mt.patch_count * sizeof(mappatch_t));
	}

	delete [] offsets;
}

void OriginalTextureRenderer::RenderFlat(const char* texName) {
    size_t actual_len = strnlen(texName, 8);
    std::string lumpName(texName, actual_len);

    const raw_lump *lump = raw_lump_keeper->GetRawLump(texName);
    if (!lump || lump->size != 4096) {
        // Invalid flat
        return;
    }

    const uint16_t width = 64;
    const uint16_t height = 64;
    uint8_t* data = new uint8_t[width * height * 4];  // RGBA
    for (uint32_t i = 0; i < 4096; ++i) {
        uint8_t idx = lump->data[i];
        uint8_t* pixel = data + i * 4;
    	auto pixel_color =playpal->get_color(idx);
    	memcpy(pixel, &pixel_color, 3);
        pixel[3] = 255;  // Opaque
    }

    textures[lumpName] = {width, height, data};
}

void OriginalTextureRenderer::RenderSprite(const char* spriteName) {
    // Sprites are treated similarly to patches in basic rendering
    RenderPatch(spriteName);  // Reuse patch logic, as sprites use patch format
}

void OriginalTextureRenderer::RenderPatch(const char* patchName) {
    const size_t actual_len = strnlen(patchName, 8);
    const std::string lumpName(patchName, actual_len);

    const raw_lump *lump = raw_lump_keeper->GetRawLump(patchName);
    if (!lump || lump->size < 8) {
        return;
    }

    int16_t width = *reinterpret_cast<int16_t*>(lump->data);
    int16_t height = *reinterpret_cast<int16_t*>(lump->data + 2);
    // int16_t left_offset = *reinterpret_cast<int16_t*>(lump->data + 4);  // Unused here
    // int16_t top_offset = *reinterpret_cast<int16_t*>(lump->data + 6);   // Unused here

    if (width <= 0 || height <= 0) {
        return;
    }

    uint32_t* column_offsets = reinterpret_cast<uint32_t*>(lump->data + 8);
    uint8_t* data = new uint8_t[width * height * 4]();  // Zero-init for transparent (alpha=0)

    for (int col = 0; col < width; ++col) {
        uint8_t* column = lump->data + column_offsets[col];
        uint8_t row_start;
        while ((row_start = *column++) != 0xFF) {
            uint8_t num_pixels = *column++;
            column++;  // Skip pad byte
            for (uint8_t p = 0; p < num_pixels; ++p) {
                uint8_t idx = *column++;
                int row = row_start + p;
                if (row >= height) continue;
                uint8_t* pixel = data + ((row * width + col) * 4);
            	auto pixel_color =playpal->get_color(idx);
            	memcpy(pixel, &pixel_color, 3);
                pixel[3] = 255;  // Opaque post
            }
            column++;  // Skip pad byte
        }
    }

    textures[lumpName] = {static_cast<uint16_t>(width), static_cast<uint16_t>(height), data};
}

raw_texture OriginalTextureRenderer::RenderTexture(const char *texName, OriginalTextureType t) {
	const size_t actual_len = strnlen(texName, 8);
	const auto lumpName= std::string(texName, actual_len);

	auto it = textures.find(lumpName);
    if (it != textures.end()) {
        return it->second;
    }

	assert(playpal->is_loaded());
    // Render based on type
    switch (t) {
	    case OriginalTextureType::Flat:
    		RenderFlat(texName);
    		break;
    	case OriginalTextureType::Sprite:
    		RenderSprite(texName);
    		break;
    	case OriginalTextureType::Patch:
    		RenderPatch(texName);
    		break;
    	case OriginalTextureType::Texture: {
    		const maptexture_t* mt = FindMapTexture(texName);
    		if (!mt) {
    			return {0, 0, nullptr};
    		}
    		uint8_t* data = new uint8_t[mt->width * mt->height * 4]();  // Zero-init
    		for (uint16_t p = 0; p < mt->patch_count; ++p) {
    			const mappatch_t& mp = mt->patches[p];
    			// Note: Assuming fixed struct: width/height should be 'patch' index; this is a bug in provided struct.
    			// Treating mp.width as 'patch' index for correctness (adjust if needed)
    			char p_name[9];
    			memcpy(p_name, patchNames[mp.patch].name, 8);  // Use mp.width as patch index
    			p_name[8] = '\0';
    			raw_texture patch_tex = RenderTexture(p_name, OriginalTextureType::Patch);
    			if (patch_tex.data == nullptr) continue;

    			for (uint16_t py = 0; py < patch_tex.h; ++py) {
    				for (uint16_t px = 0; px < patch_tex.w; ++px) {
    					int tx = px + mp.origin_x;
    					int ty = py + mp.origin_y;
    					if (tx < 0 || tx >= mt->width || ty < 0 || ty >= mt->height) continue;
    					uint8_t* src = const_cast<uint8_t*>(patch_tex.data) + (py * patch_tex.w + px) * 4;
    					if (src[3] == 0) continue;  // Transparent
    					uint8_t* dest = data + (ty * mt->width + tx) * 4;
    					memcpy(dest, src, 4);
    				}
    			}
    		}
    		textures[lumpName] = {mt->width, mt->height, data};
    		break;
    	}
    }

	it = textures.find(lumpName);
	return (it != textures.end()) ? it->second : raw_texture{0, 0, nullptr};
}

void OriginalTextureRenderer::BindPlaypal(playpal_t *playpal) {
	this->playpal = playpal;
}

void OriginalTextureRenderer::BindRawLumpKeeper(RawLumpKeeper* raw_lump_keeper) {
	this->raw_lump_keeper = raw_lump_keeper;
}

usvec2 OriginalTextureRenderer::getTextureSize(const char *texName, const OriginalTextureType t) const {
	switch (t) {
		case OriginalTextureType::Flat:
			return {64, 64};
		case OriginalTextureType::Patch:
		case OriginalTextureType::Sprite: {
			const raw_lump *lump = raw_lump_keeper->GetRawLump(texName);
			if (!lump || lump->size < 4) return {0, 0};
			unsigned short w = *reinterpret_cast<uint16_t *>(lump->data);
			unsigned short h = *reinterpret_cast<uint16_t *>(lump->data + 2);
			return {w, h};
		}
		case OriginalTextureType::Texture: {
			const maptexture_t* mt = FindMapTexture(texName);
			return mt ? usvec2{mt->width, mt->height} : usvec2{0, 0};
		}
	}
	return {0, 0};
}

void OriginalTextureRenderer::UnloadCache() {
	for (auto& texture : textures) {
		raw_texture_destroy(&texture.second);
	}
	textures.clear();
}

OriginalTextureRenderer::~OriginalTextureRenderer() {
	UnloadCache();
	delete playpal;
	for (auto& mt : maptextures) {
		delete[] mt.patches;
	}
}

