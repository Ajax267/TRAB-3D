#ifndef CIRCULAR_ARENA_H
#define CIRCULAR_ARENA_H

#include <cmath>
#include <string>

// Por enquanto a Denifição de Círculo está aqui, depois trocar
#include "utils.h"
#include "draw_utils.h"

class CircularArena : public CircleDefinition
{
public:

    // Allow declaration without parameters
    CircularArena()
        : CircleDefinition(0.0, 0.0, 0.0, "None") {} 

    CircularArena(double cx, double cy, double radius, const std::string& color)
        : CircleDefinition(cx,cy,radius,color) {};

    void DrawArena();
    
};

#endif