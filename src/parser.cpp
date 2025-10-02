//
// Created by Wiktor on 30.05.2025.
//

#include "headers/parser.h"

#include <stdexcept>


parser::game_data::game_data(uint32_t lump_count) {
	lumps = new lump[lump_count];
}

parser::game_data::~game_data() {
	delete[] lumps;
}

///Helper function to compare the lump names
bool compareLumpName(const char* lump1, const char* lump2) {
	return strncmp(lump1, lump2, 8) == 0;
}

///Loads the file and reads data within.
parser::game_data *parser::load_file(char *file_name) {
	game_data *data = nullptr;

	FILE *file = fopen(file_name, "rb");

	regex_t regex;
	header h;
	int ret;
	// Check for IWAD
	ret = regcomp(&regex, "^IWAD", REG_EXTENDED);
	if (ret != 0) {
		// Handle regex compilation error
		goto cleanup;
	}

	fread(&h, sizeof(header), 1, file);
	if (regexec(&regex, h.file_descriptor, 0, NULL, 0) != 0) {
		goto clear;
	}
	regfree(&regex);

	data = new game_data(h.lump_count);
	fseek(file, h.lump_offset, SEEK_SET);
	fread(data->lumps, sizeof(lump), h.lump_count, file);

	// Find maps (E#M# format)
	ret = regcomp(&regex, "^E[0-9]M[0-9]", REG_EXTENDED);
	if (ret != 0) {
		// Handle regex compilation error
		goto cleanup;
	}

	for (int i = 0; i < h.lump_count; i++) {
		if (regexec(&regex, data->lumps[i].lump_name, 0, NULL, 0) == 0) {
			int ep, lvl;
			sscanf(data->lumps[i].lump_name, "E%dM%d", &ep, &lvl);
			map m = {};
			m.episode = ep;
			m.level = lvl;

			// Look for associated lumps (next 10 entries after the map marker)
			for (int j = 1; j <= 10 && (i + j) < h.lump_count; j++) {
				lump *current = &data->lumps[i + j];

				if (compareLumpName(current->lump_name, "THINGS")) {
					m.THINGS = current;
				} else if (compareLumpName(current->lump_name, "LINEDEFS")) {
					m.LINEDEFS = current;
				} else if (compareLumpName(current->lump_name, "SIDEDEFS")) {
					m.SIDEDEFS = current;
				} else if (compareLumpName(current->lump_name, "VERTEXES")) {
					m.VERTEXES = current;
				} else if (compareLumpName(current->lump_name, "SEGS")) {
					m.SEGS = current;
				} else if (compareLumpName(current->lump_name, "SSECTORS")) {
					m.SSECTORS = current;
				} else if (compareLumpName(current->lump_name, "NODES")) {
					m.NODES = current;
				} else if (compareLumpName(current->lump_name, "SECTORS")) {
					m.SECTORS = current;
				} else if (compareLumpName(current->lump_name, "REJECT")) {
					m.REJECT = current;
				} else if (compareLumpName(current->lump_name, "BLOCKMAP")) {
					m.BLOCKMAP = current;
				}
			}

			data->maps.push_back(m);
			i += 10; // Skip the next 10 lumps (they belong to this map)
		}
	}

clear:
	regfree(&regex);
	fflush(stdout);
	fclose(file);
	return data;

cleanup:
	regfree(&regex);
	fclose(file);
	throw std::runtime_error("Error parsing file");
}
