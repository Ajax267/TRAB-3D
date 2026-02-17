#ifndef ARENA_OBSTACLES_H
#define ARENA_OBSTACLES_H

#include "utils.h"
#include "draw_utils.h"

#define CIRCULAR_OBSTACLE_HEIGHT_SCALE 1.0 //Same as Player
#define CIRCULAR_OBSTACLE_RES 32 

extern GLuint g_texturaObstaculo;
extern bool toggle_texture;
extern double player_height;


class CircularObstacle : public CircleDefinition
{
    private:
        float height_scale = CIRCULAR_OBSTACLE_HEIGHT_SCALE;
    public:
        CircularObstacle(double cx, double cy, double radius, const std::string& color) :
        CircleDefinition(cx, cy, radius, color) {};
        
        void DrawObstacle();

        const float& GetHeightScale() const {return this->height_scale;};
        void SetHeightScale(float scale) {this->height_scale=scale;};
};

#endif