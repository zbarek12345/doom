//
// Created by Wiktor on 02.10.2025.
//
#include "original_classes.h"
#include <vector>
#ifndef MAP_H
#define MAP_H

struct original_map{
	std::string level_name;
	uint16_t level;
	uint16_t episode;

    std::vector<original_classes::thing> things;
    std::vector<original_classes::linedef> linedefs;
    std::vector<original_classes::sidedef> sidedefs;
    std::vector<original_classes::vertex> vertexes;
    std::vector<original_classes::seg> segs;
    std::vector<original_classes::ssector> ssectors;
    std::vector<original_classes::node> nodes;
    std::vector<original_classes::sector> sectors;
    // std::vector<original_classes::sector> reject;
    // std::vector<original_classes::sector> blockmap;
};
#endif //MAP_H
