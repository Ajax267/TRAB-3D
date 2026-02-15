#include "bullet.h"

bool Bullet::Move(
    CircularArena& arena,
    std::vector<CircularObstacle>& obstacles_vec,
    std::vector<ArenaPlayer>& players_vec, 
    GLdouble timeDiference
)
{
    this->GetPosition().SetX(
        this->GetPosition().GetX()+ this->GetVelocity().GetVx() * timeDiference
    );
    this->GetPosition().SetY(
        this->GetPosition().GetY()+ this->GetVelocity().GetVy() * timeDiference
    );

    if ( this->ObstacleCollision(arena,obstacles_vec) || 
         this->PlayerCollision(arena,players_vec) || 
         !(InArena(arena))
    )
    {
        return false;
    }

    return true;
}

void Bullet::DrawBullet()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            this->GetPosition().GetZ()
        );
        // printf("Bullet Z : %.3f\n",this->GetPosition().GetZ());
        // DrawCircWithBorder(
        //     this->GetRadius(),
        //     this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
        // );
        DrawSphere(
            this->GetRadius(),
            this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB(),
            BULLET_RES
        );
    glPopMatrix();
}

bool Bullet::Delete(CircularArena& arena, std::vector<ArenaPlayer>& players_vec, std::vector<CircularObstacle>& obstacles_vec)
{
    if( this->ObstacleCollision(arena,obstacles_vec) || this->PlayerCollision(arena,players_vec) )
    {
        return true;
    }

    return false;
}

double Bullet::SquareDistanceTo(double x, double y) //, double z)
{
    double dx = fabs(x - this->GetPosition().GetX());
    double dy = fabs(y - this->GetPosition().GetY());
    //double dz = fabs(y - this->GetPosition().GetZ());

    return (dx*dx + dy*dy); //+ dz*dz);
}

bool Bullet::InArena(CircularArena& arena)
{
    return (
        this->SquareDistanceTo(arena.GetPosition().GetX(),arena.GetPosition().GetY()) 
        <= (arena.GetRadius()-this->GetRadius())*(arena.GetRadius()-this->GetRadius())
    );
}

bool Bullet::ObstacleCollision(CircularArena& arena, std::vector<CircularObstacle>& obstacles_vec)
{
    if (this->InArena(arena))
    {
        for (CircularObstacle& obstacle : obstacles_vec)
        {
            double player_distance_from_obstacle_center = this->SquareDistanceTo(
                obstacle.GetPosition().GetX(),
                obstacle.GetPosition().GetY()
            );
            double limit = obstacle.GetRadius() + this->Hitbox();
            if ( player_distance_from_obstacle_center <= limit*limit )
            {
                if (
                    this->GetPosition().GetZ() >= obstacle.GetPosition().GetZ() &&
                    this->GetPosition().GetZ() <= obstacle.GetHeight()
                )
                {
                    return true;
                }
            }
        }
    }
    else
    {
        return true;
    }

    return false;
}

bool Bullet::PlayerCollision(CircularArena& arena, std::vector<ArenaPlayer>& players_vec)
{
    if (this->InArena(arena))
    {
        for (ArenaPlayer& current_player : players_vec)
        {
            if (current_player.GetId() == this->GetOwner()) continue;

            double bullet_distance_from_current_player = this->SquareDistanceTo(
                current_player.GetPosition().GetX(),
                current_player.GetPosition().GetY()
            );
            double limit = current_player.Hitbox() + this->Hitbox();
            if (bullet_distance_from_current_player <= limit * limit)
            {
                if (
                    this->GetPosition().GetZ() >= current_player.GetPosition().GetZ() &&
                    this->GetPosition().GetZ() <= current_player.GetHeight()
                )
                {
                    current_player.GotHit();
                    return true;
                }                
            }
        }
    }
    else
    {
        return true;
    }

    return false;  
}