//
// Created by Wiktor on 17.11.2025.
//

#ifndef DOOM_ORIGINALTEXTURERENDERER_H
#define DOOM_ORIGINALTEXTURERENDERER_H
#include <unordered_map>
#include <vector>

#include "playpal.h"
#include "RawLumpKeeper.h"
#include "texture.h"
#include "vec2.h"

enum class OriginalTextureType {
	Patch,
	Sprite,
	Texture,
	Flat
};

struct pnames_t {
	char name[8];
};

struct mappatch_t {
	uint16_t origin_x, origin_y;
	uint16_t patch;
	///ignored
	uint16_t step_dir;
	///ignored
	uint16_t colormap;
};

struct maptexture_t {
	char name[8];
	uint32_t masked;
	uint16_t width, height;
	///Unused param, compat required
	uint32_t column_directory;
	uint16_t patch_count;
	mappatch_t* patches;
};

class OriginalTextureRenderer {

	RawLumpKeeper* raw_lump_keeper = nullptr;
	std::vector<maptexture_t> maptextures;
	std::vector<pnames_t> patchNames;
	playpal_t* playpal = nullptr;

	///Texture cache used for quick access;
	std::unordered_map<std::string, raw_texture> textures;

	void RenderFlat(const char* texName);
	void RenderSprite(const char* spriteName);
	void RenderPatch(const char* patchName);
	public:

	void BindPlaypal(playpal_t* playpal);

	void BindRawLumpKeeper(RawLumpKeeper* raw_lump_keeper);

	usvec2 getTextureSize(const char *texName, OriginalTextureType t) const;

	svec2 getOriginalPatchOffset(const char *texName) const;

	void UnloadCache();

	~OriginalTextureRenderer();

	const maptexture_t *FindMapTexture(const char *texName) const;

	void LoadTextureData();

	raw_texture RenderTexture(const char *texName, OriginalTextureType t);
};


#endif //DOOM_ORIGINALTEXTURERENDERER_H