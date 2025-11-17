//
// Created by Wiktor on 17.11.2025.
//

#include "headers/RawLumpKeeper.h"

#include <string.h>

RawLumpKeeper::RawLumpKeeper() {
	raw_lumps = std::unordered_map<std::string, raw_lump>();
}

RawLumpKeeper::~RawLumpKeeper() {
	for (const auto& lump: raw_lumps)
		free(lump.second.data);
}

const raw_lump* RawLumpKeeper::GetRawLump(const char *LumpName) {
	const size_t actual_len = strnlen(LumpName, 8);
	const auto lumpName = std::string(LumpName, actual_len);

	auto it = raw_lumps.find(lumpName);
	if (it != raw_lumps.end()) {
		return &it->second;
	}
	return nullptr;
}

void RawLumpKeeper::CreateRawLumps(const char *LumpName, uint8_t *data, const uint32_t size) {
	const size_t actual_len = strnlen(LumpName, 8);
	const auto lumpName= std::string(LumpName, actual_len);

	const auto it = raw_lumps.find(lumpName);
	if (it != raw_lumps.end()) {
		free(it->second.data);  // Free old data before overwrite
	}
	raw_lumps[lumpName] = {data, size};

	raw_lumps[lumpName] = raw_lump{data, size};
}



