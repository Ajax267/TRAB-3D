#ifndef UTILS_H
#define UTILS_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>

#include "tinyxml2.h"

// --------------------------------------------- //

class PositionDefinition
{
    private:
        double _x;
        double _y;
        double _z;

    public:
        PositionDefinition() :  _x(0.0),_y(0.0),_z(0.0) {};
        PositionDefinition(double x,double y, double z) 
            : _x(x),_y(y),_z(z) {};
        
        const double& GetX() const { return _x; };
        const double& GetY() const { return _y; };
        const double& GetZ() const { return _z; };

        void SetX(double x) { _x = x; };
        void SetY(double y) { _y = y; };
        void SetZ(double z) { _z = z; };

        void PrintAttributes();
};

class OrientationDefinition
{
    private:
        double _roll;
        double _pitch;
        double _yaw;

    public:
        OrientationDefinition() :  _roll(0.0),_pitch(0.0),_yaw(0.0) {};
        OrientationDefinition(double roll ,double pitch, double yaw) 
            : _roll(roll),_pitch(pitch),_yaw(yaw) {};
        
        const double& GetRoll()  const { return _roll; };
        const double& GetPitch() const { return _pitch; };
        const double& GetYaw()   const { return _yaw; };

        void SetRoll(double roll)   { _roll = roll; };
        void SetPitch(double pitch) { _pitch = pitch; };
        void SetYaw(double yaw)     { _yaw = yaw; };

        void PrintAttributes();
};

class VelocityDefinition
{
    private:
        double _vx;
        double _vy;
        double _vz;

    public:
        VelocityDefinition(double vx,double vy, double vz) 
            : _vx(vx),_vy(vy),_vz(vz) {};
        
        const double& GetVx() const { return _vx; };
        const double& GetVy() const { return _vy; };
        const double& GetVz() const { return _vz; };

        void SetVx(double vx) { _vx = vx; };
        void SetVy(double vy) { _vy = vy; };
        void SetVz(double vz) { _vz = vz; };

        void PrintAttributes();
};

class RGBColor
{
    private:
        double _r;
        double _g;
        double _b;
    public:
        RGBColor() : _r(0), _g(0), _b(0) {};
        RGBColor(double r,double g, double b) : _r(r), _g(g), _b(b) {};

        const double& GetR() const {return _r;};
        const double& GetG() const {return _g;};
        const double& GetB() const {return _b;};

        void SetR(double r) {_r = r;};
        void SetG(double g) {_g = g;};
        void SetB(double b) {_b = b;};

        void PrintAttributes();
};

class StateDefinition : public PositionDefinition
{
    private:
        double _time;
    
    public:
        StateDefinition() : PositionDefinition(0,0,0),_time(0){};
        StateDefinition(double x, double y, double z, double time) 
            : PositionDefinition(x,y,y),_time(time){};
        
        const double& GetTime() const {return _time;};

};

// --------------------------------------------- //

class ObjectDefinition
{
    private:
        PositionDefinition _position;
        OrientationDefinition _orientation;
        std::string color;
        RGBColor rgb;
    public:
        ObjectDefinition() 
          :_position{0.0,0.0,0.0},_orientation{0.0,0.0,0.0},color("NULL"),rgb{}
        {};
        ObjectDefinition(double x, double y, double z,
                        double roll, double pitch, double yaw,
                        const std::string& color
        ) : _position{x, y, z}, _orientation{roll,pitch,yaw}, color(color), rgb{}
            {
                this->SetRBGByColorName();
            }
        
        const std::string& GetColorName() const { return color; };
        void SetColorName(const std::string& c) { color = c; };
        void SetRBGByColorName();
        RGBColor& GetRGB() {return rgb;};

        PositionDefinition& GetPosition() { return _position; };
        OrientationDefinition& GetOrientation() { return _orientation; };


        void PrintAttributes();
};


class CircleDefinition : public ObjectDefinition
{
    private:
        double _radius;  
    public:
        CircleDefinition(double cx, double cy, double radius, const std::string& color)
            : ObjectDefinition(cx,cy,0.0,0.0,0.0,0.0,color), _radius(radius) {};
        
        const double& GetRadius() const { return _radius; };
        void SetRadius(double r) { _radius = r; };
        
};

// --------------------------------------------- //

class EntityDefinition : public ObjectDefinition
{
    private:
        VelocityDefinition velocity;
        PositionDefinition _last_pos;
        bool _is_moving = false;

    public:
        EntityDefinition() 
          : ObjectDefinition(), velocity{0.0,0.0,0.0},_last_pos{0.0,0.0,0.0}
        {};
        EntityDefinition(
            double x, double y, double z,
            double roll ,double pitch, double yaw,
            const std::string& color,
            double vx, double vy, double vz
        ) : ObjectDefinition(x, y, z, roll, pitch, yaw, color), velocity{vx, vy, vz}, _last_pos{x,y,z}
        {};

        VelocityDefinition& GetVelocity() { return velocity; };

        void SetLastPosition(PositionDefinition last_pos) {this->_last_pos = last_pos;};
        const PositionDefinition& GetLastPosition() const {return this->_last_pos;};

        void SetMovingStatus(bool is_moving) {this->_is_moving = is_moving ;};
        const bool& GetMovingStatus() const {return this->_is_moving;};

        bool IsMoving();
};

class CircularEntityDefinition : public EntityDefinition
{
    private:
        double _radius;

    public:
        CircularEntityDefinition() 
          : EntityDefinition(),_radius(0.0) 
        {};
        CircularEntityDefinition(
            double x, double y, double z,
            double roll ,double pitch, double yaw,
            const std::string& color,
            double vx, double vy, double vz,
            double radius
        ) : EntityDefinition(x, y, z, roll, pitch, yaw, color,vx,vy,vz), _radius(radius)
        {};

        const double& GetRadius() const { return _radius; };
        void SetRadius(double r) { _radius = r; };
};

// --------------------------------------------- //

std::vector<CircleDefinition> svg_parser(const char* path);
double dot_product_2d(double x1, double y1, double x2, double y2);
double cross_product_2d(double x1, double y1, double x2, double y2);
void normalize_3d(float a[3]);
void cross_product_3d(float a[3], float b[3], float out[3]);
void vector_3d_difference(float a[3], float b[3], float out[3]);

#endif