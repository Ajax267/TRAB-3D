#include "utils.h"

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