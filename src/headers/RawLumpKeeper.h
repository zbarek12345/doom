//
// Created by Wiktor on 17.11.2025.
//

#ifndef DOOM_RAWLUMPKEEPER_H
#define DOOM_RAWLUMPKEEPER_H
#include <string>
#include <unordered_map>

#include "original_classes.h"

class RawLumpKeeper {
	std::unordered_map<std::string, raw_lump> raw_lumps;

public:

	RawLumpKeeper();

	void CreateRawLumps(const char *LumpName, uint8_t *data, uint32_t size);

	const raw_lump* GetRawLump(const char* LumpName);

	~RawLumpKeeper();
};

#endif //DOOM_RAWLUMPKEEPER_H