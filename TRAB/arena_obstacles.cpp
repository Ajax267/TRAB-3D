#include "arena_obstacles.h"

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
            this->GetHeight(),
            this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB(),
            CIRCULAR_OBSTACLE_RES
        );
        // DrawCirc(
        //     this->GetRadius(),
        //     this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
        // );
    glPopMatrix();
}