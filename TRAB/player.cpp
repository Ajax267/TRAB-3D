#include "player.h"
#include "objloader.h"
#include "utils.h"

extern meshes g_soldado;
extern bool g_drawSoldado;
extern int g_movIdle;
extern int g_movWalking;
extern float g_modelScale, g_modelRotX, g_modelRotZ;

extern meshes g_arma;
extern int movArma;

extern bool g_debugCompareModel;

static const int IDX_HAND_POS = 9263;
static const int IDX_HAND_AIM = 8480;
static const int IDX_HAND_UP = -1;
static const int IDX_HAND_SIDE = 8133;
static const int IDX_EYE_POS = 21913;

extern bool night_mode;

//valores pra mover os tiros ate o cano
static float MUZZLE_X = 0.0f;
static float MUZZLE_Y = 30.0f;
static float MUZZLE_Z = 0.0f;

//----------Drawing------------//

void ArenaPlayer::DrawLegs()
{
    if (this->GetMovingStatus())
    {
        glPushMatrix();
        glTranslatef(
            this->GetRadius() * BODY_X_RADIUS_MULTIPLER * LEG_DISTANCE_MULTIPLER,
            0,
            0);

        if (this->_last_leg_id == LEFT_LEG_ID)
        {
            glRotatef(
                180,
                0, 0, 1);
        }
        DrawRect(
            this->GetRadius() * LEG_HEIGHT_MULTIPLER,
            this->GetRadius() * LEG_WIDTH_MULTIPLER,
            0, 0, 0);
        glPopMatrix();

        // Left Leg
        glPushMatrix();
        glTranslatef(
            -this->GetRadius() * BODY_X_RADIUS_MULTIPLER * LEG_DISTANCE_MULTIPLER,
            0,
            0);
        if (this->_last_leg_id == RIGHT_LEG_ID)
        {
            glRotatef(
                180, 0, 0, 1);
        }
        DrawRect(
            this->GetRadius() * LEG_HEIGHT_MULTIPLER,
            this->GetRadius() * LEG_WIDTH_MULTIPLER,
            0, 0, 0);
        glPopMatrix();
        this->is_leg_rotated = false;
    }
}



void ArenaPlayer::DrawLanternLight()
{
    int player_light = 0;
    if (this->_id == PLAYER1_ID)
    {
        player_light = GL_LIGHT1;
    }
    if (this->_id == PLAYER2_ID)
    {
        player_light = GL_LIGHT2;    
    }

    if (g_drawSoldado)
    {
        GLfloat modelview_matrix[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);        
        GLfloat light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(player_light, GL_POSITION, light_position);
        
        GLfloat light_dir[] = { 
            modelview_matrix[4], 
            modelview_matrix[5],  
            modelview_matrix[6], 
        };
        glLightfv(player_light, GL_SPOT_DIRECTION, light_dir);
    }
    else
    {
        glPushMatrix();
            glTranslatef(
                (this->GetRadius()*ARM_WIDTH_MULTIPLER - this->GetRadius()*BULLET_RADIUS_SCALER),
                this->GetRadius()*ARM_HEIGHT_MULTIPLER + this->GetRadius()*BULLET_RADIUS_SCALER,
                -this->GetRadius()*ARM_WIDTH_MULTIPLER*LANTERN_Z_SCALE
            );
            DrawRect3D(
                this->GetRadius() * ARM_HEIGHT_MULTIPLER * 0.2,
                this->GetRadius() * ARM_WIDTH_MULTIPLER * 0.2,
                this->GetRadius() * ARM_WIDTH_MULTIPLER * 0.2,
                this->GetRGB().GetR(), this->GetRGB().GetG(), this->GetRGB().GetB()
            ); 
            
            GLfloat light_center[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glLightfv(player_light, GL_POSITION, light_center);
            
            GLfloat light_dir[] = { 0.0f, -1.0f, 0.0f};
            glLightfv(player_light, GL_SPOT_DIRECTION, light_dir);
        glPopMatrix();
    }
    
    glLightf(player_light, GL_SPOT_CUTOFF, 20.0f);
    glLightf(player_light, GL_SPOT_EXPONENT, 15.0f);
    
    if (!night_mode)
    {
        GLfloat light_off[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glLightfv(player_light, GL_DIFFUSE, light_off);
        glLightfv(player_light, GL_AMBIENT, light_off);
        glLightfv(player_light, GL_SPECULAR, light_off);
    }
    else
    {
        GLfloat light_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
        glLightfv(player_light, GL_DIFFUSE, light_diffuse);
        
        GLfloat light_ambient[] = { 0.15f, 0.15f, 0.15f, 1.0f };
        glLightfv(player_light, GL_AMBIENT, light_ambient);
        
        GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(player_light, GL_SPECULAR, light_specular);
    }
}



void ArenaPlayer::DrawArm()
{
    glPushMatrix();
    glTranslatef(
        this->GetRadius(),
        0,
        player_height // Braço vai ficar no meio do cilindro
    );
    // glTranslatef(
    //     this->GetRadius() * BODY_X_RADIUS_MULTIPLER * ARM_DISTANCE_MULTIPLER,
    //     0,
    //     0
    // );
    glRotatef(
        this->gun_yaw,
        0, 0, 1
    );
    glRotatef(
        this->gun_roll,
        1, 0, 0
    );
    // DrawRectWithBorder(
    //     this->GetRadius()*ARM_HEIGHT_MULTIPLER,
    //     this->GetRadius()*ARM_WIDTH_MULTIPLER,
    //     this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
    // );
    DrawRect3D(
        this->GetRadius() * ARM_HEIGHT_MULTIPLER,
        this->GetRadius() * ARM_WIDTH_MULTIPLER,
        this->GetRadius() * ARM_WIDTH_MULTIPLER,
        this->GetRGB().GetR(), this->GetRGB().GetG(), this->GetRGB().GetB()
    );
    this->DrawLanternLight();
    glPopMatrix();
}



void ArenaPlayer::DrawBody()
{
    glPushMatrix();
        DrawClosedCilinder(
            this->GetRadius(),
            player_height,
            this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB(),
            PLAYER_RES
        );
        // DrawEllipseWithBorder(
        //     BODY_X_RADIUS_MULTIPLER*this->GetRadius(),
        //     (BODY_X_RADIUS_MULTIPLER/4.0)*this->GetRadius(),
        //     this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
        // );
        // DrawCircWithBorder(
        //     this->GetRadius(),
        //     this->GetRGB().GetR(),this->GetRGB().GetG(),this->GetRGB().GetB()
        // );
    glPopMatrix();
}



void ArenaPlayer::DrawPlayer()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(this->GetPosition().GetX(), -this->GetPosition().GetY(), this->GetPosition().GetZ());
    glRotatef(this->GetOrientation().GetYaw(), 0, 0, 1);

    if (g_drawSoldado)
    {
        if (g_debugCompareModel)
        {
            glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
            DrawClosedCilinder(this->GetRadius(), player_height, 1, 1, 0, PLAYER_RES);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glPopAttrib();
        }
        glPushMatrix();

        glRotatef(g_modelRotZ, 0, 0, 1);
        glRotatef(g_modelRotX, 1, 0, 0);

        glScalef(g_modelScale, g_modelScale, g_modelScale);

        int mov = this->GetSoldadoMov();
        int frame = this->GetSoldadoFrame();
        if (mov < 0)
        {
            mov = g_movIdle;
            frame = 0;
        }

        g_soldado.draw(mov, frame);

        mesh &m = g_soldado.vecMeshes[mov][frame];
        glPushMatrix();
        float upx = 0, upy = 0, upz = 1;
        if (IDX_HAND_UP >= 0)
        {
            upx = m.vertsPos[IDX_HAND_UP].x - m.vertsPos[IDX_HAND_POS].x;
            upy = m.vertsPos[IDX_HAND_UP].y - m.vertsPos[IDX_HAND_POS].y;
            upz = m.vertsPos[IDX_HAND_UP].z - m.vertsPos[IDX_HAND_POS].z;
        }

        ChangeCoordSys(
            m.vertsPos[IDX_HAND_AIM].x, m.vertsPos[IDX_HAND_AIM].y, m.vertsPos[IDX_HAND_AIM].z,
            m.vertsPos[IDX_HAND_POS].x, m.vertsPos[IDX_HAND_POS].y, m.vertsPos[IDX_HAND_POS].z,
            upx, upy, upz
        );

        // sweet spot da rotação
        float correcaoPulso = 15.0f;

        if (mov == g_movWalking)
        {
            correcaoPulso = -30.0f;
        }

        glRotatef(correcaoPulso, 0, 1, 0);
        // ajustando os eixos da arma com o resto
        glRotatef(this->gun_yaw, -1, 0, 0);
        glRotatef(this->gun_roll, 0, 0, 1);

        g_arma.draw(movArma, 0);

        glTranslatef(MUZZLE_X, MUZZLE_Y, MUZZLE_Z);

        glPushMatrix();

        glTranslatef(0.0f, 0.0f, -1.0f);

        this->DrawLanternLight();

        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
    }
    else
    {
        // this->DrawLegs();
        //  glTranslatef(
        //      this->GetPosition().GetX(),
        //      -this->GetPosition().GetY(),
        //      this->GetPosition().GetZ()
        //  );
        //  glRotatef(
        //      this->GetOrientation().GetYaw(),
        //      0,0,1
        //  );
        this->DrawBody();
        this->DrawArm();
    }
    glPopMatrix();
}



bool ArenaPlayer::IsMoving()
{
    return (
        this->GetPosition().GetX() - this->last_animation_attempt_position.GetX() +
            this->GetPosition().GetY() - this->last_animation_attempt_position.GetY() +
            this->GetPosition().GetZ() - this->last_animation_attempt_position.GetZ() !=
        0);
}



void ArenaPlayer::Animate()
{
    if (this->IsMoving())
    {
        // this->GetPosition().PrintAttributes();
        // this->last_animation_attempt_position.PrintAttributes();

        this->SetMovingStatus(true);
        this->last_animation_attempt_position = this->GetPosition();
        if (this->GetLastLeg() == RIGHT_LEG_ID)
        {
            this->SetCurrentLeg(LEFT_LEG_ID);
        }
        else
            this->SetCurrentLeg(RIGHT_LEG_ID);
    }
    else
        this->SetMovingStatus(false);
}



pos ArenaPlayer::GetEyePos()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glTranslatef(this->GetPosition().GetX(), -this->GetPosition().GetY(), this->GetPosition().GetZ());
        glRotatef(this->GetOrientation().GetYaw(), 0, 0, 1);
        glRotatef(g_modelRotZ, 0, 0, 1);
        glRotatef(g_modelRotX, 1, 0, 0);
        glScalef(g_modelScale, g_modelScale, g_modelScale);
        int mov = this->GetSoldadoMov();
        int frame = this->GetSoldadoFrame();

        if (mov < 0)
        {
            mov = g_movIdle;
            frame = 0;
        }

        mesh &m = g_soldado.vecMeshes[mov][frame];
        // float upx = 0, upy = 0, upz = 1;
        pos v = m.vertsPos[IDX_EYE_POS];
        // float normalized[3]= {v.x,v.y,v.z};
        // normalize_3d(normalized);
        // ChangeCoordSys(
        //     m.vertsPos[IDX_EYE_POS].x, m.vertsPos[IDX_EYE_POS].y, m.vertsPos[IDX_EYE_POS].z,
        //     m.vertsPos[IDX_EYE_POS].x + normalized[0], m.vertsPos[IDX_EYE_POS].y + normalized[1], m.vertsPos[IDX_EYE_POS].z + normalized[2],
        //     upx, upy, upz
        // );

        // float px = m.vertsPos[IDX_EYE_POS].x;
        // float py = m.vertsPos[IDX_EYE_POS].y;
        // float pz = m.vertsPos[IDX_EYE_POS].z;

        // GLfloat modelview_matrix[16];
        // glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

        // float m_x = modelview_matrix[0]*px + modelview_matrix[1]*py + modelview_matrix[2]*pz + modelview_matrix[12];
        // float m_y = modelview_matrix[4]*px + modelview_matrix[5]*py + modelview_matrix[6]*pz + modelview_matrix[13];
        // float m_z = modelview_matrix[8]*px + modelview_matrix[9]*py + modelview_matrix[10]*pz + modelview_matrix[14];

        // v.x = m_x;
        // v.y = m_y;
        // v.z = m_z;
        
        glPopMatrix();
        return v;

}

//----------Interaction------------//



void ArenaPlayer::Move(
    CircularArena &arena,
    std::vector<CircularObstacle> &obstacles_vec,
    std::vector<ArenaPlayer> &player_vec,
    GLdouble timeDiference)
{
    // Fiz pegando a cópia não referência, consertar depois

    this->SetLastPosition(this->GetPosition());

    this->GetPosition().SetX(
        this->GetPosition().GetX() + this->GetVelocity().GetVx() * timeDiference);
    this->GetPosition().SetY(
        this->GetPosition().GetY() + this->GetVelocity().GetVy() * timeDiference);

    bool arena_c = this->ArenaCollision(arena);
    bool object_c = this->ObstacleCollision(arena, obstacles_vec);
    bool player_c = this->PlayerCollision(arena, player_vec);
    if (arena_c || object_c || player_c)
    {
        this->GetPosition() = this->GetLastPosition();
    }
}



void ArenaPlayer::Rotate(GLdouble timeDiference)
{
    this->GetOrientation().SetYaw(this->GetOrientation().GetYaw() + PLAYER_ROTATIONAL_SPEED * timeDiference);
    if (this->GetOrientation().GetYaw() >= 360.0)
        this->GetOrientation().SetYaw(this->GetOrientation().GetYaw() - 360.0);
    if (this->GetOrientation().GetYaw() <= -360.0)
        this->GetOrientation().SetYaw(this->GetOrientation().GetYaw() + 360.0);

    // Direction Vector
    this->direction.SetX(sin(this->GetOrientation().GetYaw() * RADIANS));
    this->direction.SetY(cos(this->GetOrientation().GetYaw() * RADIANS));

    // Velocity Vector
    this->GetVelocity().SetVx(-PLAYER_SPEED * this->direction.GetX());
    this->GetVelocity().SetVy(-PLAYER_SPEED * this->direction.GetY());
}



void ArenaPlayer::RotateGunYaw(GLdouble timeDiference)
{
    this->gun_yaw += GUN_ROTATIONAL_SPEED * timeDiference;
    if (this->gun_yaw >= 45.0)
        this->gun_yaw = 45.0;
    if (this->gun_yaw <= -45.0)
        this->gun_yaw = -45.0;
    // printf("gun_yaw %3.f\n",gun_yaw);
}



void ArenaPlayer::RotateGunRoll(GLdouble timeDiference)
{
    this->gun_roll += GUN_ROTATIONAL_SPEED * timeDiference;
    if (this->gun_roll >= 45.0)
        this->gun_roll = 45.0;
    if (this->gun_roll <= -45.0)
        this->gun_roll = -45.0;
    // printf("gun_yaw %3.f\n",gun_yaw);
}



void ArenaPlayer::Shoot()
{
    glPushMatrix();
    glLoadIdentity(); // Camera interfere no viewmodel matrix
    glTranslatef(
        this->GetPosition().GetX(),
        -this->GetPosition().GetY(),
        this->GetPosition().GetZ());
    glRotatef(
        this->GetOrientation().GetYaw(),
        0, 0, 1);
    if (g_drawSoldado)
    {
        glRotatef(g_modelRotZ, 0, 0, 1);
        glRotatef(g_modelRotX, 1, 0, 0);
        glScalef(g_modelScale, g_modelScale, g_modelScale);

        int mov = this->GetSoldadoMov();
        int frame = this->GetSoldadoFrame();
        if (mov < 0)
        {
            mov = g_movIdle;
            frame = 0;
        }

        mesh &m = g_soldado.vecMeshes[mov][frame];

        float upx = 0, upy = 0, upz = 1;
        if (IDX_HAND_UP >= 0)
        {
            upx = m.vertsPos[IDX_HAND_UP].x - m.vertsPos[IDX_HAND_POS].x;
            upy = m.vertsPos[IDX_HAND_UP].y - m.vertsPos[IDX_HAND_POS].y;
            upz = m.vertsPos[IDX_HAND_UP].z - m.vertsPos[IDX_HAND_POS].z;
        }

        ChangeCoordSys(
            m.vertsPos[IDX_HAND_AIM].x, m.vertsPos[IDX_HAND_AIM].y, m.vertsPos[IDX_HAND_AIM].z,
            m.vertsPos[IDX_HAND_POS].x, m.vertsPos[IDX_HAND_POS].y, m.vertsPos[IDX_HAND_POS].z,
            upx, upy, upz);

        // mesma rotação da arma
        float correcaoPulso = 15.0f;

        if (mov == g_movWalking)
        {           
            correcaoPulso = -30.0f;
        }
        glRotatef(correcaoPulso, 0, 1, 0);
        glRotatef(this->gun_yaw, -1, 0, 0);
        glRotatef(this->gun_roll, 0, 0, 1);

        // anda até o cano
        glTranslatef(MUZZLE_X, MUZZLE_Y, MUZZLE_Z);
    }
    else
    {
        glTranslatef(
            this->GetRadius(),
            0,
            player_height
        );
        glRotatef(
            this->gun_yaw,
            0, 0, 1);
        glRotatef(
            this->gun_roll,
            1, 0, 0);
        // Center bullet on the gun
        glTranslatef(
            (this->GetRadius() * ARM_WIDTH_MULTIPLER - this->GetRadius() * BULLET_RADIUS_SCALER),
            this->GetRadius() * ARM_HEIGHT_MULTIPLER + this->GetRadius() * BULLET_RADIUS_SCALER,
            -this->GetRadius() * ARM_WIDTH_MULTIPLER + this->GetRadius() * BULLET_RADIUS_SCALER
        );
        // this->GetRadius() * ARM_HEIGHT_MULTIPLER,
        // this->GetRadius() * ARM_WIDTH_MULTIPLER,
        // this->GetRadius() * ARM_WIDTH_MULTIPLER,
    }
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
    double bullet_z_angle = modelview_matrix[6];

    this->bullet_vec.emplace_back(
        bullet_x, -bullet_y, bullet_z,
        0.0, 0.0, 0.0,
        this->GetColorName(),
        BULLET_VEL * bullet_x_angle,
        -BULLET_VEL * bullet_y_angle,
        BULLET_VEL * bullet_z_angle,
        this->GetRadius() * BULLET_RADIUS_SCALER, this->GetId());
    glPopMatrix();
}



void ArenaPlayer::IncreaseHeight(GLdouble timeDiference, int jump_button_status)
{
    // printf("passe aqui 0\n");
    if (!jump_button_status)
        this->jump_decay = true; // Ativa a queda se solta o botão do pulo

    if (!this->jump_decay)
    {
        // printf("passe aqui 1\n");
        float jump_delta = MAX_JUMP_HEIGHT_SCALE * player_height * timeDiference;
        this->current_jump_height += jump_delta;
        this->GetPosition().SetZ(this->GetPosition().GetZ() + jump_delta);
    }
    if (this->current_jump_height >= MAX_JUMP_HEIGHT_SCALE * player_height)
    {
        // printf("passe aqui 2\n");
        this->current_jump_height = MAX_JUMP_HEIGHT_SCALE * player_height;
        this->jump_decay = true; // Ativa a queda se a altura de pulo máximo é atinginda
    }
}



void ArenaPlayer::DecreaseHeight(GLdouble timeDiference, ArenaPlayer player)
{
    float current_z = this->GetPosition().GetZ();

    if (this->jump_decay)
    {
        // printf("jump_decay_type %d\n",jump_decay_type);
        current_z -= MAX_JUMP_HEIGHT_SCALE * player_height * timeDiference;
        if (jump_decay_type == JUMP_DECAY_ARENA)
        {
            if (current_z <= 0)
            {
                current_z = 0;
                this->jump_decay = false;
                this->current_jump_height = 0.0;
            }
        }
        else if (jump_decay_type == JUMP_DECAY_OBSTACLE)
        {
            if (current_z <= OBSTACLE_HEIGHT_SCALE * player_height)
            {
                current_z = OBSTACLE_HEIGHT_SCALE * player_height;
                this->jump_decay = false;
                this->current_jump_height = 0.0;
            }
        }
        else if (jump_decay_type == JUMP_DECAY_PLAYER)
        {
            if (current_z <= player.GetPosition().GetZ() + player_height)
            {
                current_z = player.GetPosition().GetZ() + player_height;
                this->jump_decay = false;
                this->current_jump_height = 0.0;
            }
        }
        this->GetPosition().SetZ(current_z);
    }
}



void ArenaPlayer::UpdateDecayType(
    CircularArena &arena,
    std::vector<CircularObstacle> &obstacles_vec,
    std::vector<ArenaPlayer> &player_vec)
{

    bool arena_c = this->ArenaCollision(arena);
    bool object_c = this->ObstacleCollision(arena, obstacles_vec);
    bool player_c = this->PlayerCollision(arena, player_vec);
    if (arena_c || object_c || player_c)
    {
        this->GetPosition() = this->GetLastPosition();
    }
    // //Hierarquia dos pulos
    this->jump_decay_type = JUMP_DECAY_ARENA;
    if (this->on_obstacle)
        this->jump_decay_type = JUMP_DECAY_OBSTACLE;
    if (this->on_player)
        this->jump_decay_type = JUMP_DECAY_PLAYER;
}

//----------Collision------------//

double ArenaPlayer::SquareDistanceTo(double x, double y)
{
    double dx = fabs(x - this->GetPosition().GetX());
    double dy = fabs(y - this->GetPosition().GetY());

    return (dx * dx + dy * dy);
}



bool ArenaPlayer::InArena(CircularArena &arena)
{
    return (
        this->SquareDistanceTo(arena.GetPosition().GetX(), arena.GetPosition().GetY()) <= arena.GetRadius() * arena.GetRadius());
}



bool ArenaPlayer::ArenaCollision(CircularArena &arena)
{
    if (this->InArena(arena))
    {
        double player_distance_from_arena_center = this->SquareDistanceTo(
            arena.GetPosition().GetX(),
            arena.GetPosition().GetY());
        double equivalent_radius = arena.GetRadius() - this->Hitbox();
        return !(player_distance_from_arena_center <= (equivalent_radius * equivalent_radius));
    }
    else
    {
        return true; // Player somehow escaped the arena ??? useful for debug I imagine
    }
    return false;
}



// Obstacles should only exist inside the Arena
bool ArenaPlayer::ObstacleCollision(CircularArena &arena, std::vector<CircularObstacle> &obstacles_vec)
{
    if (this->InArena(arena))
    {
        for (CircularObstacle &obstacle : obstacles_vec)
        {
            double player_distance_from_obstacle_center = this->SquareDistanceTo(
                obstacle.GetPosition().GetX(),
                obstacle.GetPosition().GetY());
            double limit = obstacle.GetRadius() + this->Hitbox();
            if (player_distance_from_obstacle_center <= limit * limit)
            {
                if (
                    this->GetPosition().GetZ() >= obstacle.GetPosition().GetZ() &&
                    this->GetPosition().GetZ() < obstacle.GetHeightScale() * player_height)
                {
                    return true;
                }

                this->on_obstacle = true;
                return false;
            }
        }
    }
    else
    {
        return true; // Player somehow escaped the arena ??? useful for debug I imagine
    }

    this->on_obstacle = false;
    return false;
}



bool ArenaPlayer::PlayerCollision(CircularArena &arena, std::vector<ArenaPlayer> &players_vec)
{
    if (this->InArena(arena))
    {
        for (ArenaPlayer &current_player : players_vec)
        {
            if (&current_player == this)
                continue;

            double player_distance_from_current_player = this->SquareDistanceTo(
                current_player.GetPosition().GetX(),
                current_player.GetPosition().GetY());
            double limit = current_player.Hitbox() + this->Hitbox();
            if (player_distance_from_current_player <= limit * limit)
            {
                if (
                    this->GetPosition().GetZ() >= current_player.GetPosition().GetZ() &&
                    this->GetPosition().GetZ() < current_player.GetPosition().GetZ() + player_height)
                {
                    return true;
                }
                else if (this->GetPosition().GetZ() >= current_player.GetPosition().GetZ() + player_height)
                {
                    this->on_player = true;
                    return false;
                }
            }
        }
    }
    else
    {
        return true; // Player somehow escaped the arena ??? useful for debug I imagine
    }

    this->on_player = false;
    return false;
}



void ArenaPlayer::UpdateSoldadoAnim(double dtMs, bool andando)
{
    const int targetMov = andando ? g_movWalking : g_movIdle;

    if (_soldadoMov != targetMov)
    {
        _soldadoMov = targetMov;
        _soldadoFrame = 0;
        _soldadoAccumMs = 0.0;
    }
    const double frameMs = 20.0;
    _soldadoAccumMs += dtMs;

    while (_soldadoAccumMs >= frameMs)
    {
        _soldadoAccumMs -= frameMs;
        _soldadoFrame++;

        int maxFrames = (int)g_soldado.vecMeshes[_soldadoMov].size();
        if (maxFrames <= 0)
        {
            _soldadoFrame = 0;
            break;
        }
        if (_soldadoFrame >= maxFrames)
        {
            _soldadoFrame = 0;
        }
    }
}
