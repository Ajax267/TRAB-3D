#include "arena_obstacles.h"
extern GLuint g_texturaObstaculo;
extern bool toggle_texture;


void CircularObstacle::DrawObstacle()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            0
        );

        if (toggle_texture)
        {
            DrawTexturedCylinderOutside(
                this->GetRadius(),
                this->GetHeight(),
                g_texturaObstaculo
            );

            DrawTexturedFloor(
                this->GetRadius(), 
                this->GetHeight(), 
                g_texturaObstaculo
            );
        }
        else
        {
            DrawClosedCilinder(
                this->GetRadius(),
                this->GetHeight(),
                this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB(),
                CIRCULAR_OBSTACLE_RES
            );
        }

    glPopMatrix();
}