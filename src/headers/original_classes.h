//
// Created by Wiktor on 02.10.2025.
//

#include <cstdint>

#ifndef ORIGINAL_CLASSES_H
#define ORIGINAL_CLASSES_H


namespace original_classes {
	struct header{
		char file_descriptor[4];
		uint32_t lump_count;
		uint32_t lump_offset;
	};

	struct lump{
		uint32_t lump_offset;
		uint32_t lump_size;
		char lump_name[8];
	};

	struct thing{
		int16_t x;
		int16_t y;
		uint16_t angle;
		uint16_t type;
		uint16_t flags;
	};

	struct linedef {
		int16_t v1;
		int16_t v2;
		uint16_t flags;
		uint16_t special_type;
		uint16_t sector_tag;
		uint16_t sidedef[2];
	};

	struct sidedef {
		int16_t x_offset;
		int16_t y_offset;
		char upper_texture[8];
		char lower_texture[8];
		char middle_texture[8];
		uint16_t sector_tag;
	};

	struct vertex {
		int16_t x;
		int16_t y;
	};

	struct seg {
		uint16_t v1;
		uint16_t v2;
		uint16_t angle;
		uint16_t linedef_num;
		uint16_t direction;
		uint16_t offset;
	};

	struct ssector {
		uint16_t seg_count;
		uint16_t first_seg;
	};

	struct node {
		uint16_t partition[2];
		uint16_t partition_end[2];
		uint16_t bounding_box[2];
		uint16_t children[2];
	};

	struct sector {
		int16_t floor_height;
		int16_t ceiling_height;
		char floor_texture[8];
		char ceiling_texture[8];
		uint16_t light_level;
		uint16_t type;
		uint16_t tag_no;
	};
};



#endif //ORIGINAL_CLASSES_H
