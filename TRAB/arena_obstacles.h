#ifndef ARENA_OBSTACLES_H
#define ARENA_OBSTACLES_H

#include "utils.h"
#include "draw_utils.h"

class CircularObstacle : public CircleDefinition
{
    public:
        CircularObstacle(double cx, double cy, double radius, const std::string& color) :
        CircleDefinition(cx, cy, radius, color) {};
        
        void DrawObstacle();
};

#endif