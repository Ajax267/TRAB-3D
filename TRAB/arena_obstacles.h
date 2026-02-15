#ifndef ARENA_OBSTACLES_H
#define ARENA_OBSTACLES_H

#include "utils.h"
#include "draw_utils.h"

#define CIRCULAR_OBSTACLE_HEIGHT 10 //Same as Player
#define CIRCULAR_OBSTACLE_RES 32 

class CircularObstacle : public CircleDefinition
{
    private:
        float height = CIRCULAR_OBSTACLE_HEIGHT;
    public:
        CircularObstacle(double cx, double cy, double radius, const std::string& color) :
        CircleDefinition(cx, cy, radius, color) {};
        
        void DrawObstacle();

        const float& GetHeight() const {return this->height;};
        void SetHeight(float height) {this->height=height;};
};

#endif