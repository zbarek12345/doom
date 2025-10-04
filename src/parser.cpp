//
// Created by Wiktor on 30.05.2025.
//

#include "headers/parser.h"

#include <stdexcept>
#include <cstring>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>


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

struct vec3 {
	int16_t x, y, z;
	bool operator==(const vec3& other) const {
		return  x == other.x &&
				y == other.y &&
				z == other.z;
	}
};

struct vec2 {
	int16_t x, y;
	bool operator==(const vec2& other) const {
		return  x == other.x &&
				y == other.y;
	}
};

namespace std {
	template <>
	struct hash<vec3> {
		size_t operator()(const vec3& v) const {
			size_t h1 = std::hash<int16_t>{}(v.x);
			size_t h2 = std::hash<int16_t>{}(v.y);
			size_t h3 = std::hash<int16_t>{}(v.z);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	template <>
	struct hash<vec2> {
		size_t operator()(const vec2& v) const {
			size_t h1 = std::hash<int16_t>{}(v.x);
			size_t h2 = std::hash<int16_t>{}(v.y);
			return h1 ^ (h2 << 1);
		}
	};
}

void Parser::obj_export(int id, const char *filepath) {
    auto mp = content->maps[id];
    FILE* f = fopen(filepath, "w");
    if (!f) return;

    // Write material library reference
    fprintf(f, "mtllib solids.mtl\n");

    // Store vertices per sector, preserving linedef order
    std::vector<std::vector<vec3>> sector_vertices(mp->sectors.size());
    std::unordered_map<vec3, int> vertex_to_index; // Maps vec3 to 1-based OBJ index
    uint16_t none = static_cast<uint16_t>(-1);

    // Collect vertices in linedef order for each sector
    for (const auto& line : mp->linedefs) {
        vec3 v1 = { (mp->vertexes[line.v1].x),
                    (mp->vertexes[line.v1].y), 0};
        vec3 v2 = { (mp->vertexes[line.v2].x),
                    (mp->vertexes[line.v2].y), 0};

        if (line.sidedef[0] != none) {
            int s = mp->sidedefs[line.sidedef[0]].sector_tag;
            sector_vertices[s].push_back(v1);
            sector_vertices[s].push_back(v2);
        }
        if (line.sidedef[1] != none) {
            int s = mp->sidedefs[line.sidedef[1]].sector_tag;
            sector_vertices[s].push_back(v1);
            sector_vertices[s].push_back(v2);
        }
    }

    // Buffers for OBJ output
    std::string v_floor_buff = "g Floors\nusemtl Red\n";
    std::string v_ceil_buff = "g Ceils\nusemtl Blue\n";
    std::string v_floor_line_buff, v_ceil_line_buff;
    int v_count = 1; // OBJ indices start at 1

    // Process each sector
    for (size_t i = 0; i < mp->sectors.size(); ++i) {
        if (sector_vertices[i].empty()) continue; // Skip empty sectors

        std::unordered_set<vec3> unique_vertices; // Ensure no duplicates in this sector
        std::string l_buf_floor = "l ";
        std::string l_buf_ceil = "l ";

        // Process vertices in order
        for (const auto& vertex : sector_vertices[i]) {
            if (unique_vertices.find(vertex) != unique_vertices.end()) continue; // Skip duplicates
            unique_vertices.insert(vertex);

            // Floor vertex
            vec3 floor_vertex = {vertex.x, vertex.y, (mp->sectors[i].floor_height)};
            if (vertex_to_index.find(floor_vertex) == vertex_to_index.end()) {
                vertex_to_index[floor_vertex] = v_count++;
                v_floor_buff += "v " + std::to_string(floor_vertex.x) + " " +
                                std::to_string(floor_vertex.y) + " " +
                                std::to_string(floor_vertex.z) + "\n";
            }
            l_buf_floor += std::to_string(vertex_to_index[floor_vertex]) + " ";

            // Ceiling vertex
            vec3 ceil_vertex = {vertex.x, vertex.y, (mp->sectors[i].ceiling_height)};
            if (vertex_to_index.find(ceil_vertex) == vertex_to_index.end()) {
                vertex_to_index[ceil_vertex] = v_count++;
                v_ceil_buff += "v " + std::to_string(ceil_vertex.x) + " " +
                               std::to_string(ceil_vertex.y) + " " +
                               std::to_string(ceil_vertex.z) + "\n";
            }
            l_buf_ceil += std::to_string(vertex_to_index[ceil_vertex]) + " ";
        }

        // Close the loops if there are vertices
        if (!unique_vertices.empty()) {
            auto first_vertex = *unique_vertices.begin();
            l_buf_floor += std::to_string(vertex_to_index[{first_vertex.x, first_vertex.y, mp->sectors[i].floor_height}]) + "\n";
            l_buf_ceil += std::to_string(vertex_to_index[{first_vertex.x, first_vertex.y, mp->sectors[i].ceiling_height}]) + "\n";
            v_floor_line_buff += l_buf_floor;
            v_ceil_line_buff += l_buf_ceil;
        }
    }

    // Write buffers to file
    fprintf(f, "%s", v_floor_buff.c_str());
    fprintf(f, "%s\n", v_floor_line_buff.c_str());
    fprintf(f, "%s", v_ceil_buff.c_str());
    fprintf(f, "%s\n", v_ceil_line_buff.c_str());

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
