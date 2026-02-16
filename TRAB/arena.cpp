#include "arena.h" 

void CircularArena::DrawArena()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            0
        );
        DrawOpenCilinder(
            this->GetRadius(),
            this->GetHeight(),
            INTERIOR_R,INTERIOR_G,INTERIOR_B,
            CIRCULAR_ARENA_RES
        );
        glTranslatef(
            0,
            0,
            -this->GetHeight()
        );
        DrawClosedCilinder(
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