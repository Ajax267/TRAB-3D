#ifndef BULLET_H
#define	BULLET_H

#include <math.h>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>

#include "utils.h"
#include "draw_utils.h"
#include "player.h"
#include "arena.h"
#include "arena_obstacles.h"


#define BULLET_RES 10

class ArenaPlayer; // forward declaration

class Bullet : public CircularEntityDefinition
{
    private:

        int _owner;

    public:
        Bullet()
          : CircularEntityDefinition(), _owner(0)
        {};

        Bullet(
            double x, double y, double z,
            double roll ,double pitch, double yaw,
            const std::string& color,
            double vx, double vy, double vz,
            double bullet_radius, int owner
        ) : CircularEntityDefinition(x,y,z,roll,pitch,yaw,color,vx,vy,vz,bullet_radius),
          _owner(owner) 
        {};

        void DrawBullet();
        bool Move(
            CircularArena& arena,
            std::vector<CircularObstacle>& obstacles_vec,
            std::vector<ArenaPlayer>& player_vec, 
            GLdouble timeDiference
        );
        bool Delete(CircularArena& arena, std::vector<ArenaPlayer>& players_vec, std::vector<CircularObstacle>& obstacles_vec);

        // Poderia dizer quero toda entidade da arena tem isso, mas deixa ... se complicar troco

        // Collions Check
        double SquareDistanceTo(double x, double y);
        bool InArena(CircularArena& arena);
        bool PlayerCollision(CircularArena& arena,std::vector<ArenaPlayer>& players_vec);
        bool ObstacleCollision(CircularArena& arena, std::vector<CircularObstacle>& obstacles_vec);

        // Pos and Hitbox
        double Hitbox(){ return this->GetRadius(); };
        int GetOwner(){ return _owner;};


};

#endif	/* TIRO_H */

