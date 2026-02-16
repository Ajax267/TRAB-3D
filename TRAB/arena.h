#ifndef CIRCULAR_ARENA_H
#define CIRCULAR_ARENA_H

#include <cmath>
#include <string>

// Por enquanto a Denifição de Círculo está aqui, depois trocar
#include "utils.h"
#include "draw_utils.h"

#define CIRCULAR_ARENA_HEIGHT 40
#define CIRCULAR_ARENA_RES 64

#define INTERIOR_R 1.0
#define INTERIOR_G 1.0
#define INTERIOR_B 0

class CircularArena : public CircleDefinition
{
    private:
        float height = CIRCULAR_ARENA_HEIGHT;
    public:
        // Allow declaration without parameters
        CircularArena()
            : CircleDefinition(0.0, 0.0, 0.0, "None") {} 

        CircularArena(double cx, double cy, double radius, const std::string& color)
            : CircleDefinition(cx,cy,radius,color) {};

        void DrawArena();

        const float& GetHeight() const {return this->height;};
        void SetHeight(float height) {this->height=height;};
    
};

#endif