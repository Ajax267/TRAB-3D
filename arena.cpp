#include "arena.h"

void CircularArena::DrawArena()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            0
        );
        DrawCirc(
            this->GetRadius(),
            this->GetRGB().GetR(),
            this->GetRGB().GetG(),
            this->GetRGB().GetB()
        );
    glPopMatrix();
}