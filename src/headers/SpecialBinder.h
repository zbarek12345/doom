//
// Created by Wiktor on 23.11.2025.
//

#ifndef DOOM_SPECIALBINDER_H
#define DOOM_SPECIALBINDER_H
#include <cstdint>
#include <map>
#include <vector>

#include "new_models.h"

enum class ActivationType {
	Walkover,
	Trigger,
	Shot
};

class SpecialBinder {
	static std::map<uint16_t, std::set<NewModels::Sector*>> tags;
public:
	static void CreateSpecial(NewModels::Wall *activator, ActivationType t);

	static void BindSpecial(uint16_t tag, NewModels::Sector* sector);

	static void Clear();
};


#endif //DOOM_SPECIALBINDER_H