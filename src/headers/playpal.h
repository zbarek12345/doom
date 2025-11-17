#ifndef DOOM_PLAYPAL_H
#define DOOM_PLAYPAL_H

#include "colors.h"
#include <cstdint>
#define PLAYPALS_SIZE 14
#define PLAYPAL_SIZE 3*256

///Aach color is rgb. Pallete has a const size of (3 B * 256);
class playpal_t {
protected:
	uint8_t* palette;
public:
	void load_palette(uint8_t* palette);

	void destroy_palette();

	color3 get_color(uint8_t index);

	bool is_loaded();
};


#endif //DOOM_PLAYPAL_H