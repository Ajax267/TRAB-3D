#include "draw_utils.h"

// -----------------RECTANGLE--------------------//
void DrawRect(
    GLint height, GLint width,
    GLfloat R, GLfloat G, GLfloat B
)
{
   /* Define cor dos vértices com os valores R, G e B variando de 0.0 a 1.0 */
   /* Desenhar um polígono vermelho (retângulo) */
   glBegin(GL_POLYGON);  
        glColor3f (R, G, B);
        glVertex3f (-width/2, 0.0, 0.0);
        glColor3f (R, G, B);
        glVertex3f (width/2, 0.0 , 0.0);
        glColor3f (R, G, B);
        glVertex3f (width/2, height, 0.0);
        glColor3f (R, G, B);
        glVertex3f (-width/2, height, 0.0);
   glEnd();

}

void DrawRectWithBorder(
    GLint height, GLint width,
    GLfloat R, GLfloat G, GLfloat B
)
{
    // Border
    DrawRect(
        height,width,
        0,0,0
    );
    // Actual Object
    DrawRect(
        height*(1-BORDER_SIZE*0.5),width*(1-BORDER_SIZE*0.5),
        R,G,B
    );
}

// -----------------CIRCLE--------------------//
void DrawCirc(
    GLint radius,
    GLfloat R, GLfloat G, GLfloat B,
    bool points,
    double circle_res
)
{
    if (points) glBegin(GL_POINTS);
    else glBegin(GL_POLYGON);
        for (double i = 0.0; i <= CIRCLE_DEGREES; i+= (CIRCLE_DEGREES/circle_res) )
        {
            if (points) glPointSize(POINT_SIZE);
            glColor3f (R, G, B);
            glVertex3f (radius * cos(i*RADIANS), radius * sin(i*RADIANS), 0.0);
        }
   glEnd();
}
void DrawCircWithBorder(
    GLint radius,
    GLfloat R, GLfloat G, GLfloat B,
    bool points,
    double circle_res
)
{
    // Border
    DrawCirc(
        radius,
        0,0,0,
        points,
        circle_res
    );
    // Actual Object
    DrawCirc(
        radius*(1.0-BORDER_SIZE),
        R,G,B,
        points,
        circle_res
    );
}

// -----------------ELLIPSE--------------------//
void DrawEllipse(
    GLfloat radius_x, GLfloat radius_y,
    GLfloat R, GLfloat G, GLfloat B,
    bool points,
    double elipse_res
)
{

    if (points) glBegin(GL_POINTS);
    else glBegin(GL_POLYGON);

    for (int i = 0; i < CIRCLE_DEGREES; i+= (CIRCLE_DEGREES/elipse_res))
    {
        glColor3f(R, G, B);
        glVertex3f(radius_x * cos(i*RADIANS), radius_y * sin(i*RADIANS),0);
    }

    glEnd();
}
void DrawEllipseWithBorder(
    GLfloat radius_x, GLfloat radius_y,
    GLfloat R, GLfloat G, GLfloat B,
    bool points,
    double elipse_res
)
{
    // Border
    DrawEllipse(
        radius_x,radius_y,
        0,0,0,
        false,
        40
    );
    // Actual Object
    DrawEllipse(
        radius_x*(1.0-BORDER_SIZE),radius_y*(1.0-BORDER_SIZE),
        R,G,B,
        false,
        40
    );
}
