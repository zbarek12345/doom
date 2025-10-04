//
// Created by Wiktor on 30.05.2025.
//

#include "headers/parser.h"

#include <stdexcept>
#include <cstring>
#include <map>
#include <set>
#include <unordered_map>


///Helper function to compare the lump names
bool compareLumpName(const char* lump1, const char* lump2) {
	return strncmp(lump1, lump2, 8) == 0;
}

template<class T>
T* Parser::load_lump(FILE* file, original_classes::lump pointer) {
	T* data = new T[pointer.lump_size / sizeof(T)];

	fseek(file, pointer.lump_offset, SEEK_SET);
	fread(data, sizeof(T), pointer.lump_size / sizeof(T), file);
	return data;
}
///Loads the file and reads data within.
void Parser::load_file(char *file_name) {

	FILE *file = fopen(file_name, "rb");

	game_data gd= {{}, nullptr};

	fread(&gd.header, sizeof(original_classes::header), 1, file);

	if (strncmp("IWAD", gd.header.file_descriptor, 4))
		throw std::runtime_error("Not an IWAD file");

	gd.lumps = (original_classes::lump*) malloc(gd.header.lump_count * sizeof(original_classes::lump));

	fseek(file, gd.header.lump_offset, SEEK_SET);

	fread(gd.lumps, sizeof(original_classes::lump), gd.header.lump_count, file);

	regex_t regex;

	int ret;
	const char* pat = "^E[0-9]M[0-9]|^MAP[0-9][0-9]";

	// Find maps (E#M# format)
	ret = regcomp(&regex, pat, REG_EXTENDED);
	if (ret != 0) {
		// Handle regex compilation error
		goto cleanup;
	}

	content = new game_content;
	for (int i = 0; i < gd.header.lump_count; i++) {
		if (regexec(&regex, gd.lumps[i].lump_name, 0, NULL, 0) == 0) {
			auto* m = new original_map;m->level = m->episode = 0;

			{
				sscanf(gd.lumps[i].lump_name, "E%hdM%hd", &m->episode, &m->level);
				if (m->episode == 0 && m->level == 0)
					sscanf(gd.lumps[i].lump_name, "MAP%hd", &m->level);
				m->things = std::vector<original_classes::thing>();
				m->linedefs = std::vector<original_classes::linedef>();
				m->sidedefs = std::vector<original_classes::sidedef>();
				m->vertexes = std::vector<original_classes::vertex>();
				m->segs = std::vector<original_classes::seg>();
				m->ssectors = std::vector<original_classes::ssector>();
				m->nodes = std::vector<original_classes::node>();
				m->sectors = std::vector<original_classes::sector>();
			}

			// Look for associated lumps (next 10 entries after the map marker)
			for (int j = 1; j <= 10 && (i + j) < gd.header.lump_count; j++) {
				original_classes::lump *current = &gd.lumps[i + j];
				int count = 0;
				if (compareLumpName(current->lump_name, "THINGS")) {
					auto *things = load_lump<original_classes::thing>(file, *current);
					count = current->lump_size / sizeof(original_classes::thing);
					m->things.assign( things, things + count);
					free(things);
				} else if (compareLumpName(current->lump_name, "LINEDEFS")) {
					auto *linedefs = load_lump<original_classes::linedef>(file, *current);
					count = current->lump_size / sizeof(original_classes::linedef);
					m->linedefs.assign(linedefs, linedefs + count);
					free(linedefs);
				} else if (compareLumpName(current->lump_name, "SIDEDEFS")) {
					auto *sidedefs = load_lump<original_classes::sidedef>(file, *current);
					count = current->lump_size / sizeof(original_classes::sidedef);
					m->sidedefs.assign(sidedefs, sidedefs + count);
					free(sidedefs);
				} else if (compareLumpName(current->lump_name, "VERTEXES")) {
					auto *vertexes = load_lump<original_classes::vertex>(file, *current);
					count = current->lump_size / sizeof(original_classes::vertex);
					m->vertexes.assign(vertexes, vertexes + count);
					free(vertexes);
				} else if (compareLumpName(current->lump_name, "SEGS")) {
					auto *segs = load_lump<original_classes::seg>(file, *current);
					count = current->lump_size / sizeof(original_classes::seg);
					m->segs.assign(segs, segs + count);
					free(segs);
				} else if (compareLumpName(current->lump_name, "SSECTORS")) {
					auto *ssectors = load_lump<original_classes::ssector>(file, *current);
					count = current->lump_size / sizeof(original_classes::ssector);
					m->ssectors.assign(ssectors, ssectors + count);
					free(ssectors);
				} else if (compareLumpName(current->lump_name, "NODES")) {
					auto *nodes = load_lump<original_classes::node>(file, *current);
					count = current->lump_size / sizeof(original_classes::node);
					m->nodes.assign(nodes, nodes + count);
					free(nodes);
				} else if (compareLumpName(current->lump_name, "SECTORS")) {
					auto *sectors = load_lump<original_classes::sector>(file, *current);
					count = current->lump_size / sizeof(original_classes::sector);
					m->sectors.assign(sectors, sectors + count);
					free(sectors);
				} else if (compareLumpName(current->lump_name, "REJECT")) {
					//m.REJECT = current;
				} else if (compareLumpName(current->lump_name, "BLOCKMAP")) {
					//m.BLOCKMAP = current;
				}
			}

			content->maps.push_back(m);
			i += 10; // Skip the next 10 lumps (they belong to this map)
		}
	}

	///cleaning
	regfree(&regex);
	fflush(stdout);
	fclose(file);
	return;
cleanup:
	regfree(&regex);
	fclose(file);
	throw std::runtime_error("Error parsing file");
}

void Parser::obj_export(int id, const char *filepath) {
	auto mp = content->maps[id];
	FILE* f = fopen(filepath, "w");
	if (!f) return;

	std::vector<std::multimap<int, int> > sector_adj(mp->sectors.size());

    uint16_t none = static_cast<uint16_t>(-1);

    for (const auto& line : mp->linedefs) {
        if (line.sidedef[0] != none) {
            int s = mp->sidedefs[line.sidedef[0]].sector_tag;
            sector_adj[s].insert({line.v1, line.v2});
        }
        if (line.sidedef[1] != none) {
            int s = mp->sidedefs[line.sidedef[1]].sector_tag;
            sector_adj[s].insert({line.v2, line.v1});
        }
    }

    int v_count = 1; // OBJ indices start at 1

    for (size_t i = 0; i < mp->sectors.size(); ++i) {
        auto& adj = sector_adj[i];
        while (!adj.empty()) {
            // Find one loop
            int start = adj.begin()->first;
            int current = start;
            std::vector<int> ordered;
            bool closed = false;
            do {
                ordered.push_back(current);
                auto it = adj.lower_bound(current);
                if (it == adj.end() || it->first != current) {
                    break;
                }
                int next = it->second;
                adj.erase(it);
                current = next;
                if (current == start && !ordered.empty()) {
                    closed = true;
                    break;
                }
            } while (true);

            if (!closed || ordered.size() < 3) {
                continue;
            }

            // Write vertices and build line strings
            std::string floor_str = "f ";
            std::string ceil_str = "f ";
            int first_floor = v_count;
            int first_ceil = v_count + 1;
            for (int vert_idx : ordered) {
                auto& vert = mp->vertexes[vert_idx];
                fprintf(f, "v %d %d %d\n", vert.x, vert.y, mp->sectors[i].floor_height);
                fprintf(f, "v %d %d %d\n", vert.x, vert.y, mp->sectors[i].ceiling_height);
                floor_str += std::to_string(v_count) + " ";
                ceil_str += std::to_string(v_count + 1) + " ";
                v_count += 2;
            }
            // Close the loops
            floor_str += std::to_string(first_floor);
            ceil_str += std::to_string(first_ceil);

            // Write the lines
            fprintf(f, "%s\n", floor_str.c_str());
            fprintf(f, "%s\n", ceil_str.c_str());
        }
    }

	fflush(f);
	fclose(f);
}

std::vector<std::string> Parser::get_levels() const {
	auto res = std::vector<std::string>();
	for (const auto &map : content->maps) {
		res.push_back(std::to_string(map->episode) + " " + std::to_string(map->level));
	}
	return res;
}
