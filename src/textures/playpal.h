#include <cstdio>

#include "bimg/3rdparty/etc2/Types.hpp"

#ifndef DOOM_PLAYPAL_H
#define DOOM_PLAYPAL_H

///Aach color is rgb. Pallete has a const size of (3 B * 256);
class playpal {
	static uint8_t* palette;

	playpal(uint8_t* ptr);

	~playpal();
};


#endif //DOOM_PLAYPAL_H