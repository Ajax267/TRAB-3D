#include "utils.h"
#include "imageloader.h"

using namespace tinyxml2;

void PositionDefinition::PrintAttributes()
{
    printf(" Position:");
    printf(" x: %.2f|", this->GetX());
    printf(" y: %.2f|", this->GetY());
    printf(" z:  %.2f", this->GetZ());
    printf("\n\n");
}

void OrientationDefinition::PrintAttributes()
{
    printf(" Orientation:");
    printf(" roll: %.2f|",  this->GetRoll());
    printf(" pitch: %.2f|", this->GetPitch());
    printf(" yaw:  %.2f",   this->GetYaw());
    printf("\n\n");
}


void VelocityDefinition::PrintAttributes()
{
    printf(" Velocity:");
    printf(" vx: %.2f|", this->GetVx());
    printf(" vy: %.2f|", this->GetVy());
    printf(" vz:  %.2f", this->GetVz());
    printf("\n\n");
}


void RGBColor::PrintAttributes()
{
    printf(" Colors:");
    printf(" r: %.2f|", this->GetR());
    printf(" g: %.2f|", this->GetG());
    printf(" b:  %.2f", this->GetB());
    printf("\n\n");
}


void ObjectDefinition::PrintAttributes()
{
    printf(" Circle:");
    printf(" cx: %.2f|", this->GetPosition().GetX());
    printf(" cy: %.2f|", this->GetPosition().GetY());
    // printf(" radius: %.2f|", this->GetRadius());
    printf(" color: %s", this->GetColorName().c_str());
    printf("\n\n");
}


void ObjectDefinition::SetRBGByColorName()
{
    if (this->color == "black")
    {
        this->rgb.SetR(0.0);
        this->rgb.SetG(0.0);
        this->rgb.SetB(0.0);
    }

    if (this->color == "red")
    {
        this->rgb.SetR(1.0);
        this->rgb.SetG(0.0);
        this->rgb.SetB(0.0);
    }

    if (this->color == "green")
    {
        this->rgb.SetR(0.0);
        this->rgb.SetG(1.0);
        this->rgb.SetB(0.0);
    }

    if (this->color == "blue")
    {
        this->rgb.SetR(0.0);
        this->rgb.SetG(0.0);
        this->rgb.SetB(1.0);
    }

    if (this->color == "white")
    {
        this->rgb.SetR(1.0);
        this->rgb.SetG(1.0);
        this->rgb.SetB(1.0);
    }
}


// Não Funciona porque o LastPosition apenas atualiza quando move
// Resultado -> Sempre retorna que está move
// Se der tempo penso em algo melhor
// A posição é usada para colisão 
// Por enquanto checo no loop do OpenGL mesmo
// Pegando duas posições no tempo e vendo se são iguais
bool EntityDefinition::IsMoving()
{
    return(
        this->GetPosition().GetX()-this->GetLastPosition().GetX() +
        this->GetPosition().GetY()-this->GetLastPosition().GetY() +
        this->GetPosition().GetZ()-this->GetLastPosition().GetZ() != 0
    );
}


std::vector<CircleDefinition> svg_parser(const char* path)
{
    XMLDocument doc;
    doc.LoadFile(path);
	
    XMLElement* svg = doc.FirstChildElement("svg");
    if (!(svg != NULL))
    {
        printf("Elemento <svg> não encontrado!\n");
        return {};
    }

    std::vector<CircleDefinition> circle_vec;
    for (XMLElement* circle = svg->FirstChildElement("circle"); circle != nullptr; circle = circle->NextSiblingElement("circle"))
    {
        double cx = 0, cy = 0, radius = 0;
        const char* color = nullptr;

        circle->QueryDoubleAttribute("cx", &cx);
        circle->QueryDoubleAttribute("cy", &cy);
        circle->QueryDoubleAttribute("r", &radius);
        color = circle->Attribute("fill");
        CircleDefinition circle_class = CircleDefinition(cx,cy,radius,color);
        circle_vec.push_back(circle_class);
    }

    return circle_vec;
}


// As funções partem do princípio que x1,y1 = (0,0)
double dot_product_2d(double x1, double y1, double x2, double y2)
{
    return (x1*x2+y1*y2);
}

double cross_product_2d(double x1, double y1, double x2, double y2)
{
    return (x1*y2-x1*y1);
}
double vector_2d_squared_size(double x1, double y1)
{
    return (x1*x1+y1*y1);
}

//Funcao auxiliar para normalizar um vetor a/|a|
void normalize_3d(float a[3])
{
    double norm = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]); 
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}

//Funcao auxiliar para fazer o produto vetorial entre dois vetores a x b = out
void cross_product_3d(float a[3], float b[3], float out[3])
{
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

void vector_3d_difference(float a[3], float b[3], float out[3])
{
    out[0] = a[0] - b[0];
    out[1] = a[1] - b[1];
    out[2] = a[2] - b[2];
}

void dot_product_3d(float a[3], float b[3], float out[3])
{
    out[0] = a[0]*b[0];
    out[1] = a[1]*b[1];
    out[2] = a[2]*b[2];
}


void ChangeCoordSys(
    GLdouble ax, GLdouble ay, GLdouble az,
    GLdouble bx, GLdouble by, GLdouble bz,
    GLdouble upx, GLdouble upy, GLdouble upz
)
{
    float x[3], y[3], z[3];
    float m[16] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    y[0] = ax - bx; y[1] = ay - by; y[2] = az - bz;
    normalize_3d(y);

    float up[3] = { (float) upx, (float) upy, (float) upz};
    normalize_3d(up);

    cross_product_3d(y, up, x);
    normalize_3d(x);

    cross_product_3d(x, y, z);
    normalize_3d(z);

    m[0] = x[0]; m[4] = y[0]; m[8]  = z[0]; m[12] = bx;
    m[1] = x[1]; m[5] = y[1]; m[9]  = z[1]; m[13] = by;
    m[2] = x[2]; m[6] = y[2]; m[10] = z[2]; m[14] = bz;

    glMultMatrixf(m);
}



GLuint LoadTextureRAW( const char * filename )
{

    GLuint texture;
    
    Image* image = loadBMP(filename);

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                             0,                            //0 for now
                             GL_RGB,                       //Format OpenGL uses for image
                             image->width, image->height,  //Width and height
                             0,                            //The border of the image
                             GL_RGB, //GL_RGB, because pixels are stored in RGB format
                             GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                                               //as unsigned numbers
                             image->pixels);               //The actual pixel data
    delete image;

    return texture;
}

void DrawTexturedFloor(float radius, float z_position, GLuint textureID)
{
    float repeticoes = 4.0f;
    int numSegments = 64;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLfloat mat_white[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_white);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_TRIANGLE_FAN);

    glNormal3f(0.0f, 0.0f, 1.0f);                    
    glTexCoord2f(repeticoes / 2.0f, repeticoes / 2.0f); 
    glVertex3f(0.0f, 0.0f, z_position);

    for (int i = 0; i <= numSegments; i++)
    {
        float angle = (2.0f * M_PI * i) / numSegments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

       
        float u = (x / (radius * 2)) * repeticoes + (repeticoes / 2);
        float v = (y / (radius * 2)) * repeticoes + (repeticoes / 2);

        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(u, v);
        glVertex3f(x, y, z_position);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


void DrawTexturedCylinder(float radius, float height, GLuint textureID)
{
    int numSegments = 64;
    float repeticoes = 10.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLfloat mat_white[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_white);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSegments; i++)
    {
        float angle = (2.0f * M_PI * i) / numSegments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        float u = ((float)i / numSegments) * repeticoes;

        glNormal3f(-x / radius, -y / radius, 0.0f);

        glTexCoord2f(u, 0.0f);
        glVertex3f(x, y, 0.0f);

        glTexCoord2f(u, 1.0f);
        glVertex3f(x, y, height);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


void DrawTexturedCylinderOutside(float radius, float height, GLuint textureID)
{
    int numSegments = 32; 
    float repeticoes = 1.0f; 

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLfloat mat_white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_white);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSegments; i++)
    {
        float angle = (2.0f * M_PI * i) / numSegments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        float u = ((float)i / numSegments) * repeticoes;

        glNormal3f(x / radius, y / radius, 0.0f);

        glTexCoord2f(u, 1.0f);
        glVertex3f(x, y, height);

        glTexCoord2f(u, 0.0f);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

