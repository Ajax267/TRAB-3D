#include "bullet.h"

extern GLuint g_texturaBala;
extern bool toggle_texture;

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
    this->GetPosition().SetZ(
        this->GetPosition().GetZ()+ this->GetVelocity().GetVz() * timeDiference
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

        if (toggle_texture)
        {
            DrawTexturedSphere(
                this->GetRadius(), 
                16, 16, 
                g_texturaBala
            );
        }
        else
        {
            // Código antigo (Cor sólida)
            DrawSphere(
                this->GetRadius(),
                this->GetRGB().GetR(), this->GetRGB().GetG(), this->GetRGB().GetB(),
                BULLET_RES
            );
        }
        
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

    return (dx*dx + dy*dy);
}

bool Bullet::InArena(CircularArena& arena)
{
    if (this->GetPosition().GetZ() < 0) //Arena tem origem em 0
    {
        return false; 
    }
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
                    (this->GetPosition().GetZ() + this->GetRadius() >= obstacle.GetPosition().GetZ() &&
                    this->GetPosition().GetZ() + this->GetRadius() <= obstacle.GetHeightScale() * player_height) ||
                    (this->GetPosition().GetZ() - this->GetRadius() >= obstacle.GetPosition().GetZ() &&
                    this->GetPosition().GetZ() - this->GetRadius() <=obstacle.GetHeightScale() * player_height)
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
                    (this->GetPosition().GetZ() + this->GetRadius() >= current_player.GetPosition().GetZ() &&
                    this->GetPosition().GetZ() + this->GetRadius() <= current_player.GetPosition().GetZ() + player_height) ||
                    ((this->GetPosition().GetZ() - this->GetRadius() >= current_player.GetPosition().GetZ() &&
                    this->GetPosition().GetZ() - this->GetRadius() <= current_player.GetPosition().GetZ() + player_height))
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