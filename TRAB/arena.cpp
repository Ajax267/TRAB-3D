#include "arena.h" 

void CircularArena::DrawArena()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            -this->GetHeight()
        );
        DrawCilinder(
            this->GetRadius(),
            this->GetHeight(),
            this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB(),
            CIRCULAR_ARENA_RES
        );
        // DrawCirc(
        //     this->GetRadius(),
        //     this->GetRGB().GetR(),
        //     this->GetRGB().GetG(),
        //     this->GetRGB().GetB()
        // );
    glPopMatrix();
}