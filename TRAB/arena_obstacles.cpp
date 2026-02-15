#include "arena_obstacles.h"

#define CIRCULAR_OBSTACLE_HEIGHT 20
#define CIRCULAR_OBSTACLE_RES 32 

void CircularObstacle::DrawObstacle()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            0
        );
        DrawCilinder(
            this->GetRadius(),
            CIRCULAR_OBSTACLE_HEIGHT,
            this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB(),
            CIRCULAR_OBSTACLE_RES
        );
        // DrawCirc(
        //     this->GetRadius(),
        //     this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
        // );
    glPopMatrix();
}