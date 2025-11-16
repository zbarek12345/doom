//
// Created by Wiktor on 30.05.2025.
//

#include "headers/parser.h"
#include "headers/CircuitFinder.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <CDT.h>
#include <climits>

#include "headers/vec2.h"


///Helper function to compare the lump names
bool compareLumpName(const char *lump1, const char *lump2) {
	return strncmp(lump1, lump2, 8) == 0;
}

template<class T>
T *Parser::load_lump(FILE *file, original_classes::lump pointer) {
	T *data = new T[pointer.lump_size / sizeof(T)];

	fseek(file, pointer.lump_offset, SEEK_SET);
	fread(data, sizeof(T), pointer.lump_size / sizeof(T), file);
	return data;
}

///Loads the file and reads data within.
void Parser::load_file(char *file_name) {
	FILE *file = fopen(file_name, "rb");

	game_data gd = {{}, nullptr};

	fread(&gd.header, sizeof(original_classes::header), 1, file);


	gd.lumps = (original_classes::lump *) malloc(gd.header.lump_count * sizeof(original_classes::lump));

	fseek(file, gd.header.lump_offset, SEEK_SET);

	fread(gd.lumps, sizeof(original_classes::lump), gd.header.lump_count, file);

	regex_t regex;

	int ret;
	const char *pat = "^E[0-9]M[0-9]|^MAP[0-9][0-9]";

	// Find maps (E#M# format)
	ret = regcomp(&regex, pat, REG_EXTENDED);
	if (ret != 0) {
		// Handle regex compilation error
		goto cleanup;
	}

	content = new game_content;
	for (int i = 0; i < gd.header.lump_count; i++) {
		if (regexec(&regex, gd.lumps[i].lump_name, 0, NULL, 0) == 0) {
			auto *m = new original_map;
			m->level = m->episode = 0; {
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
					m->things.assign(things, things + count);
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

NewModels::Map *Parser::generateMap(int id) {
	auto *mp = content->maps[id];
	printf("Linedef no. %llu\n Vertex no. %llu\n", mp->linedefs.size(), mp->vertexes.size());



	auto map = new NewModels::Map(); {

		///Find initial position of the player
		for (auto &thing: mp->things) {
			if (thing.type == 1) {
				map->player_start = {thing.x, thing.y, 0};
				map->player_start_angle = thing.angle - 90;
				break;
			}
		}

		struct svec2_less {
			constexpr bool operator()(const svec2& a, const svec2& b) const noexcept {
				return a.x != b.x ? a.x < b.x : a.y < b.y;
			}
		};

		std::vector<std::set<svec2, svec2_less>> Nodes(mp->sectors.size());
		std::vector<std::vector<std::pair<uint16_t, uint16_t> > > Lines(mp->sectors.size());
		std::vector<std::vector<uint16_t>> line_binding;

		uint16_t none = 0xFFFF;
		for (auto &line: mp->linedefs) {
			for (int i = 0; i < 2; i++) {
				if (line.sidedef[i] != none) {
					svec2 v1 = {mp->vertexes[line.v1].x, mp->vertexes[line.v1].y},
							v2 = {mp->vertexes[line.v2].x, mp->vertexes[line.v2].y};
					auto sector = mp->sidedefs[line.sidedef[i]].sector_tag;
					Nodes[sector].insert(v1);
					Nodes[sector].insert(v2);

					Lines[sector].emplace_back(std::make_pair(line.v1, line.v2));
				}
			}
		}

		//printf("Lines loaded;\n");

		map->sectors = std::vector<NewModels::Sector>(mp->sectors.size());
		for (uint16_t i = 0; i < mp->sectors.size(); i++) {
			printf("Calculating sector #%d;\n", i);
			auto sector = &map->sectors[i];
			sector->id = i;
			sector->floor_height = mp->sectors[i].floor_height;
			sector->ceil_height = mp->sectors[i].ceiling_height; {
				std::vector<svec2> temp;
				int16_t ceil = mp->sectors[i].ceiling_height, floor = mp->sectors[i].floor_height;

				int16_t min_x = SHRT_MAX, min_y = SHRT_MAX, max_x = SHRT_MIN, max_y = SHRT_MIN;

				for (auto node: Nodes[i]) {
					min_x = std::min(min_x, node.x);
					min_y = std::min(min_y, node.y);
					max_x = std::max(max_x, node.x);
					max_y = std::max(max_y, node.y);
					sector->nodes.push_back(node);
					temp.push_back(node);
				}

				sector->bounding_box[0] = {min_x,min_y};
				sector->bounding_box[1] = {max_x,max_y};

				auto find_index = [&temp](const svec2 &pos) -> int {
					for (size_t j = 0; j < temp.size(); ++j) {
						if (temp[j] == pos) {
							return static_cast<int>(j);
						}
					}
					// If not found, handle error (assuming always found for valid input).
					return -1;
				};

				//printf("Trying to find indexes for %d\n", i);
				int max_val = -1;
				std::vector<CDT::Edge> edges;
				std::vector<uint16_t> edges_map(temp.size());
				for (auto &line: Lines[i]) {

					svec2 v1 = {mp->vertexes[line.first].x, mp->vertexes[line.first].y},
							v2 = {mp->vertexes[line.second].x, mp->vertexes[line.second].y};

					auto p1 = find_index(v1);
					auto p2 = find_index(v2);

					if (p1 != -1 && p2 != -1) {
						edges.emplace_back(p1, p2);
					}

					edges_map[p1] = line.first;
					edges_map[p2] = line.second;
				}


				CDT::Triangulation<double> cdt = CDT::Triangulation<double>(CDT::VertexInsertionOrder::AsProvided, CDT::IntersectingConstraintEdges::TryResolve, 1.0);
				std::vector<CDT::V2d<double>> vertices;
				for (auto& vertex : temp ) {
					vertices.emplace_back(vertex.x, vertex.y);
				}

				cdt.insertVertices(vertices);
				cdt.insertEdges(edges);

				cdt.eraseOuterTrianglesAndHoles();

				auto triangles = cdt.triangles;

				for (auto &triangle: triangles) {
					sector->lines.push_back(
					NewModels::triangle{
						(uint16_t)triangle.vertices[2],
						(uint16_t)triangle.vertices[1],
						(uint16_t)triangle.vertices[0]
					});
				}

				sector->outer_edges = cdt.fixedEdges;
				sector->edges_map = edges_map;

				// if (sector->isInside(NewModels::vec2{map->player_start.x, map->player_start.y})) {
				// 	map->player_start.z = sector->floor_height;
				// 	printf("Player is inside sector #%d, height is %d\n", i, sector->floor_height);
				// }
			}
		}
		//printf("Sectors calculated;\n");
		auto tb = map->texture_binder;
		for (auto &line: mp->linedefs) {
			auto right_sector_tag = line.sidedef[0];
			auto left_sector_tag =  line.sidedef[1];

			NewModels::Sector* right_sector = nullptr, * left_sector = nullptr;
			if (right_sector_tag != none)
				right_sector = &map->sectors[mp->sidedefs[right_sector_tag].sector_tag];
			if (left_sector_tag != none)
				left_sector = &map->sectors[mp->sidedefs[left_sector_tag].sector_tag];

			svec2 v1 = {mp->vertexes[line.v1].x, mp->vertexes[line.v1].y},
					v2 = {mp->vertexes[line.v2].x, mp->vertexes[line.v2].y};
			auto wall = new NewModels::Wall(right_sector, left_sector);
			wall->setCoordinates(v1,v2);
			wall->setFlag(line.flags);
			///Right sector handling;
			if (right_sector != nullptr) {
				auto sd = &mp->sidedefs[line.sidedef[0]];
				if (sd->upper_texture[0] != '-') {
					auto tex = tb->GetTexture(sd->upper_texture);
					wall->assignUpperTexture(0,tex);
				}
				if (sd->lower_texture[0] != '-') {
					auto tex = tb->GetTexture(sd->lower_texture);
					wall->assignLowerTexture(0,tex);
				}
				if (sd->middle_texture[0] != '-') {
					auto tex = tb->GetTexture(sd->middle_texture);
					wall->assignMiddleTexture(0,tex);
				}
				if (line.special_type != 0) {
					wall->assignSpecial(line.special_type);
				}
				right_sector->bindWall(wall);
			}

			if (left_sector != nullptr) {
				left_sector->neighbors.emplace(right_sector);
				right_sector->neighbors.emplace(left_sector);
				auto sd = &mp->sidedefs[line.sidedef[1]];
				wall->setCoordinates(v2,v1);
				if (sd->upper_texture[0] != '-') {
					auto tex = tb->GetTexture(sd->upper_texture);
					wall->assignUpperTexture(1,tex);
				}
				if (sd->lower_texture[0] != '-') {
					auto tex = tb->GetTexture(sd->lower_texture);
					wall->assignLowerTexture(1,tex);
				}
				if (sd->middle_texture[0] != '-') {
					auto tex = tb->GetTexture(sd->middle_texture);
					wall->assignMiddleTexture(1,tex);
				}
				left_sector->bindWall(wall);
				left_sector->neighbors.emplace(right_sector);
				right_sector->neighbors.emplace(left_sector);
			}

			map->walls.push_back(wall);
		}

		printf("All Walls calculated;\n");
	}

	return map;
}

void Parser::testExport(int id, const char *filepath) {
	auto mp = content->maps[id];

	std::vector<std::pair<svec2, svec2> > vertices;
	std::vector<std::pair<svec2, svec2> > lefts;
	std::vector<std::pair<svec2, svec2> > rights;
	uint16_t none = 0xFFFF;

	for (auto &line: mp->linedefs) {

		svec2 v1 = {mp->vertexes[line.v1].x, mp->vertexes[line.v1].y},
				v2 = {mp->vertexes[line.v2].x, mp->vertexes[line.v2].y};

		vertices.push_back(std::make_pair(v1, v2)); // Calculate the vector from v1 to v2
		float dx = v2.x - v1.x;
		float dy = v2.y - v1.y;

		if (line.special_type != 0)
		printf("Line %u\n", line.special_type);
		// Calculate the perpendicular vector
		float perpDx = -dy;
		float perpDy = dx;

		// Normalize the perpendicular vector
		float magnitude = std::sqrt(perpDx * perpDx + perpDy * perpDy);
		float unitPerpDx = perpDx / magnitude;
		float unitPerpDy = perpDy / magnitude;

		float distance = 20.0f;
		// Calculate the parallel vectors by offsetting in the perpendicular direction
		float offsetX = unitPerpDx * distance;
		float offsetY = unitPerpDy * distance;

		// Parallel line points (as floats for calculations)
		fvec2 left1 = {v1.x - offsetX, v1.y - offsetY};
		fvec2 left2 = {v2.x - offsetX, v2.y - offsetY};
		fvec2 right1 = {v1.x + offsetX, v1.y + offsetY};
		fvec2 right2 = {v2.x + offsetX, v2.y + offsetY};

		// Determine on which side the parallel line lies
		// Use cross product to determine the relative position
		float crossProduct = (dx * (right1.y - v1.y) - dy * (right1.x - v1.x));
		if (crossProduct > 0) {
			// The right line is actually on the right side
			if (line.sidedef[0] != none)
			rights.push_back(std::make_pair(
				svec2{static_cast<int16_t>(right1.x), static_cast<int16_t>(right1.y)},
				svec2{static_cast<int16_t>(right2.x), static_cast<int16_t>(right2.y)}
			));
			if (line.sidedef[1] != none)
			lefts.push_back(std::make_pair(
				svec2{static_cast<int16_t>(left1.x), static_cast<int16_t>(left1.y)},
				svec2{static_cast<int16_t>(left2.x), static_cast<int16_t>(left2.y)}
			));
		} else {
			// The right line is on the left side (swap roles)
			if (line.sidedef[1] != none)
			rights.push_back(std::make_pair(
				svec2{static_cast<int16_t>(right1.x), static_cast<int16_t>(right1.y)},
				svec2{static_cast<int16_t>(right2.x), static_cast<int16_t>(right2.y)}
			));
			if (line.sidedef[0] != none)
			lefts.push_back(std::make_pair(
				svec2{static_cast<int16_t>(left1.x), static_cast<int16_t>(left1.y)},
				svec2{static_cast<int16_t>(left2.x), static_cast<int16_t>(left2.y)}
			));
		}
	}
	FILE *f = fopen(filepath, "w");
	if (!f) return;

	fprintf(f, "mtllib solids.mtl\n");

	int dot_count = 1;
	for (auto &pair: vertices) {
		fprintf(f, "v %d %d 0\n", pair.first.x, pair.first.y); // left dot
		fprintf(f, "v %d %d 0\n", pair.second.x, pair.second.y); // right dot
		fprintf(f, "l %d %d\n", dot_count, dot_count + 1); // line between the dots
		dot_count += 2;
	}

	fprintf(f, "\n\n g Lefts\n usemtl Red\n");
	for (auto &pair: lefts) {
		fprintf(f, "v %d %d 0\n", pair.first.x, pair.first.y); // left dot
		fprintf(f, "v %d %d 0\n", pair.second.x, pair.second.y); // right dot
		fprintf(f, "l %d %d\n", dot_count, dot_count + 1); // line between the dots
		dot_count += 2;
	}

	fprintf(f, "\n\ng Lefts\n usemtl Blue\n");
	for (auto &pair: rights) {
		fprintf(f, "v %d %d 0\n", pair.first.x, pair.first.y); // left dot
		fprintf(f, "v %d %d 0\n", pair.second.x, pair.second.y); // right dot
		fprintf(f, "l %d %d\n", dot_count, dot_count + 1); // line between the dots
		dot_count += 2;
	}

	fflush(f);
	fclose(f);
}

void Parser::find_sector(int id, svec3 vertex) {
	auto map = generateMap(id);
	// for (int i = 0; i < map->sectors.size(); i++) {
	// 	for (auto &floor_p: map->sectors[i].floor) {
	// 		if (vertex == floor_p)
	// 			printf("Found sector %4d\n", i);
	// 	}
	// }
}

void Parser::obj_export(int id, const char *filepath) {
	auto mp = content->maps[id];
	FILE *f = fopen(filepath, "w");
	if (!f) return;

	// Write material library reference
	fprintf(f, "mtllib solids.mtl\n");

	auto map = generateMap(id);

	int v_count = 1;
	std::vector<int> offsets[2] = {std::vector<int>(mp->sectors.size()), std::vector<int>(mp->sectors.size())};

	//Register vertexes
	// for (int i = 0; i < map->sectors.size(); i++) {
	// 	auto sector = &map->sectors[i];
	// 	offsets[0][i] = v_count;
	// 	v_count += sector->floor.size();
	// 	for (auto &e: sector->floor) {
	// 		fprintf(f, "v %d %d %d\n", e.x, e.y, e.z);
	// 	}
	// 	offsets[1][i] = v_count;
	// 	for (auto &e: sector->ceiling) {
	// 		fprintf(f, "v %d %d %d\n", e.x, e.y, e.z);
	// 	}
	// 	v_count += sector->ceiling.size();
	// }

	//draw floors
	fprintf(f, "\n\ng Ceilings\n usemtl Red \n");

	for (int i = 0; i < map->sectors.size(); i++) {
		auto offset = offsets[0][i];
		auto sector = &map->sectors[i];
		for (auto &line: sector->lines) {
			std::string out = "f ";
			out += std::to_string(offset + line.v1) + " ";
			out += std::to_string(offset + line.v2) + " ";
			out += std::to_string(offset + line.v3) + " ";
			out += std::to_string(offset + line.v1);
			fprintf(f, "%s\n", out.c_str());
		}
	}

	fprintf(f, "\n\ng Ceilings\n usemtl Green \n");
	//draw floors
	for (int i = 0; i < map->sectors.size(); i++) {
		auto offset = offsets[1][i];
		auto sector = &map->sectors[i];
		for (auto &line: sector->lines) {
			std::string out = "f ";
			out += std::to_string(offset + line.v1) + " ";
			out += std::to_string(offset + line.v2) + " ";
			out += std::to_string(offset + line.v3) + " ";
			out += std::to_string(offset + line.v1);
			fprintf(f, "%s\n", out.c_str());
		}
	}

	fprintf(f, "\n\n\n");
	//draw walls
	std::string low_buff = "g LowWalls \n usemtl Blue\n", high_buff = "g HighWall \n usemtl Purple\n", med_buff =
			"g MedWall \n usemtl Orange\n";
	auto offset = v_count;
	for (int i = 0; i < map->sectors.size(); i++) {
		// for (auto &wall: map->sectors[i].walls) {
		// 	fprintf(f, "v %d %d %d \n", (wall.left_top->x), (wall.left_top->y), (wall.left_top->z));
		// 	fprintf(f, "v %d %d %d \n", (wall.right_top->x), (wall.right_top->y), (wall.right_top->z));
		// 	fprintf(f, "v %d %d %d \n", (wall.right_bottom->x), (wall.right_bottom->y), (wall.right_bottom->z));
		// 	fprintf(f, "v %d %d %d \n", (wall.left_bottom->x), (wall.left_bottom->y), (wall.left_bottom->z));
		// 	auto buff = "f " + std::to_string(offset) + " " + std::to_string(offset + 1) + " "
		// 	            + std::to_string(offset + 2) + " " + std::to_string(offset + 3) + " " + std::to_string(offset) +
		// 	            "\n";
		// 	if (wall.type == 0)
		// 		low_buff += buff;
		// 	else if (wall.type == 1)
		// 		med_buff += buff;
		// 	else
		// 		high_buff += buff;
		//
		// 	offset += 4;
		// }
	}
	fprintf(f, "%s\n\n", low_buff.c_str());
	fprintf(f, "%s\n\n", med_buff.c_str());
	fprintf(f, "%s\n\n", high_buff.c_str());

	fflush(f);
	fclose(f);
}

std::vector<std::string> Parser::get_levels() const {
	auto res = std::vector<std::string>();
	for (const auto &map: content->maps) {
		res.push_back(std::to_string(map->episode) + " " + std::to_string(map->level));
	}
	return res;
}
