#include "arena_obstacles.h"

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
                this->GetHeightScale() * player_height,
                g_texturaObstaculo
            );

            DrawTexturedFloor(
                this->GetRadius(), 
                this->GetHeightScale() * player_height, 
                g_texturaObstaculo
            );
        }
        else
        {
            DrawClosedCilinder(
                this->GetRadius(),
                this->GetHeightScale() * player_height,
                this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB(),
                CIRCULAR_OBSTACLE_RES
            );
        }

    glPopMatrix();
}