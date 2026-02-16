#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "utils.h"
#include "arena_parser.h"
#include "arena.h"
#include "arena_obstacles.h"
#include "player.h"
#include "bullet.h"
#include "objloader.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <math.h>

#include <stdlib.h>

#define INC_KEY 10

#define SPECIAL_KEY 231      // isso é para o 'ç'
#define CAPS_SPECIAL_KEY 199 // isso é para o 'ç
#define ESC_KEY 27
#define SPACE_BAR 32

#define LEFT_CLICK 0
#define RIGHT_CLICK 1
#define MOUSE_PRESSED 0
#define MOUSE_RELEASED 1

#define TIME_S 0.001
#define LEG_ANIMATION_DELAY_MS 350.0
#define WEAPON_FIRERATE 200.0
#define JUMP_DELAY 3.0

// Nos PCs do LabGradII 'MOUSE_SENSITIVY 2.0' estava muito alto
#define MOUSE_SENSITIVY 1.2

// debug
int debug = 0;

// Key status
int keyStatus[256];

// Window dimensions
const GLint Width = 500;
const GLint Height = 500;

// View
GLint VWidth;
GLint VHeight;

// Arena, Obstacles, Players and Bullets
bool first_start = true;
std::vector<PositionDefinition> initial_players_pos;
std::vector<double> initial_players_angle;

CircularArena g_arena;
std::vector<CircularObstacle> g_obstacles;
std::vector<ArenaPlayer> g_players;

GLdouble last_animation_attempt_time = 0;
std::vector<PositionDefinition> last_players_recorded_pos;

std::vector<GLdouble> last_time_player_shoot = {0.0, 0.0};

// podia fazem um enum mas preguiça ...
#define PLAYER1_ID 1
#define PLAYER2_ID 2

#define NO_PLAYER -2
#define DRAW -1
#define PLAYER1_WON PLAYER1_ID
#define PLAYER2_WON PLAYER2_ID

#define PLACAR_PLAYER1 PLAYER1_ID
#define PLACAR_PLAYER2 PLAYER2_ID

bool game_finished = false;
short int game_winner = NO_PLAYER;

// Debugging Vel
PositionDefinition past_p1_pos;
PositionDefinition past_b1_pos;

// Counts how many times the ball was hit
static char str[1000];
void *font = GLUT_BITMAP_9_BY_15;

// Mouse
float gCurrentMouseX = 0;
float gCurrentMouseY = 0;
float gPastMouseX = 0;
float gPastMouseY = 0;

int LeftMouseButton = -1;
int LeftMouseState = -1;
int RightMouseButton = -1;
int RightMouseState = -1;

// Camera Modes

// Enable Spectator Mode
int camera_spectator_mode = 0;
int toggle_player_camera = 0;

// Camera movement
float camera_coords[3] = {0.0, 0.0, 0.0};
float look_camera_coords[3] = {0.0, 0.0, 0.0};
float old_look_camera_coords[3] = {0.0, 0.0, 0.0};
float camera_up_vec[3] = {0.0, 0.0, 0.0};
float camXZAngle = 0;
float camXYAngle = 0;

// Meshes
meshes g_soldado;
bool g_drawSoldado = false;
int g_movIdle = -1;
int g_movWalking = -1;
float g_modelScale = 0.5f;
float g_modelRotX = 90.0f;
float g_modelRotZ = 180.0f;

meshes g_arma;
int movArma = -1;
// Disable Features
int toggle_light = 0;
int toggle_texture = 0;

bool g_debugCompareModel = true;

void init_player1_camera(void)
{
    camera_coords[0] = g_players[0].GetPosition().GetX();
    camera_coords[1] = -g_players[0].GetPosition().GetY();
    camera_coords[2] = g_players[0].GetPosition().GetZ() + PLAYER_HEIGHT;

    // float center_start[3] = {0.0,0.0,0.0};
    // vector_3d_difference(center_start,camera_coords,look_camera_coords);
    // normalize_3d(look_camera_coords);

    // camXYAngle = asin( look_camera_coords[1] ) * 180.0/M_PI;
    // camXZAngle = atan2( look_camera_coords[0], -look_camera_coords[2] ) * 180.0/M_PI;
}

void init_arena_camera(void)
{
    camera_coords[0] = 0;
    camera_coords[1] = -400; //-g_arena.GetRadius()*2.5; -400,400
    camera_coords[2] = 400;  // g_arena.GetRadius()*1.5;

    float center_start[3] = {0.0, 0.0, 0.0};
    vector_3d_difference(center_start, camera_coords, look_camera_coords);
    normalize_3d(look_camera_coords);

    camXYAngle = asin(look_camera_coords[1]) * 180.0 / M_PI;
    camXZAngle = atan2(look_camera_coords[0], -look_camera_coords[2]) * 180.0 / M_PI;
}

/**
 * @brief Updates Mouse coordinates.
 * @param x Mouse X Coordinates
 * @param y Mouse Y Coordinates
 */
void globalmouseMotion(int x, int y)
{
    gPastMouseX = gCurrentMouseX;
    gPastMouseY = gCurrentMouseY;
    gCurrentMouseX = (x / ((float)VHeight));
    gCurrentMouseY = ((Height - y) / ((float)Height));

    if (RightMouseButton == RIGHT_CLICK && RightMouseState == MOUSE_PRESSED && camera_spectator_mode)
    {

        camXZAngle -= (gCurrentMouseX - gPastMouseX) * 50;
        camXYAngle += (gCurrentMouseY - gPastMouseY) * 50;

        // // camXYAngle= 0;
        // if(camXYAngle > 180.0)  camXYAngle = 180.0;
        // if(camXYAngle < -180.0) camXYAngle = -180.0;

        // // camXZAngle= 0;
        // if(camXZAngle > 170.0)  camXYAngle = 170.0;
        // if(camXZAngle < -170.0) camXYAngle = -170.0;

        look_camera_coords[0] = -cos(camXYAngle * M_PI / 180.0) * sin(camXZAngle * M_PI / 180.0);
        look_camera_coords[1] = sin(camXYAngle * M_PI / 180.0);
        look_camera_coords[2] = -cos(camXYAngle * M_PI / 180.0) * cos(camXZAngle * M_PI / 180.0);

        float anti_up_look[3] = {0.0, 0.0, 0.0};
        dot_product_3d(look_camera_coords, camera_up_vec, anti_up_look);
        if ((anti_up_look[0] * anti_up_look[0] +
             anti_up_look[1] * anti_up_look[1] +
             anti_up_look[2] * anti_up_look[2]) == 0)
        {
            look_camera_coords[0] = old_look_camera_coords[0];
            look_camera_coords[1] = old_look_camera_coords[1];
            look_camera_coords[2] = old_look_camera_coords[2];
        }
        else
        {
            old_look_camera_coords[0] = look_camera_coords[0];
            old_look_camera_coords[1] = look_camera_coords[1];
            old_look_camera_coords[2] = look_camera_coords[2];
        }

        glutPostRedisplay();
    }
}

/**
 * @brief Get Mouse button and its state.
 * @param button button number
 * @param state state=0 -> Released || state=1-> Pressed
 * @param x Mouse X Coordinates
 * @param y Mouse Y Coordinates
 */
void mouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        LeftMouseButton = LEFT_CLICK;
        LeftMouseState = MOUSE_PRESSED;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        LeftMouseButton = -1;
        LeftMouseState = MOUSE_RELEASED;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        RightMouseButton = RIGHT_CLICK;
        RightMouseState = MOUSE_PRESSED;
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        RightMouseButton = -1;
        RightMouseState = MOUSE_RELEASED;
    }
}

/**
 * @brief Updates Player2 all Bullets.
 * @param timeDiference diferença de tempo milissegundo entre duas execuções da mesma função
 */
void Player2_Bullets(GLdouble timeDiference)
{
    std::vector<Bullet> &bullet_vec = g_players[1].GetBulletVec();
    for (unsigned int i = 0; i < bullet_vec.size(); i++)
    {
        Bullet &bullet = bullet_vec[i];
        if (!bullet.Move(g_arena, g_obstacles, g_players, timeDiference))
        {
            bullet_vec.erase(bullet_vec.begin() + i);
            // As proximas balas são movidas para trás
            // logo o índice tem que atrasar para não pular um elemento
            i--;
        }
    }
}

/**
 * @brief Updates Player1 all Bullets.
 * @param timeDiference diferença de tempo milissegundo entre duas execuções da mesma função
 */
void Player1_Bullets(GLdouble timeDiference)
{
    std::vector<Bullet> &bullet_vec = g_players[0].GetBulletVec();
    for (unsigned int i = 0; i < bullet_vec.size(); i++)
    {
        Bullet &bullet = bullet_vec[i];
        if (!bullet.Move(g_arena, g_obstacles, g_players, timeDiference))
        {
            bullet_vec.erase(bullet_vec.begin() + i);
            // As proximas balas são movidas para trás
            // logo o índice tem que atrasar para não pular um elemento
            i--;
        }
    }
}

/**
 * @brief Updates Player2 based on pressed Keys.
 * @param timeDiference diferença de tempo milissegundo entre duas execuções da mesma função
 * @param currentTime Tempo decorrido desde o inicio do jogo em milissegundo
 */
void Player2_Keys(GLdouble timeDiference, GLdouble currentTime)
{
    // Treat keyPress
    ArenaPlayer &p2 = g_players[1];
    if (keyStatus[(int)('o')])
    {
        p2.Move(g_arena, g_obstacles, g_players, timeDiference);
    }
    if (keyStatus[(int)('l')])
    {
        p2.Move(g_arena, g_obstacles, g_players, -timeDiference);
    }
    if (keyStatus[(int)('k')])
    {
        if (!keyStatus[(int)'.'] && !p2.GetJumpDecay())
        {
            p2.Rotate(timeDiference);
        }
    }
    if (keyStatus[SPECIAL_KEY])
    {
        if (!keyStatus[(int)'.'] && !p2.GetJumpDecay())
        {
            p2.Rotate(-timeDiference);
        }
    }
    if (keyStatus[(int)('5')] &&
        (currentTime - last_time_player_shoot[1]) > WEAPON_FIRERATE)
    {
        p2.Shoot();
        last_time_player_shoot[1] = currentTime;
    }

    if (keyStatus[(int)('4')])
    {
        p2.RotateGunYaw(timeDiference);
    }
    if (keyStatus[(int)('6')])
    {
        p2.RotateGunYaw(-timeDiference);
    }

    if (keyStatus[(int)('2')])
    {
        p2.RotateGunRoll(-timeDiference * MOUSE_SENSITIVY);
    }
    if (keyStatus[(int)('8')])
    {
        p2.RotateGunRoll(timeDiference * MOUSE_SENSITIVY);
    }

    p2.IncreaseHeight(timeDiference, keyStatus[(int)'.']);

    // Queria fazer genérico, mas vai ficar hardcoded como jogador 2
    // Porque estamos sem tempo
    ArenaPlayer p1 = g_players[0];
    p2.DecreaseHeight(timeDiference, p1);
    p2.UpdateDecayType(g_arena, g_obstacles, g_players);
}

/**
 * @brief Updates Player1 based on pressed Keys.
 * @param timeDiference diferença de tempo milissegundo entre duas execuções da mesma função
 * @param currentTime Tempo decorrido desde o inicio do jogo em milissegundo
 */
void Player1_Keys(GLdouble timeDiference, GLdouble currentTime)
{
    ArenaPlayer &p1 = g_players[0];
    // Treat keyPress
    if (keyStatus[(int)('w')])
    {
        // printf("Jogador Andando\n");
        p1.Move(g_arena, g_obstacles, g_players, timeDiference);
    }
    if (keyStatus[(int)('s')])
    {
        p1.Move(g_arena, g_obstacles, g_players, -timeDiference);
    }
    if (keyStatus[(int)('a')])
    {
        if (!keyStatus[SPACE_BAR] && !p1.GetJumpDecay())
        {
            p1.Rotate(timeDiference);
        }
    }
    if (keyStatus[(int)('d')])
    {
        if (!keyStatus[SPACE_BAR] && !p1.GetJumpDecay())
        {
            p1.Rotate(-timeDiference);
        }
    }
    if (LeftMouseButton == LEFT_CLICK && LeftMouseState == MOUSE_PRESSED &&
        (currentTime - last_time_player_shoot[0]) > WEAPON_FIRERATE)
    {
        p1.Shoot();
        last_time_player_shoot[0] = currentTime;
    }

    // Movimento é Horizontal, quis fazer um movimento adaptativo em relação
    // à orientação do personagem, mas vou deixar isso parado por hora

    // printf("Current Mouse X %.2f\n",gCurrentMouseX);
    // printf("Past Mouse X %.2f\n",gPastMouseX);
    double mouse_x_angle = gCurrentMouseX - gPastMouseX;
    if (mouse_x_angle < 0)
    {
        p1.RotateGunYaw(timeDiference * MOUSE_SENSITIVY);
    }
    if (mouse_x_angle > 0)
    {
        p1.RotateGunYaw(-timeDiference * MOUSE_SENSITIVY);
    }

    double mouse_y_angle = gCurrentMouseY - gPastMouseY;
    if (mouse_y_angle < 0)
    {
        p1.RotateGunRoll(-timeDiference * MOUSE_SENSITIVY);
    }
    if (mouse_y_angle > 0)
    {
        p1.RotateGunRoll(timeDiference * MOUSE_SENSITIVY);
    }

    p1.UpdateDecayType(g_arena, g_obstacles, g_players);
    p1.IncreaseHeight(timeDiference, keyStatus[SPACE_BAR]);

    // Queria fazer genérico, mas vai ficar hardcoded como jogador 2
    // Porque estamos sem tempo
    ArenaPlayer p2 = g_players[1];
    p1.DecreaseHeight(timeDiference, p2);
}

void SpectatorMode_Keys(double deltaTime)
{
    // velocidade da câmera
    double camera_speed = 50.0 * deltaTime;

    if (keyStatus['w'])
    {
        camera_coords[0] += look_camera_coords[0] * camera_speed;
        camera_coords[1] += look_camera_coords[1] * camera_speed;
        camera_coords[2] += look_camera_coords[2] * camera_speed;
    }
    if (keyStatus['s'])
    {
        camera_coords[0] -= look_camera_coords[0] * camera_speed;
        camera_coords[1] -= look_camera_coords[1] * camera_speed;
        camera_coords[2] -= look_camera_coords[2] * camera_speed;
    }
}

/**
 * @brief Anima todos os jogadores presentes.
 * @param currentTime Tempo decorrido desde o inicio do jogo em milissegundo
 */
void AnimatePlayers(GLdouble currentTime)
{
    if ((currentTime - last_animation_attempt_time) >= LEG_ANIMATION_DELAY_MS)
    {
        for (ArenaPlayer &player : g_players)
        {
            player.Animate();
        }
        last_animation_attempt_time = currentTime;
    }
}

/**
 * @brief Renders Text.
 * @param x Text X coordinate on the screen
 * @param y Text Y coordinate on the screen
 * @param z Text Z coordinate on the screen
 * @param r Red color ranging 0-1
 * @param g Green color ranging 0-1
 * @param b Blue color ranging 0-1
 * @param player Player to print
 */
void RasterChars(GLfloat x, GLfloat y, GLfloat z, const char *text, double r, double g, double b)
{
    // Push to recover original attributes
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    // Draw text in the x, y, z position
    glColor3f(r, g, b);
    glRasterPos3f(x, y, z);
    const char *tmpStr;
    tmpStr = text;
    while (*tmpStr)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *tmpStr);
        tmpStr++;
    }
    glPopAttrib();
}

void PrintText(GLfloat x, GLfloat y, const char *text, double r, double g, double b)
{
    // Draw text considering a 2D space (disable all 3d features)
    glMatrixMode(GL_PROJECTION);
    // Push to recover original PROJECTION MATRIX
    glPushMatrix();
    glLoadIdentity();

    // Peguei o glOrtho do Trabalho anterior
    // Para não me preocupar com a conversão de nada
    glOrtho(
        -(VWidth / 2),  // X coordinate of left edge
        (VWidth / 2),   // X coordinate of right edge
        -(VHeight / 2), //-(VHeight/2)     // Y coordinate of bottom edge
        (VHeight / 2),  // (VHeight/2)     // Y coordinate of top edge
        -1,             // Z coordinate of the “near” plane
        1);             // Z coordinate of the “far” plane
    // glOrtho (0, 1, 0, 1, -1, 1);
    //  Salva e reseta a MODELVIEW para não ter que saber onde a câmera está
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    RasterChars(x, y, 0, text, r, g, b);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Prints players score and which player won on the screen.
 * @param x Text X coordinate on the screen
 * @param y Text Y coordinate on the screen
 * @param player Player to print
 */
void ImprimePlacar(GLfloat x, GLfloat y, int player)
{

    // Cria a string a ser impressa
    double r = 1.0, g = 1.0, b = 1.0;
    if (game_winner == NO_PLAYER)
    {
        sprintf(str, "P%d Health: %d", player, g_players[player - 1].GetHealth());
        if (player == PLAYER1_ID)
        {
            r = 0.0;
            g = 1.0;
            b = 0.0;
        }
        else if (player == PLAYER2_ID)
        {
            r = 1.0;
            g = 0.0;
            b = 0.0;
        }
    }
    else if (game_winner == PLAYER1_WON)
        sprintf(str, "P%d Wins", PLAYER1_ID);
    else if (game_winner == PLAYER2_WON)
        sprintf(str, "P%d Wins", PLAYER2_ID);
    else if (game_winner == DRAW)
        sprintf(str, "Draw");

    // Define a cor e posicao onde vai comecar a imprimir
    PrintText(x, y, str, r, g, b);
    if (camera_spectator_mode)
    {
        PrintText(-VWidth * 0.5, VHeight * 0.30, "X -> RED", 1, 0, 0);
        PrintText(-VWidth * 0.5, VHeight * 0.25, "Y -> GREEN", 0, 1, 0);
        PrintText(-VWidth * 0.5, VHeight * 0.20, "Z -> BLUE", 0, 1, 1);
    }
}

void PrintVectors()
{
    printf("Up : (%.4f, %.4f, %.4f)\n",
           camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]);

    printf("Forward : (%.4f, %.4f, %.4f)\n",
           look_camera_coords[0], look_camera_coords[1], look_camera_coords[2]);

    printf("------------------------------\n");
}

void DrawArenaLights()
{
    GLfloat height = 250.0;

    // Center
    GLfloat light_position_center[] = {0.0, (GLfloat)g_arena.GetPosition().GetY(), height, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position_center);

    // Edges

    // // Y
    // GLfloat light_position_edge1[] = { 0.0, (GLfloat) (g_arena.GetRadius() -g_arena.GetPosition().GetY()), height, 1.0 };
    // glLightfv( GL_LIGHT1, GL_POSITION, light_position_edge1);
    // GLfloat light_position_edge2[] = { 0.0, (GLfloat) -(g_arena.GetRadius() + g_arena.GetPosition().GetY()), height, 1.0 };
    // glLightfv( GL_LIGHT2, GL_POSITION, light_position_edge2);

    // // // X
    // GLfloat light_position_edge3[] = {(GLfloat) g_arena.GetRadius(), (GLfloat) -g_arena.GetPosition().GetY(), height, 1.0 };
    // glLightfv( GL_LIGHT3, GL_POSITION, light_position_edge3);
    // GLfloat light_position_edge4[] = { (GLfloat) -g_arena.GetRadius(), (GLfloat) -g_arena.GetPosition().GetY(), height, 1.0 };
    // glLightfv( GL_LIGHT4, GL_POSITION, light_position_edge4);

    // printf("\n========= LIGHT DEBUG =========\n");
    // printf("Arena Radius : %.4f\n", g_arena.GetRadius());
    // printf("Arena Pos Y  : %.4f\n", g_arena.GetPosition().GetY());
}

/**
 * @brief Renders OpenGL images.
 */
void renderScene(void)
{
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // gluLookAt(0,-400,400, 0,0,0, 0,1,0);
    if (!toggle_player_camera)
    {

        gluLookAt(
            camera_coords[0],
            camera_coords[1],
            camera_coords[2],
            camera_coords[0] + look_camera_coords[0],
            camera_coords[1] + look_camera_coords[1],
            camera_coords[2] + look_camera_coords[2],
            camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]);
    }
    else
    {
        init_player1_camera();
        // Testing Player 1 Camera
        gluLookAt(
            camera_coords[0],
            camera_coords[1],
            camera_coords[2] + PLAYER_HEIGHT,
            camera_coords[0] - g_players[0].GetDirection().GetX(),
            camera_coords[1] + g_players[0].GetDirection().GetY(),
            camera_coords[2] + g_players[0].GetDirection().GetZ() + PLAYER_HEIGHT,
            camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]);

        // g_players[0].GetDirection().PrintAttributes();
        // printf("\n------------ CAMERA DEBUG ------------\n");

        // printf(" camera_coords : (%.4f, %.4f, %.4f)\n", camera_coords[0], camera_coords[1], camera_coords[2]);
        // printf(" player_direction : (%.4f, %.4f, %.4f)\n", g_players[0].GetDirection().GetX(), g_players[0].GetDirection().GetY(), g_players[0].GetDirection().GetZ());
        // printf(" center  : (%.4f, %.4f, %.4f)\n", camera_coords[0] - g_players[0].GetDirection().GetX(), camera_coords[1] + g_players[0].GetDirection().GetY(), g_players[0].GetDirection().GetZ()  + PLAYER_HEIGHT );
        // printf(" up : (%.4f, %.4f, %.4f)\n", camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]);
    }

    // GLfloat light_position[] = { light_x, light_y, light_z, 1.0 };
    // glLightfv(  GL_LIGHT0, GL_POSITION, light_position);

    DrawAxes(100);

    g_arena.DrawArena();

    for (CircularObstacle &obstacle : g_obstacles)
    {
        obstacle.DrawObstacle();
    }
    for (ArenaPlayer &player : g_players)
    {
        if (game_winner == PLAYER1_WON && player.GetId() == PLAYER2_ID)
            continue;
        if (game_winner == PLAYER2_WON && player.GetId() == PLAYER1_ID)
            continue;
        if (game_winner == DRAW)
            break; // Does not draw any player

        player.DrawPlayer();
        for (Bullet &bullet : player.GetBulletVec())
        {
            bullet.DrawBullet();
        }
    }
    if (!(game_finished))
    {
        ImprimePlacar(-VWidth * 0.5, VHeight * 0.45, PLACAR_PLAYER1);
        ImprimePlacar(-VWidth * 0.5, VHeight * 0.40, PLACAR_PLAYER2);
    }
    else
    {
        ImprimePlacar(-VWidth * 0.05, VHeight * 0, game_winner);

        // g_players[0].GetDirection().PrintAttributes();
        // printf("\n------------ CAMERA DEBUG ------------\n");

        // printf(" camera_coords : (%.4f, %.4f, %.4f)\n", camera_coords[0], camera_coords[1], camera_coords[2]);
        // printf(" player_direction : (%.4f, %.4f, %.4f)\n", g_players[0].GetDirection().GetX(), g_players[0].GetDirection().GetY(), g_players[0].GetDirection().GetZ());
        // printf(" center  : (%.4f, %.4f, %.4f)\n", camera_coords[0] - g_players[0].GetDirection().GetX(), camera_coords[1] + g_players[0].GetDirection().GetY(), g_players[0].GetDirection().GetZ()  + PLAYER_HEIGHT );
        // printf(" up : (%.4f, %.4f, %.4f)\n", camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]);
    }

    DrawArenaLights();

    DrawAxes(100);

    g_arena.DrawArena();

    for (CircularObstacle &obstacle : g_obstacles)
    {
        obstacle.DrawObstacle();
    }
    for (ArenaPlayer &player : g_players)
    {
        if (game_winner == PLAYER1_WON && player.GetId() == PLAYER2_ID)
            continue;
        if (game_winner == PLAYER2_WON && player.GetId() == PLAYER1_ID)
            continue;
        if (game_winner == DRAW)
            break; // Does not draw any player

        player.DrawPlayer();
        for (Bullet &bullet : player.GetBulletVec())
        {
            bullet.DrawBullet();
        }
    }
    if (!(game_finished))
    {
        ImprimePlacar(-VWidth * 0.5, VHeight * 0.45, PLACAR_PLAYER1);
        ImprimePlacar(-VWidth * 0.5, VHeight * 0.40, PLACAR_PLAYER2);
    }
    else
        ImprimePlacar(-VWidth * 0.05, VHeight * 0, game_winner);

    // PrintVectors();

    glutSwapBuffers(); // Desenha the new frame of the game.
}

void init_camera_direction_vector(void)
{
    init_arena_camera();

    // camXZAngle = asin( look_camera_coords[2] ) * 180.0/M_PI;
    // camXYAngle   = atan2( look_camera_coords[1],look_camera_coords[0] ) * 180.0/M_PI;

    old_look_camera_coords[0] = look_camera_coords[0];
    old_look_camera_coords[1] = look_camera_coords[1];
    old_look_camera_coords[2] = look_camera_coords[2];

    camera_up_vec[0] = 0.0;
    camera_up_vec[1] = 0.0;
    camera_up_vec[2] = 1.0;
}

/**
 * @brief Initial game config.
 */
void init_game(void)
{
    // Record Last Positions before start
    // Setting Up Character to look at each other

    // printf("Reiniciei o game\n");
    game_winner = NO_PLAYER;
    if (first_start)
    {
        ArenaPlayer &p1 = g_players[0];
        ArenaPlayer &p2 = g_players[1];
        double dtheta = atan2(
            p1.GetPosition().GetY() - p2.GetPosition().GetY(),
            p1.GetPosition().GetX() - p2.GetPosition().GetX());
        dtheta = (dtheta / RADIANS);

        p1.GetOrientation().SetYaw(-90 - dtheta + 180);
        p2.GetOrientation().SetYaw(-90 - dtheta);

        p1.Rotate(0); // Updates Direction Vector
        p2.Rotate(0); // Updates Direction Vector

        initial_players_angle.push_back(p1.GetOrientation().GetYaw());
        initial_players_angle.push_back(p2.GetOrientation().GetYaw());
    }

    last_players_recorded_pos.clear();
    last_animation_attempt_time = glutGet(GLUT_ELAPSED_TIME);

    for (unsigned int i = 0; i < g_players.size(); i++)
    {
        ArenaPlayer &player = g_players[i];

        if (game_finished)
        {
            player.GetBulletVec().clear();   // Clear Bullets
            player.SetHealth(PLAYER_HEALTH); // puts start Health
            if (initial_players_pos.size())  // puts in the start Position
            {
                player.GetPosition().SetX(initial_players_pos[i].GetX());
                player.GetPosition().SetY(initial_players_pos[i].GetY());
                player.GetPosition().SetZ(initial_players_pos[i].GetZ());
                player.SetLastPosition(initial_players_pos[i]);
            }
            player.GetOrientation().SetYaw(initial_players_angle[i]); // puts in the start Yaw
            player.Rotate(0);                                         // Updates Direction vector
            player.SetGunYaw(0.0);                                    // puts in the start gun yaw
            player.SetGunPitch(0.0);
            player.SetMovingStatus(false);
            player.SetLastAnimationAttemptPosition(player.GetPosition());
        }
        if (first_start)
        {
            initial_players_pos.push_back(player.GetPosition());
        }

        last_players_recorded_pos.push_back(player.GetPosition());
    }
    if (first_start)
        first_start = false; // first setup

    init_camera_direction_vector();
}

/**
 * @brief Gets Key Pressed.
 * @param key key pressed
 */
void keyPress(unsigned char key, int x, int y)
{
    // printf("Key : n:%d c:%c\n",key,key);
    switch (key)
    {
    case 'y':
    case 'Y':
        camera_spectator_mode = !camera_spectator_mode;
        break;

    case 'h':
    case 'H':
        toggle_player_camera = !toggle_player_camera;
        if (toggle_player_camera)
            init_player1_camera();
        else
            init_arena_camera();
        break;

    case 'n':
    case 'N':
        toggle_light = !toggle_light;
        if (toggle_light)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
        break;

    //------------------Player 1------------------//
    case 'w':
    case 'W':
        keyStatus[(int)('w')] = 1; // Using keyStatus trick
        break;
    case 's':
    case 'S':
        keyStatus[(int)('s')] = 1; // Using keyStatus trick
        break;
    case 'a':
    case 'A':
        keyStatus[(int)('a')] = 1; // Using keyStatus trick
        break;
    case 'd':
    case 'D':
        keyStatus[(int)('d')] = 1; // Using keyStatus trick
        break;

    case SPACE_BAR: // Barra de espaço
        keyStatus[SPACE_BAR] = 1;
        break;

    //------------------Player 2------------------//
    case 'o':
    case 'O':
        keyStatus[(int)('o')] = 1; // Using keyStatus trick
        break;
    case 'l':
    case 'L':
        keyStatus[(int)('l')] = 1; // Using keyStatus trick
        break;
    case 'k':
    case 'K':
        keyStatus[(int)('k')] = 1; // Using keyStatus trick
        break;
    case SPECIAL_KEY:
    case CAPS_SPECIAL_KEY:
        keyStatus[(int)(SPECIAL_KEY)] = 1; // Using keyStatus trick
        break;
    case '2':
        keyStatus[(int)('2')] = 1; // Using keyStatus trick
        break;
    case '5':
        keyStatus[(int)('5')] = 1; // Using keyStatus trick
        break;
    case '4':
        keyStatus[(int)('4')] = 1; // Using keyStatus trick
        break;
    case '6':
        keyStatus[(int)('6')] = 1; // Using keyStatus trick
        break;
    case '8':
        keyStatus[(int)('8')] = 1; // Using keyStatus trick
        break;
    case '.':
        keyStatus[(int)('.')] = 1; // Using keyStatus trick
        break;

    case 'c':
    case 'C':
        g_debugCompareModel = !g_debugCompareModel;
        printf("!compare = %d\n", g_debugCompareModel);
        break;

    case 'z': // aumenta escala
    case 'Z':
        g_modelScale *= 1.05f;
        printf("g_modelScale = %.4f\n", g_modelScale);
        break;

    case 'x': // diminui escala
    case 'X':
        g_modelScale /= 1.05f;
        printf("g_modelScale = %.4f\n", g_modelScale);
        break;

    //------------------Game------------------//
    case 'r':
    case 'R':
        init_game();
        game_finished = false; // Using keyStatus trick
        break;

    case ESC_KEY:
        exit(0);
    }
    glutPostRedisplay();
}

/**
 * @brief Gets Key Status
 * @param key key pressed
 */
void keyup(unsigned char key, int x, int y)
{
    keyStatus[(int)(key)] = 0;
    glutPostRedisplay();
}

/**
 * @brief Reset Key Status
 */
void ResetKeyStatus(void)
{
    int i;
    // Initialize keyStatus
    for (i = 0; i < 256; i++)
        keyStatus[i] = 0;
}

void idle(void)
{
    static GLdouble previousTime = glutGet(GLUT_ELAPSED_TIME);
    GLdouble currentTime, timeDiference;

    // Pega o tempo que passou do inicio da aplicacao
    currentTime = glutGet(GLUT_ELAPSED_TIME);

    // Calcula o tempo decorrido desde de a ultima frame.
    timeDiference = currentTime - previousTime;

    // Atualiza o tempo do ultimo frame ocorrido
    previousTime = currentTime;

    if (!game_finished)
    {

        if (g_players[0].GetHealth() == 0 || g_players[1].GetHealth() == 0)
        {
            if (g_players[0].GetHealth() == 0 && g_players[1].GetHealth() == 0)
                game_winner = DRAW;
            else if (g_players[0].GetHealth() == 0)
                game_winner = PLAYER2_WON;
            else if (g_players[1].GetHealth() == 0)
                game_winner = PLAYER1_WON;
            game_finished = true;
        }
        AnimatePlayers(currentTime);
        if (!camera_spectator_mode)
            Player1_Keys(timeDiference * TIME_S, currentTime);
        else
            SpectatorMode_Keys(timeDiference * TIME_S);
        Player2_Keys(timeDiference * TIME_S, currentTime);
        g_players[0].UpdateSoldadoAnim(timeDiference, keyStatus['w'] || keyStatus['s']);
        g_players[1].UpdateSoldadoAnim(timeDiference, keyStatus['o'] || keyStatus['l']);
        Player1_Bullets(timeDiference * TIME_S);
        Player2_Bullets(timeDiference * TIME_S);

        glutPostRedisplay();
    }

    gPastMouseX = gCurrentMouseX;
    gPastMouseY = gCurrentMouseY;
}

void gl_init(void)
{
    ResetKeyStatus();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    // glShadeModel (GL_FLAT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_LIGHT4);

    glEnable(GL_DEPTH_TEST);
    glViewport(
        0,
        0,
        (GLsizei)Width,
        (GLsizei)Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        70,
        1, //(GLfloat)windowSize/(GLfloat)windowSize,
        10,
        800);

    // // The color the windows will redraw. Its done to erase the previous frame.
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black, no opacity(alpha).

    // glMatrixMode(GL_PROJECTION); // Select the projection matrix
    // glOrtho(-(VWidth/2),     // X coordinate of left edge
    //         (VWidth/2),     // X coordinate of right edge
    //         -(VHeight/2), //-(VHeight/2)     // Y coordinate of bottom edge
    //         (VHeight/2),// (VHeight/2)     // Y coordinate of top edge
    //         -100,     // Z coordinate of the “near” plane
    //         100);    // Z coordinate of the “far” plane
    // glMatrixMode(GL_MODELVIEW); // Select the projection matrix
    // glLoadIdentity();
    toggle_light = !toggle_light;
    toggle_texture = !toggle_texture;
}

/**
 * @brief Loads Arema
 * @param path path to the arena file
 */
int load_svg(const char *path)
{
    // Get Circles
    std::vector<CircleDefinition> circle_vec = svg_parser(path);

    if (circle_vec.size() == 0)
    {
        return 3;
    }

    // Get Arena, Obstacles and Players
    std::optional<CircularArena> arena = arena_getter(circle_vec);
    if (!(arena))
    {
        printf("Arena nao carregada corretamente!\n");
        return 4;
    }

    std::optional<std::vector<CircularObstacle>> obstacles = obstacles_getter(circle_vec, *arena);
    std::optional<std::vector<ArenaPlayer>> players = players_getter(circle_vec, *arena);
    if (!(players))
    {
        printf("Jogadores nao carregados corretamente!\n");
        return 5;
    }
    if (!(obstacles))
    {
        printf("Obstaculos nao carregados corretamente!\n");
        return 6;
    }

    g_arena = std::move(*arena);
    g_arena.GetPosition().SetX(0.0); // Arena beeing draw in the center of the axis
    g_arena.GetPosition().SetY(0.0); // Arena beeing draw in the center of the axis

    g_players = std::move(*players);
    g_obstacles = std::move(*obstacles);

    if (debug)
    {
        printf("Arena: \n");
        g_arena.PrintAttributes();
        printf("\n\n");

        int i = 0;
        for (ArenaPlayer &p : g_players)
        {
            printf("Player %d: \n", (i++) + 1);
            p.PrintAttributes();
            printf("\n\n");
        }

        i = 0;
        for (CircularObstacle &obs : g_obstacles)
        {
            printf("Obstacle %d: \n", (i++) + 1);
            obs.PrintAttributes();
            printf("\n\n");
        }

        g_arena.GetRGB().PrintAttributes();
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // Parsing SVG

    if (argc < 2)
    {
        printf("Insira o caminho para o SVG!\n");
        return 1;
    }
    if (argc > 2)
    {
        printf("Apenas insira o caminho para o SVG!\n");
        return 2;
    }

    int result = load_svg(argv[1]);
    if (result)
        return result;

    // Initialize openGL with Double buffer and RGB color without transparency.
    // Its interesting to try GLUT_SINGLE instead of GLUT_DOUBLE.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create the window.
    glutInitWindowSize(Width, Height);
    glutInitWindowPosition(150, 50);
    glutCreateWindow("Joguin 3D");

    // Viewing Height=Width = Arena Diamater
    VHeight = VWidth = 2 * g_arena.GetRadius();

    // Define callbacks.
    glutDisplayFunc(renderScene);
    glutKeyboardFunc(keyPress);
    glutIdleFunc(idle);
    glutKeyboardUpFunc(keyup);
    // glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseClick);
    glutMotionFunc(globalmouseMotion);
    glutPassiveMotionFunc(globalmouseMotion);

    g_movIdle = g_soldado.loadMeshAnim("Blender/idle/soldado_idle_####.obj", 64);
    g_movWalking = g_soldado.loadMeshAnim("Blender/walking/soldado_walking_####.obj", 41);
    g_soldado.loadTexture("Blender/textura_soldado.bmp");
    g_soldado.drawInit(g_movIdle);

    movArma = g_arma.loadMeshAnim("Blender/model_arma_####.obj", 1);
    g_arma.loadTexture("Blender/textura_arma.bmp");
    g_arma.drawInit(movArma);
    init_game();
    gl_init();
    glutMainLoop();

    return 0;
}
