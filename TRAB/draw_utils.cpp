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
    int draw_mode,
    double circle_res
)
{
    glColor3f (R, G, B);
    glBegin(draw_mode);
        for (double i = 0.0; i <= CIRCLE_DEGREES; i+= (CIRCLE_DEGREES/circle_res) )
        {
            
            if (draw_mode == GL_POINTS) glPointSize(POINT_SIZE);
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

// -----------------3D Drawing--------------------//

void DrawAxes(double size)
{
    GLfloat mat_ambient_r[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient_g[] = { 0.0, 1.0, 0.0, 1.0 };
    GLfloat mat_ambient_b[] = { 0.0, 0.0, 1.0, 1.0 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, 
            no_mat);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

    //x axis red
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, 
                mat_ambient_r);
        glColor3fv(mat_ambient_r);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //y axis green
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, 
                mat_ambient_g);
        glColor3fv(mat_ambient_g);
        glRotatef(90,0,0,1);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();

    //z axis blue
    glPushMatrix();
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
        glColor3fv(mat_ambient_b);
        glRotatef(-90,0,1,0);
        glScalef(size, size*0.1, size*0.1);
        glTranslatef(0.5, 0, 0); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();    
}

void DrawObj(double size)
{
   GLfloat materialEmission[] = { 0.00, 0.00, 0.00, 1.0};
   GLfloat materialColor[] = { 1.0, 1.0, 0.0, 1.0};
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0};
   GLfloat mat_shininess[] = { 128 };
   glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
   glMaterialfv(GL_FRONT, GL_AMBIENT, materialColor);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glColor3f(1,0,0);
   //glutSolidSphere(size, 20, 10);
   glutSolidCube(size);
}

void DrawOpenCilinder(GLdouble radius, GLdouble height, GLfloat r, GLfloat g, GLfloat b, GLint slices_stacks)
{
    GLfloat materialEmission[] = { 0.00, 0.00, 0.00, 1.0};
    GLfloat ambientColor[] = { r, g, b, 0.1};
    GLfloat diffuseColor[] = { r, g, b, 0.5};
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = { 128 };
    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glColor3f(r,g,b);

    glPushMatrix();
        GLUquadric* quadratic = gluNewQuadric();
        gluQuadricOrientation(quadratic, GLU_INSIDE);
        gluQuadricNormals(quadratic, GLU_SMOOTH);

        // Lateral
        gluCylinder(quadratic,radius,radius,height,slices_stacks,slices_stacks);

        gluDeleteQuadric(quadratic);
    glPopMatrix();
}

void DrawClosedCilinder(GLdouble radius, GLdouble height, GLfloat r, GLfloat g, GLfloat b, GLint slices_stacks)
{
    GLfloat materialEmission[] = { 0.00, 0.00, 0.00, 1.0};
    GLfloat ambientColor[] = { r, g, b, 0.1};
    GLfloat diffuseColor[] = { r, g, b, 1.0};
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = { 128 };
    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glColor3f(r,g,b);

    glPushMatrix();
        GLUquadric* quadratic = gluNewQuadric();
        gluQuadricNormals(quadratic, GLU_SMOOTH);

        // Lateral
        gluCylinder(quadratic,radius,radius,height,slices_stacks,slices_stacks);

        // Base 
        glPushMatrix();
        glRotatef(180, 1,0,0);   // gira para desenhar voltada para baixo
        gluDisk(quadratic, 0, radius, slices_stacks, 1);
        glPopMatrix();

        // Topo
        glPushMatrix();
        glTranslatef(0,0,height);
        gluDisk(quadratic, 0, radius, slices_stacks, 1);
        glPopMatrix();

        gluDeleteQuadric(quadratic);
    glPopMatrix();
}

void DrawRect3D(
    double size_x, double size_y, double size_z, 
    GLfloat r, GLfloat g, GLfloat b,
    double rot, int rot_x, int rot_y, int rot_z,
    double trans_x, double trans_y, double trans_z
)
{
    GLfloat diffuseColor[] = { r,g,b, 1.0 };
    GLfloat ambientColor[] = { r,g,b, 0.1 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);
    glColor3f(r,g,b);

    glPushMatrix();
        // glMaterialfv(GL_FRONT, GL_EMISSION, 
        //         mat_ambient_g);
        // glColor3fv(mat_ambient_g);
        glRotatef(rot,rot_x,rot_y,rot_z);
        glScalef(size_x, size_y, size_z);
        glTranslatef(trans_x, trans_y, trans_z); // put in one end
        glutSolidCube(1.0);
    glPopMatrix();
  
}

void DrawSphere(GLdouble radius, GLfloat r, GLfloat g, GLfloat b, GLint slices_stacks)
{
    GLfloat diffuseColor[] = { r,g,b, 1.0 };
    GLfloat ambientColor[] = { r,g,b, 0.1 };
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);
    glColor3f(r,g,b);

    //y axis green
    glPushMatrix();
        // glMaterialfv(GL_FRONT, GL_EMISSION, 
        //         mat_ambient_g);
        // glColor3fv(mat_ambient_g);
        glTranslatef(radius/2, 0, 0); // put in one end
        glutSolidSphere(radius, 20, 10);
    glPopMatrix();
}