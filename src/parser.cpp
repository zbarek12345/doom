//
// Created by Wiktor on 30.05.2025.
//

#include "headers/parser.h"
#include "headers/CircuitFinder.h"
#include <algorithm>
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

NewModels::Map* Parser::generateMap(int id) {

	auto* mp = content->maps[id];
	printf("Linedef no. %llu\n Vertex no. %llu\n", mp->linedefs.size(), mp->vertexes.size());
	auto map = new NewModels::Map();
	{
		std::vector<std::set<NewModels::vec2> >Nodes(mp->sectors.size());
		std::vector<std::vector<std::pair<NewModels::vec2, NewModels::vec2> > > Lines(mp->sectors.size());
		uint16_t none = 0xFFFF;
		for (auto& line: mp->linedefs) {
			for (int i =0;i<2;i++) {
				if (line.sidedef[i]!=none) {
					NewModels::vec2 v1 = {mp->vertexes[line.v1].x, mp->vertexes[line.v1].y},
									v2 = {mp->vertexes[line.v2].x, mp->vertexes[line.v2].y};
					auto sector= mp->sidedefs[line.sidedef[i]].sector_tag;
					if (sector == 37)
						printf("Line from %d %d -> %d %d \n", v1.x, v1.y, v2.x, v2.y);
					Nodes[sector].insert(v1);
					Nodes[sector].insert(v2);
					Lines[sector].emplace_back(std::make_pair(v1,v2));
				}
			}
		}

		printf("Lines loaded;\n");

		map->sectors = std::vector<NewModels::Sector>(mp->sectors.size());
		for ( int i = 0; i < mp->sectors.size(); i++ ) {
			//printf("Calculating sector #%d;\n", i);
			auto sector = &map->sectors[i];

			sector->floor_height = mp->sectors[i].floor_height;
			sector->ceil_height = mp->sectors[i].ceiling_height;
			{
				std::vector<NewModels::vec2> temp;
				int16_t ceil = mp->sectors[i].ceiling_height, floor = mp->sectors[i].floor_height;
				for (auto node: Nodes[i]) {
					temp.emplace_back(node);
					sector->ceiling.push_back({node.x, node.y, ceil});
					sector->floor.push_back({node.x, node.y, floor});
				}

				auto find_index = [&temp](const NewModels::vec2& pos) -> int {
					for (size_t j = 0; j < temp.size(); ++j) {
						if (temp[j] == pos) {
							return static_cast<int>(j);
						}
					}
					// If not found, handle error (assuming always found for valid input).
					return -1;
				};

				int max_val = -1;
				std::vector<std::pair<int, int> > edges;
				for (auto& line : Lines[i]) {
					auto p1 = find_index(line.first);
					auto p2 = find_index(line.second);

					max_val = std::max(max_val, std::max(p1, p2));
					if (p1 != -1 && p2 != -1) {
						edges.emplace_back(p1, p2);
					}
				}

				CircuitFinder cf = CircuitFinder(max_val, edges);
				auto res = cf.run();
				for (auto& e : res) {
					std::vector<uint16_t> ln;
					for (auto& p : e) {
						ln.push_back(p);
					}
					sector->lines.push_back(ln);
				}

				//printf("Exited loop for : #%d\n", i);
			}

		}
		//printf("Sectors calculated;\n");

		for (auto& line : mp->linedefs) {
			for (int i =0;i<2;i++) {
				if (line.sidedef[i]!=none) {
					auto sd = mp->sidedefs[line.sidedef[i]];
					auto other = line.sidedef[(i+1)%2];
					NewModels::vec2 v1 = {mp->vertexes[line.v1].x, mp->vertexes[line.v1].y},
									v2 = {mp->vertexes[line.v2].x, mp->vertexes[line.v2].y};

					if (other == none) {
						if (sd.middle_texture[0] == '-')
							continue;
						auto sector = &map->sectors[sd.sector_tag];
						auto wall = NewModels::wall{
						sector->getPointer({v1.x,v1.y}, NewModels::Sector::Ceiling),
							sector->getPointer({v2.x,v2.y}, NewModels::Sector::Ceiling),
						sector->getPointer({v2.x,v2.y}, NewModels::Sector::Floor),
						sector->getPointer({v1.x,v1.y}, NewModels::Sector::Floor),
							1
						};
						sector->walls.push_back(wall);
					}
					else {
						auto sector = &map->sectors[sd.sector_tag];
						auto other_sector = &map->sectors[mp->sidedefs[other].sector_tag];

						if (sd.upper_texture[0] != '-' && sector->ceil_height> other_sector->ceil_height) {
							//printf("Upper texture %8s\n", sd.upper_texture);
							auto wall = NewModels::wall{
								sector->getPointer({v1.x,v1.y}, NewModels::Sector::Ceiling),
								sector->getPointer({v2.x,v2.y}, NewModels::Sector::Ceiling),
								other_sector->getPointer({v2.x,v2.y}, NewModels::Sector::Ceiling),
								other_sector->getPointer({v1.x,v1.y}, NewModels::Sector::Ceiling),
									0
								};
							sector->walls.push_back(wall);
						}

						if (sd.middle_texture[0] != '-') {
							//printf("Middle texture %8s\n", sd.middle_texture);
							auto sector = &map->sectors[sd.sector_tag];
							auto wall = NewModels::wall{
								sector->getPointer({v1.x,v1.y}, NewModels::Sector::Ceiling),
									sector->getPointer({v2.x,v2.y}, NewModels::Sector::Ceiling),
								sector->getPointer({v2.x,v2.y}, NewModels::Sector::Floor),
								sector->getPointer({v1.x,v1.y}, NewModels::Sector::Floor),
									1
								};
							sector->walls.push_back(wall);
						}

						if (sd.lower_texture[0] != '-') {
							//printf("Lower texture %8s\n", sd.lower_texture);
							auto wall = NewModels::wall{
								other_sector->getPointer({v1.x,v1.y}, NewModels::Sector::Floor),
								other_sector->getPointer({v2.x,v2.y}, NewModels::Sector::Floor),
								sector->getPointer({v2.x,v2.y}, NewModels::Sector::Floor),
								sector->getPointer({v1.x,v1.y}, NewModels::Sector::Floor),
									2
								};
							sector->walls.push_back(wall);
						}
					}
				}
			}


		}

		printf("All Walls calculated;\n");
	}


	for (auto& thing : mp->things) {
		if (thing.type == 1) {
			map->player_start = {thing.x,thing.y, 0};
			break;
		}
	}
	return map;
}

void Parser::find_sector(int id, NewModels::vec3 vertex) {
	auto map = generateMap(id);
	for (int i =0;i<map->sectors.size();i++) {
		for (auto& floor_p : map->sectors[i].floor) {
			if (vertex == floor_p)
				printf("Found sector %4d\n", i);
		}
	}
}

void Parser::obj_export(int id, const char *filepath) {
    auto mp = content->maps[id];
    FILE* f = fopen(filepath, "w");
    if (!f) return;

    // Write material library reference
    fprintf(f, "mtllib solids.mtl\n");

	auto map = generateMap(id);

	int v_count = 1;
	std::vector<int> offsets[2] = {std::vector<int>(mp->sectors.size()), std::vector<int>(mp->sectors.size())};

	//Register vertexes
	for (int i =0; i<map->sectors.size(); i++) {
		auto sector = &map->sectors[i];
		offsets[0][i] = v_count;
		v_count+= sector->floor.size();
		for (auto& e : sector->floor) {
			fprintf(f, "v %d %d %d\n", e.x, e.y, e.z);
		}
		offsets[1][i] = v_count;
		for (auto& e : sector->ceiling) {
			fprintf(f, "v %d %d %d\n", e.x, e.y, e.z);
		}
		v_count+= sector->ceiling.size();
	}

	//draw floors
	fprintf(f, "\n\ng Ceilings\n usemtl Red \n");

	for (int i =0; i<map->sectors.size(); i++) {
		auto offset = offsets[0][i];
		auto sector = &map->sectors[i];
		for (auto& line : sector->lines) {
			std::string out = "f ";
			for (auto& point: line) {
				out+= std::to_string(offset+point)+" ";
			}
			out += std::to_string(offset+line[0]);
			fprintf(f,"%s\n", out.c_str());
		}
	}

	fprintf(f, "\n\ng Ceilings\n usemtl Green \n");
	//draw floors
	for (int i =0; i<map->sectors.size(); i++) {
		auto offset = offsets[1][i];
		auto sector = &map->sectors[i];
		for (auto& line : sector->lines) {
			std::string out = "f ";
			for (auto& point: line) {
				out+= std::to_string(offset+point)+" ";
			}
			out += std::to_string(offset+line[0]);
			fprintf(f,"%s\n", out.c_str());
		}
	}

	fprintf(f,"\n\n\n");
	//draw walls
	std::string low_buff = "g LowWalls \n usemtl Blue\n", high_buff = "g HighWall \n usemtl Purple\n", med_buff = "g MedWall \n usemtl Orange\n";
	auto offset = v_count;
	for (int i =0; i<map->sectors.size(); i++) {
		for (auto& wall : map->sectors[i].walls) {
			fprintf(f, "v %d %d %d \n", (wall.left_top->x), (wall.left_top->y), (wall.left_top->z));
			fprintf(f, "v %d %d %d \n", (wall.right_top->x), (wall.right_top->y), (wall.right_top->z));
			fprintf(f, "v %d %d %d \n", (wall.right_bottom->x), (wall.right_bottom->y), (wall.right_bottom->z));
			fprintf(f, "v %d %d %d \n", (wall.left_bottom->x), (wall.left_bottom->y), (wall.left_bottom->z));
			auto buff = "f " + std::to_string(offset) + " " + std::to_string(offset+1) + " "
					+ std::to_string(offset+2) + " "+ std::to_string(offset+3)+ " "+  std::to_string(offset) + "\n";
			if (wall.type == 0)
				low_buff += buff;
			else if (wall.type == 1)
				med_buff += buff;
			else
				high_buff += buff;

			offset+=4;
		}
	}
	fprintf(f, "%s\n\n", low_buff.c_str());
	fprintf(f, "%s\n\n", med_buff.c_str());
	fprintf(f, "%s\n\n", high_buff.c_str());

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
