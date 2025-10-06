#include "../headers/parser.h"
#include <stdexcept>
#include <cstring>
#include <regex.h>

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
void Parser::load_maps(FILE *file, const game_data& gd, uint32_t& i) {
	//Reserved memory
	auto* m = new original_map;
	{
		sscanf(gd.lumps[i].lump_name, "E%huM%hu", &m->episode, &m->level);
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
		const original_classes::lump *current = &gd.lumps[i + j];
		unsigned long count = 0;
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
void Parser::load_pallet(FILE *file, const game_data& gd, const uint32_t i) {
	const int count = gd.lumps[i].lump_size/(256*3);

	auto* p = load_lump<original_classes::palette>(file, gd.lumps[i]);
	for (int j=0;j<count;j++) {
		content->color_palette.push_back(&p[j]);
	}
}
void Parser::load_color_map(FILE *file, const game_data &gd, const uint32_t i) {
	const int count = gd.lumps[i].lump_size/(256);

	auto* map = load_lump<original_classes::colormap>(file, gd.lumps[i]);
	for (int j=0;j<count;j++) {
		content->color_map.push_back(&map[j]);
	}
}
void Parser::load_pname(FILE *file, const game_data &gd, const uint32_t i) {
	uint32_t count = 0;

	fseek(file, gd.lumps[i].lump_offset, SEEK_SET);
	fread(&count, sizeof(count), 1, file);

	auto* data = new char[count*8];
	fseek(file, gd.lumps[i].lump_offset + 4, SEEK_SET);
	int test = fread(data, 8, count, file);


	for (uint32_t j=0;j<count;j++) {
		char dest[9];
		strncpy(dest, &data[j*8], 8);

		content->p_names.name_patch.emplace_back(dest);
	}

	delete[] data;
}

void Parser::load_patch(FILE *file, const game_data &gd, uint32_t i) {
}

void Parser::convert_to_rgba(std::vector<std::vector<uint16_t>> &in,
                             std::vector<std::vector<std::vector<uint8_t>>> &out, int pallet_idx) {

	const unsigned int width = in.size();
	const unsigned int height = in[0].size();

	//Checking dimensions
	if (out.size() != width || out[0].size() != height) {
		throw std::runtime_error("Error convert to RGBA: not check width or height in both image");
	}

	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			if (in[i][j] == 0xFF) {
				out[i][j][0] = 255;
				out[i][j][1] = 255;
				out[i][j][2] = 255;
				out[i][j][3] = 0;
			} else {
				out[i][j][0] = content->color_palette[pallet_idx]->color[in[i][j]].r;
				out[i][j][1] = content->color_palette[pallet_idx]->color[in[i][j]].g;
				out[i][j][2] = content->color_palette[pallet_idx]->color[in[i][j]].b;
				out[i][j][3] = 255;
			}
		}
	}
}

void Parser::convert_to_rgb(unsigned char *in, int width, int height, unsigned char* out, int pallet_idx) {
	for (int i = 0; i < width*height; ++i) {
		out[i*3] = content->color_palette[pallet_idx]->color[in[i]].r;
		out[i*3+1] = content->color_palette[pallet_idx]->color[in[i]].g;
		out[i*3+2] = content->color_palette[pallet_idx]->color[in[i]].b;
	}
}

///Loads the file and reads data within.
void Parser::load_file(char *file_name) {

	FILE *file = fopen(file_name, "rb");

	game_data gd= {{}, nullptr};

	fread(&gd.header, sizeof(original_classes::header), 1, file);

	//Checking if it is the main file
	if (strncmp("IWAD", gd.header.file_descriptor, 4) != 0)
		throw std::runtime_error("Not an IWAD file");

	//Reserving space on Directory and loading it
	gd.lumps = static_cast<original_classes::lump *>(malloc(gd.header.lump_count * sizeof(original_classes::lump)));
	fseek(file, gd.header.lump_offset, SEEK_SET);
	fread(gd.lumps, sizeof(original_classes::lump), gd.header.lump_count, file);

	//todo To delete
	for (uint32_t i = 0; i < gd.header.lump_count; i++) {
		printf("%s\n",gd.lumps[i].lump_name);
	}

	// Find maps (E#M# format)
	regex_t regex;
	const int ret = regcomp(&regex, "^E[0-9]M[0-9]", REG_EXTENDED);
	if (ret != 0) {
		// Handle regex compilation error
		///cleaning
		fflush(stdout);
		fclose(file);
		regfree(&regex);
		throw std::runtime_error("Error parsing file");
	}

	//We go through all the lumps
	std::vector<std::string> not_used_lumps;
	for (uint32_t i = 0; i < gd.header.lump_count; i++) {
		const char* lump_name = gd.lumps[i].lump_name;

		if (regexec(&regex, lump_name, 0, nullptr, 0) == 0) {
			load_maps(file, gd, i);
		}
		else if (compareLumpName(lump_name, "PLAYPAL")) {
			load_pallet(file, gd, i);
		}
		else if (compareLumpName(lump_name, "COLORMAP")) {
			load_color_map(file, gd, i);
		}else if (compareLumpName(lump_name, "PNAMES")) {
			load_pname(file, gd, i);
		}else {
			char dest[9];
			strncpy(dest, lump_name, 8);
			not_used_lumps.emplace_back(dest);
		}
	}

	///cleaning
	fflush(stdout);
	fclose(file);
	regfree(&regex);
	free(gd.lumps);
}

std::vector<std::string> Parser::get_levels() const {
	auto res = std::vector<std::string>();
	for (const auto &map : content->maps) {
		res.push_back(std::to_string(map->episode) + " " + std::to_string(map->level));
	}
	return res;
}

original_classes::palette * Parser::get_palette(const int id) const {
	return content->color_palette[id];
}

unsigned char *Parser::get_color_map(const int id) {
	auto out = new unsigned char[256*3];
	auto* in = reinterpret_cast<unsigned char *>(&content->color_map[id]->color_idx);

	convert_to_rgb(in, 16, 16, out, 0);
	return out;
}
