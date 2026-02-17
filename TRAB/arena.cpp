#include "arena.h"
#include "utils.h"
extern GLuint g_texturaParedeArena;
extern bool toggle_texture;

extern GLuint g_texturaChao;




void CircularArena::DrawArena()
{
    glPushMatrix();
    glTranslatef(this->GetPosition().GetX(), -this->GetPosition().GetY(), 0);

    if (toggle_texture)
    {
        DrawTexturedCylinder(
            this->GetRadius(),
            this->GetHeightScale() * player_height,
            g_texturaParedeArena);
    }
    else
    {
        DrawOpenCilinder(
            this->GetRadius(),
            this->GetHeightScale() * player_height,
            INTERIOR_R, INTERIOR_G, INTERIOR_B,
            CIRCULAR_ARENA_RES);
    }


    if (toggle_texture)
    {
        DrawTexturedFloor(this->GetRadius(), 0.0f, g_texturaChao);
    }
    else
    {
       glPushMatrix();
            glTranslatef(0, 0, -this->GetHeightScale() * player_height); 
            
            DrawClosedCilinder(
                this->GetRadius(),
                this->GetHeightScale() * player_height,
                this->GetRGB().GetR(), this->GetRGB().GetG(), this->GetRGB().GetB(),
                CIRCULAR_ARENA_RES);
        glPopMatrix();
    }

    glPopMatrix();
}
