#include "arena_obstacles.h"

/*void CircularObstacle::DrawObstacle()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            0
        );
        DrawCirc(
            this->GetRadius(),
            this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
        );
    glPopMatrix();
}*/

#include <GL/glu.h>

void CircularObstacle::DrawObstacle()
{
    GLUquadricObj *quadric = gluNewQuadric();
    
    glPushMatrix();
        glTranslatef(this->GetPosition().GetX(), this->GetPosition().GetY(), 0);
        
        glColor3f(this->GetRGB().GetR(), this->GetRGB().GetG(), this->GetRGB().GetB());
        
        gluCylinder(quadric, this->GetRadius(), this->GetRadius(), 50.0, 20, 5);
        
        glTranslatef(0, 0, 50.0);
        gluDisk(quadric, 0, this->GetRadius(), 20, 1);
        
    glPopMatrix();
    
    gluDeleteQuadric(quadric);
}