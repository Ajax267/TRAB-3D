#ifndef _ARENA_PARSER_H
#define _ARENA_PARSER_H

#include <optional>

#include "arena.h"
#include "arena_obstacles.h"
#include "player.h"

std::optional<CircularArena> arena_getter(std::vector<CircleDefinition>& svg_circles);
std::optional<std::vector<ArenaPlayer>> players_getter(
    std::vector<CircleDefinition>& svg_circles,
    CircularArena& arena
);
std::optional<std::vector<CircularObstacle>> obstacles_getter(
    std::vector<CircleDefinition>& svg_circles,
    CircularArena& arena
);

#endif 