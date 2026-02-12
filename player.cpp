#include "player.h"

//----------Drawing------------//

/*void ArenaPlayer::DrawLegs()
{
    if (this->GetMovingStatus())
    {
        glPushMatrix();
            glTranslatef(
                this->GetRadius() * BODY_X_RADIUS_MULTIPLER * LEG_DISTANCE_MULTIPLER,
                0,
                0
            );
            
            if(this->_last_leg_id == LEFT_LEG_ID)
            {
                glRotatef(
                    180,
                    0,0,1
                );
            }
            DrawRect(
                this->GetRadius()*LEG_HEIGHT_MULTIPLER,
                this->GetRadius()*LEG_WIDTH_MULTIPLER,
                0,0,0
            );
        glPopMatrix();

        // Left Leg
        glPushMatrix();
            glTranslatef(
                -this->GetRadius() * BODY_X_RADIUS_MULTIPLER * LEG_DISTANCE_MULTIPLER,
                0,
                0
            );
            if(this->_last_leg_id == RIGHT_LEG_ID)
            {
                glRotatef(
                    180,0,0,1
                );
            }
            DrawRect(
                this->GetRadius()*LEG_HEIGHT_MULTIPLER,
                this->GetRadius()*LEG_WIDTH_MULTIPLER,
                0,0,0
            );
        glPopMatrix();
        this->is_leg_rotated = false;
    }
}


void ArenaPlayer::DrawArm()
{
    glPushMatrix();
        glTranslatef(
            this->GetRadius() * BODY_X_RADIUS_MULTIPLER * ARM_DISTANCE_MULTIPLER,
            0,
            0
        );
        glRotatef(
            this->gun_yaw,
            0,0,1
        );
        DrawRectWithBorder(
            this->GetRadius()*ARM_HEIGHT_MULTIPLER,
            this->GetRadius()*ARM_WIDTH_MULTIPLER,
            this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
        );
    glPopMatrix();
}


void ArenaPlayer::DrawBody()
{
    glPushMatrix();
        DrawEllipseWithBorder(
            BODY_X_RADIUS_MULTIPLER*this->GetRadius(),
            (BODY_X_RADIUS_MULTIPLER/4.0)*this->GetRadius(),
            this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
        );
        DrawCircWithBorder(
            this->GetRadius(),
            this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
        );
    glPopMatrix();
}


void ArenaPlayer::DrawPlayer()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            0
        );
        glRotatef(
            this->GetOrientation().GetYaw(),
            0,0,1
        );
        this->DrawLegs();
        this->DrawArm();
        this->DrawBody();
    // this->GetPosition().PrintAttributes();
    glPopMatrix();
}


bool ArenaPlayer::IsMoving()
{
    return(
        this->GetPosition().GetX()-this->last_animation_attempt_position.GetX() +
        this->GetPosition().GetY()-this->last_animation_attempt_position.GetY() +
        this->GetPosition().GetZ()-this->last_animation_attempt_position.GetZ() != 0
    );
}


void ArenaPlayer::Animate()
{
    if(this->IsMoving())
    {
        // this->GetPosition().PrintAttributes();
        // this->last_animation_attempt_position.PrintAttributes();

        this->SetMovingStatus(true);
        this->last_animation_attempt_position = this->GetPosition();
        if (this->GetLastLeg() == RIGHT_LEG_ID)
        {
            this->SetCurrentLeg(LEFT_LEG_ID);
        }
        else this->SetCurrentLeg(RIGHT_LEG_ID);
    }
    else this->SetMovingStatus(false);
}


//----------Interaction------------//

void ArenaPlayer::Move(
    CircularArena& arena,
    std::vector<CircularObstacle>& obstacles_vec,
    std::vector<ArenaPlayer>& player_vec, 
    GLdouble timeDiference)
{
    // Fiz pegando a cópia não referência, consertar depois

    this->SetLastPosition(this->GetPosition());

    this->GetPosition().SetX(
        this->GetPosition().GetX() + this->GetVelocity().GetVx() * timeDiference
    );
    this->GetPosition().SetY(
        this->GetPosition().GetY() + this->GetVelocity().GetVy() * timeDiference
    );
    
    if ( this->ArenaCollision(arena) || 
         this->ObstacleCollision(arena,obstacles_vec) || 
         this->PlayerCollision(arena,player_vec)
    )
    {
        this->GetPosition() = this->GetLastPosition();
    }
}


void ArenaPlayer::Rotate(GLdouble timeDiference)
{
    this->GetOrientation().SetYaw(this->GetOrientation().GetYaw()+PLAYER_ROTATIONAL_SPEED*timeDiference);
    if (this->GetOrientation().GetYaw() >= 360.0)  this->GetOrientation().SetYaw(this->GetOrientation().GetYaw() - 360.0);
    if (this->GetOrientation().GetYaw() <= -360.0) this->GetOrientation().SetYaw(this->GetOrientation().GetYaw() + 360.0);

    // Direction Vector
    this->direction.SetX(sin(this->GetOrientation().GetYaw()*RADIANS));
    this->direction.SetY(cos(this->GetOrientation().GetYaw()*RADIANS));

    // Velocity Vector
    this->GetVelocity().SetVx(-PLAYER_SPEED*this->direction.GetX());
    this->GetVelocity().SetVy(-PLAYER_SPEED*this->direction.GetY());
}


void ArenaPlayer::RotateGun(GLdouble timeDiference)
{
    this->gun_yaw += GUN_ROTATIONAL_SPEED*timeDiference;
    if (this->gun_yaw >= 45.0) this->gun_yaw = 45.0;
    if (this->gun_yaw <= -45.0) this->gun_yaw = -45.0;
}


void ArenaPlayer::Shoot()
{
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            -this->GetPosition().GetY(),
            0
        );
        glRotatef(
            this->GetOrientation().GetYaw(),
            0,0,1
        );
        glTranslatef(
            this->GetRadius() * BODY_X_RADIUS_MULTIPLER * ARM_DISTANCE_MULTIPLER,
            0,
            0
        );
        glRotatef(
            this->gun_yaw,
            0,0,1
        );
        glTranslatef(
            0,
            this->GetRadius()*ARM_HEIGHT_MULTIPLER+ this->GetRadius()*BULLET_RADIUS_SCALER,
            0
        );
        
        // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glGet.xml
        // GL_MODELVIEW_MATRIX
        // params returns sixteen values: 
        // the modelview matrix on the top of the modelview matrix stack.
        // Initially this matrix is the identity matrix.

        GLfloat modelview_matrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

        double bullet_x = modelview_matrix[12]; // X da matriz da translação
        double bullet_y = modelview_matrix[13]; // Y da matriz da translação
        double bullet_z = modelview_matrix[14]; // Z da matriz da translação

        // Inicialmente a bala está alinhada com o eixo Y
        // Portanto pegamos as projeções devido as rotações na coluna do eixo Y
        double bullet_x_angle = modelview_matrix[4]; // Angulo em X
        double bullet_y_angle = modelview_matrix[5]; // Angulo em Y

        this->bullet_vec.emplace_back(
            bullet_x,-bullet_y,bullet_z,
            0.0,0.0,0.0,
            this->GetColorName(),
            BULLET_VEL*bullet_x_angle,
            -BULLET_VEL*bullet_y_angle,0,
            this->GetRadius()*BULLET_RADIUS_SCALER,this->GetId()
        );
    glPopMatrix();
}


//----------Collision------------//


double ArenaPlayer::SquareDistanceTo(double x, double y)
{
    double dx = fabs(x - this->GetPosition().GetX());
    double dy = fabs(y - this->GetPosition().GetY());

    return (dx*dx + dy*dy);
}

bool ArenaPlayer::InArena(CircularArena& arena)
{
    return (
        this->SquareDistanceTo(arena.GetPosition().GetX(),arena.GetPosition().GetY()) 
        <= arena.GetRadius()*arena.GetRadius()
    );
}

bool ArenaPlayer::ArenaCollision(CircularArena& arena)
{
    if (this->InArena(arena))
    {
        double player_distance_from_arena_center = this->SquareDistanceTo(
            arena.GetPosition().GetX(),
            arena.GetPosition().GetY()
        );
        double equivalent_radius = arena.GetRadius() - this->Hitbox();
        return !(player_distance_from_arena_center <= (equivalent_radius*equivalent_radius));
    }
    else
    {
        return true; //Player somehow escaped the arena ??? useful for debug I imagine
    }
    return false;
}

// Obstacles should only exist inside the Arena
bool ArenaPlayer::ObstacleCollision(CircularArena& arena, std::vector<CircularObstacle>& obstacles_vec)
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
                return true;
            }
        }
    }
    else
    {
        return true; //Player somehow escaped the arena ??? useful for debug I imagine
    }

    return false;
}

bool ArenaPlayer::PlayerCollision(CircularArena& arena, std::vector<ArenaPlayer>& players_vec)
{
    if (this->InArena(arena))
    {
        for (ArenaPlayer& current_player : players_vec)
        {
            if (&current_player == this) continue;

            double player_distance_from_current_player = this->SquareDistanceTo(
                current_player.GetPosition().GetX(),
                current_player.GetPosition().GetY()
            );
            double limit = current_player.Hitbox() + this->Hitbox();
            if (player_distance_from_current_player <= limit * limit)
            {
                return true;
            }
        }
    }
    else
    {
        return true; //Player somehow escaped the arena ??? useful for debug I imagine
    }

    return false;  
}*/
#include "player.h"
#include <GL/glut.h> // Necessário para glutSolidSphere/Cube

// ==========================================
// SEÇÃO DE DESENHO (NOVO 3D)
// ==========================================

// Função auxiliar para desenhar o corpo como um cilindro
void DrawCylinder(float radius, float height, float r, float g, float b)
{
    GLUquadricObj *quadric = gluNewQuadric();
    glColor3f(r, g, b);
    gluCylinder(quadric, radius, radius, height, 20, 5); // Base, Topo, Altura, Fatias, Pilhas
    
    // Desenha uma tampa no topo para não ficar oco
    glPushMatrix();
        glTranslatef(0, 0, height);
        gluDisk(quadric, 0, radius, 20, 1);
    glPopMatrix();
    
    gluDeleteQuadric(quadric);
}

void ArenaPlayer::DrawPlayer()
{
    float r = this->GetRGB().GetR();
    float g = this->GetRGB().GetG();
    float b = this->GetRGB().GetB();
    float radius = this->GetRadius();
    float height = radius * 2.0; // Altura proporcional ao raio

    glPushMatrix();
        // 1. Posiciona no mundo (X, Y)
        glTranslatef(this->GetPosition().GetX(), this->GetPosition().GetY(), 0);

        // 2. Rotaciona o corpo (Yaw)
        glRotatef(this->GetOrientation().GetYaw(), 0, 0, 1);

        // 3. Desenha Corpo (Cilindro)
        DrawCylinder(radius, height, r, g, b);

        // 4. Desenha Cabeça (Esfera no topo do cilindro)
        glPushMatrix();
            glTranslatef(0, 0, height); // Sobe para o pescoço
            glColor3f(r, g, b); 
            glutSolidSphere(radius * 0.6, 20, 20); // Cabeça um pouco menor que o corpo
            
            // Nariz/Olhos para indicar direção (ajuda a ver a rotação)
            glTranslatef(radius*0.5, 0, 0); // Move para frente (Eixo X local do boneco)
            glColor3f(0, 0, 0); // Preto
            glutSolidSphere(radius * 0.2, 10, 10);
        glPopMatrix();

        // 5. Desenha Arma (Paralelepípedo simples saindo do lado)
        glPushMatrix();
             glTranslatef(radius*0.8, 0, height*0.6); // Lado direito, altura do peito
             glRotatef(this->gun_yaw, 0, 0, 1); // Rotação da arma
             glScalef(radius*1.5, radius*0.3, radius*0.3); // Comprido e fino
             glColor3f(0.2, 0.2, 0.2); // Cinza escuro
             glutSolidCube(1.0);
        glPopMatrix();

    glPopMatrix();
}

// Funções de desenho antigas (Mantidas vazias ou simplificadas pois DrawPlayer cuida de tudo agora)
void ArenaPlayer::DrawLegs() {}
void ArenaPlayer::DrawArm() {}
void ArenaPlayer::DrawBody() {}

// ==========================================
// SEÇÃO DE LÓGICA E FÍSICA (RESTAURADA)
// ==========================================

bool ArenaPlayer::IsMoving()
{
    return(
        this->GetPosition().GetX()-this->last_animation_attempt_position.GetX() +
        this->GetPosition().GetY()-this->last_animation_attempt_position.GetY() +
        this->GetPosition().GetZ()-this->last_animation_attempt_position.GetZ() != 0
    );
}

void ArenaPlayer::Animate()
{
    if(this->IsMoving())
    {
        this->SetMovingStatus(true);
        this->last_animation_attempt_position = this->GetPosition();
        if (this->GetLastLeg() == RIGHT_LEG_ID)
        {
            this->SetCurrentLeg(LEFT_LEG_ID);
        }
        else this->SetCurrentLeg(RIGHT_LEG_ID);
    }
    else this->SetMovingStatus(false);
}


//----------Interaction------------//

void ArenaPlayer::Move(
    CircularArena& arena,
    std::vector<CircularObstacle>& obstacles_vec,
    std::vector<ArenaPlayer>& player_vec, 
    GLdouble timeDiference)
{
    this->SetLastPosition(this->GetPosition());

    this->GetPosition().SetX(
        this->GetPosition().GetX() + this->GetVelocity().GetVx() * timeDiference
    );
    this->GetPosition().SetY(
        this->GetPosition().GetY() + this->GetVelocity().GetVy() * timeDiference
    );
    
    if ( this->ArenaCollision(arena) || 
         this->ObstacleCollision(arena,obstacles_vec) || 
         this->PlayerCollision(arena,player_vec)
    )
    {
        this->GetPosition() = this->GetLastPosition();
    }
}


void ArenaPlayer::Rotate(GLdouble timeDiference)
{
    this->GetOrientation().SetYaw(this->GetOrientation().GetYaw()+PLAYER_ROTATIONAL_SPEED*timeDiference);
    if (this->GetOrientation().GetYaw() >= 360.0)  this->GetOrientation().SetYaw(this->GetOrientation().GetYaw() - 360.0);
    if (this->GetOrientation().GetYaw() <= -360.0) this->GetOrientation().SetYaw(this->GetOrientation().GetYaw() + 360.0);

    // Direction Vector
    this->direction.SetX(sin(this->GetOrientation().GetYaw()*RADIANS));
    this->direction.SetY(cos(this->GetOrientation().GetYaw()*RADIANS));

    // Velocity Vector
    this->GetVelocity().SetVx(-PLAYER_SPEED*this->direction.GetX());
    this->GetVelocity().SetVy(-PLAYER_SPEED*this->direction.GetY());
}


void ArenaPlayer::RotateGun(GLdouble timeDiference)
{
    this->gun_yaw += GUN_ROTATIONAL_SPEED*timeDiference;
    if (this->gun_yaw >= 45.0) this->gun_yaw = 45.0;
    if (this->gun_yaw <= -45.0) this->gun_yaw = -45.0;
}


void ArenaPlayer::Shoot()
{
    // Nota: O cálculo da posição da bala pode precisar de ajustes finos
    // para alinhar perfeitamente com o novo modelo 3D, mas vamos manter a lógica
    // funcional para compilar.
    
    glPushMatrix();
        glTranslatef(
            this->GetPosition().GetX(),
            this->GetPosition().GetY(), // Alterado sinal de Y para consistência com novo modelo
            0
        );
        glRotatef(
            this->GetOrientation().GetYaw(),
            0,0,1
        );
        
        // Ajuste baseado na nova posição da arma no DrawPlayer
        // A arma está em: radius*0.8 (X), 0 (Y), height*0.6 (Z)
        float height = this->GetRadius() * 2.0;
        
        glTranslatef(
            this->GetRadius() * 0.8, // X local (lado)
            0,
            height * 0.6 // Z (altura) - mas o calculo de tiro original ignora Z para velocidade
        );
        
        glRotatef(
            this->gun_yaw,
            0,0,1
        );
        
        // Avança para a ponta do cano
        glTranslatef(this->GetRadius() * 1.5, 0, 0); 
        
        GLfloat modelview_matrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

        double bullet_x = modelview_matrix[12]; 
        double bullet_y = modelview_matrix[13]; 
        double bullet_z = modelview_matrix[14]; 

        double bullet_x_angle = modelview_matrix[0]; // Eixo X da matriz (Direção do cano no mundo)
        double bullet_y_angle = modelview_matrix[1]; // Eixo Y da matriz

        // No modelo antigo, a velocidade era calculada baseada em índices 4 e 5.
        // Como giramos 90 graus ou mudamos o eixo, pode ser 0 e 1 agora.
        // Vamos usar o vetor X local da matriz (que é pra onde a arma aponta no novo modelo cube)
        
        this->bullet_vec.emplace_back(
            bullet_x, bullet_y, bullet_z,
            0.0,0.0,0.0,
            this->GetColorName(),
            BULLET_VEL * bullet_x_angle, // Vx
            BULLET_VEL * bullet_y_angle, // Vy
            0,
            this->GetRadius()*BULLET_RADIUS_SCALER, this->GetId()
        );
    glPopMatrix();
}


//----------Collision------------//


double ArenaPlayer::SquareDistanceTo(double x, double y)
{
    double dx = fabs(x - this->GetPosition().GetX());
    double dy = fabs(y - this->GetPosition().GetY());

    return (dx*dx + dy*dy);
}

bool ArenaPlayer::InArena(CircularArena& arena)
{
    return (
        this->SquareDistanceTo(arena.GetPosition().GetX(),arena.GetPosition().GetY()) 
        <= arena.GetRadius()*arena.GetRadius()
    );
}

bool ArenaPlayer::ArenaCollision(CircularArena& arena)
{
    if (this->InArena(arena))
    {
        double player_distance_from_arena_center = this->SquareDistanceTo(
            arena.GetPosition().GetX(),
            arena.GetPosition().GetY()
        );
        double equivalent_radius = arena.GetRadius() - this->Hitbox();
        return !(player_distance_from_arena_center <= (equivalent_radius*equivalent_radius));
    }
    else
    {
        return true; 
    }
    return false;
}

bool ArenaPlayer::ObstacleCollision(CircularArena& arena, std::vector<CircularObstacle>& obstacles_vec)
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
                return true;
            }
        }
    }
    else
    {
        return true; 
    }

    return false;
}

bool ArenaPlayer::PlayerCollision(CircularArena& arena, std::vector<ArenaPlayer>& players_vec)
{
    if (this->InArena(arena))
    {
        for (ArenaPlayer& current_player : players_vec)
        {
            if (&current_player == this) continue;

            double player_distance_from_current_player = this->SquareDistanceTo(
                current_player.GetPosition().GetX(),
                current_player.GetPosition().GetY()
            );
            double limit = current_player.Hitbox() + this->Hitbox();
            if (player_distance_from_current_player <= limit * limit)
            {
                return true;
            }
        }
    }
    else
    {
        return true; 
    }

    return false;  
}