#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"
#include "arena.h"
#include "arena_obstacles.h"
#include "bullet.h"
#include "objloader.h"

#include <vector>
#include <string>

#include <stdio.h>
#include <math.h>


#define PLAYER1_ID 1
#define PLAYER2_ID 2

#define PLAYER_SPEED 64.0 // Para mover para cima e para baixo se mantenha consistente
#define PLAYER_ROTATIONAL_SPEED 100.0 // Para mover para cima e para baixo se mantenha consistente
#define GUN_ROTATIONAL_SPEED 100.0 // Para mover para cima e para baixo se mantenha consistente
#define PLAYER_HEALTH 3

extern double player_height;
#define OBSTACLE_HEIGHT_SCALE 1.0
#define MAX_JUMP_HEIGHT_SCALE 2.0
#define PLAYER_RES 10

#define BULLET_VEL (PLAYER_SPEED*2.0)
#define BULLET_RADIUS_SCALER 0.2
#define BODY_X_RADIUS_MULTIPLER 2

#define ARM_DISTANCE_MULTIPLER 0.7
#define ARM_WIDTH_MULTIPLER 0.4
#define ARM_HEIGHT_MULTIPLER 2.0

#define LEG_DISTANCE_MULTIPLER 0.2
#define LEG_WIDTH_MULTIPLER (ARM_WIDTH_MULTIPLER * 1.2)
#define LEG_HEIGHT_MULTIPLER ARM_HEIGHT_MULTIPLER

#define LEFT_LEG_ID 1
#define RIGHT_LEG_ID 2

#define JUMP_DECAY_ARENA 0
#define JUMP_DECAY_OBSTACLE 1
#define JUMP_DECAY_PLAYER 2
#define MAX_JUMP_HEIGHT PLAYER_HEIGHT*2
#define LANTERN_Z_SCALE 1.5

class Bullet; // forward declaration

class ArenaPlayer : public CircularEntityDefinition
{
    private:
        PositionDefinition direction = {0,1,0}; // a Classe podia estar com outro nome, porém apenas aproveitei a classe
        PositionDefinition last_animation_attempt_position;
        std::vector<Bullet> bullet_vec;
        double gun_yaw = 0.0;
        double gun_roll = 0.0;
        short health = PLAYER_HEALTH;
        int _id;
        short _last_leg_id = LEFT_LEG_ID;
        bool is_leg_rotated = false;
        int _soldadoMov = -1;
        int _soldadoFrame = 0;
        double _soldadoAccumMs = 0.0;
        bool on_obstacle = false;
        bool on_player = false;
        float current_jump_height = 0;
        bool jump_decay = false;
        short jump_decay_type = JUMP_DECAY_ARENA;
        
    public:
        ArenaPlayer()
          : CircularEntityDefinition(),gun_yaw(0),_id(0)
        {};
        ArenaPlayer(
            double x, double y, double z,
            double roll ,double pitch, double yaw,
            const std::string& color,
            double vx, double vy, double vz,
            double head_radius, int id

        ) :  CircularEntityDefinition(x,y,z,roll,pitch,yaw,color,vx,vy,vz,head_radius),
            last_animation_attempt_position({x,y,z}),
            gun_yaw(0),
            _id(id)
        {};

        // Drawing
        void DrawBody();
        void DrawArm();
        void DrawLegs();
        void DrawPlayer();
        void Animate();
        void DrawLanternLight();

        // Player interaction -> Moving,Rotating and Shooting
        void Rotate(GLdouble timeDiference);
        void RotateGunYaw(GLdouble timeDiference);
        void RotateGunRoll(GLdouble timeDiference);
        void Move(
            CircularArena& arena,
            std::vector<CircularObstacle>& obstacles_vec,
            std::vector<ArenaPlayer>& player_vec, 
            GLdouble timeDiference
        );
        void Shoot();
        void GotHit() { this->health--;};
        bool IsMoving();
        void IncreaseHeight(GLdouble timeDiference,int jump_button_status);
        void DecreaseHeight(GLdouble timeDiference, ArenaPlayer player);
        void UpdateDecayType(
            CircularArena& arena,
            std::vector<CircularObstacle>& obstacles_vec,
            std::vector<ArenaPlayer>& player_vec
        );

        // Collions Check
        double SquareDistanceTo(double x, double y);
        bool InArena(CircularArena& arena);
        bool ArenaCollision(CircularArena& arena);
        bool ObstacleCollision(CircularArena& arena, std::vector<CircularObstacle>& obstacles_vec);
        bool PlayerCollision(CircularArena& arena, std::vector<ArenaPlayer>& players_vec);

        // Pos and Hitbox
        double Hitbox(){ return this->GetRadius(); };
        int& GetId() { return this->_id; };
        std::vector<Bullet>& GetBulletVec() { return this->bullet_vec; };
        short GetLastLeg() {return this->_last_leg_id;};
        void SetCurrentLeg(short leg_id) {this->_last_leg_id = leg_id;};
        pos GetEyePos();

        const short& GetHealth() const {return this->health;};
        void SetHealth(short health) {this->health=health;};

        void SetDirection(PositionDefinition dir) {this->direction=dir;};
        PositionDefinition GetDirection() const {return this->direction;};
        
        void SetGunYaw(double g_yaw) {this->gun_yaw=g_yaw;};
        void SetGunPitch(double g_pitch) {this->gun_roll=g_pitch;};

        void SetLastAnimationAttemptPosition(PositionDefinition pos)
            {this->last_animation_attempt_position = pos;};

    
        void UpdateSoldadoAnim(double dtMs, bool andando);
        int GetSoldadoMov() const { return _soldadoMov;};
        int GetSoldadoFrame() const { return _soldadoFrame;};

        const bool& GetJumpDecay() const {return this->jump_decay;};

};

#endif
