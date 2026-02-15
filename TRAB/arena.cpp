#include "arena.h"

#define CIRCULAR_ARENA_HEIGHT 20
#define CIRCULAR_ARENA_RES 32 

void CircularArena::DrawArena()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            -CIRCULAR_ARENA_HEIGHT
        );
        DrawCilinder(
            this->GetRadius(),
            CIRCULAR_ARENA_HEIGHT,
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