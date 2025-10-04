#ifndef ORIGINAL_CLASSES_H
#define ORIGINAL_CLASSES_H

#include <cstdint>

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
		uint16_t x;
		uint16_t y;
		uint16_t angle;
		uint16_t type;
		uint16_t flags;
	};

	struct linedef {
		uint16_t v1;
		uint16_t v2;
		uint16_t flags;
		uint16_t special_type;
		uint16_t sector_tag;
		uint16_t sidedef[2];
	};

	struct sidedef {
		uint16_t x_offset;
		uint16_t y_offset;
		char upper_texture[8];
		char middle_texture[8];
		char lower_texture[8];
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
		uint16_t floor_height;
		uint16_t ceiling_height;
		char floor_texture[8];
		char ceiling_texture[8];
		uint16_t light_level;
		uint16_t type;
		uint16_t tag_no;
	};
};



#endif //ORIGINAL_CLASSES_H
