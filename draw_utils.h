#ifndef _DRAW_UTILS_H
#define _DRAW_UTILS_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <math.h>

#define RADIANS (M_PI / 180.0)

#define CIRCLE_DEGREES 360.0
#define CIRCULE_RES 60.0
#define POINT_SIZE 4.0
#define BORDER_SIZE 0.1

void DrawRect(
    GLint height, GLint width,
    GLfloat R, GLfloat G, GLfloat B
);
void DrawRectWithBorder(
    GLint height, GLint width,
    GLfloat R, GLfloat G, GLfloat B
);
void DrawCirc(
    GLint radius,
    GLfloat R, GLfloat G,GLfloat B,
    bool points = false,
    double circle_res = CIRCULE_RES
);
void DrawCircWithBorder(
    GLint radius,
    GLfloat R, GLfloat G,GLfloat B,
    bool points = false,
    double circle_res = CIRCULE_RES
);
void DrawEllipse(
    GLfloat radius_x, GLfloat radius_y,
    GLfloat R, GLfloat G, GLfloat B,
    bool points = false,
    double circle_res = CIRCULE_RES
);
void DrawEllipseWithBorder(
    GLfloat radius_x, GLfloat radius_y,
    GLfloat R, GLfloat G, GLfloat B,
    bool points = false,
    double circle_res = CIRCULE_RES
);

#endif