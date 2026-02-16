#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "utils.h"
#include "arena_parser.h"
#include "arena.h"
#include "arena_obstacles.h"
#include "player.h"
#include "bullet.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <math.h>

#include <stdlib.h>

#define INC_KEY 10 

#define SPECIAL_KEY 231 // isso é para o 'ç'
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
#define MOUSE_SENSITIVY 1.0

// debug
int debug = 0;

//Key status
int keyStatus[256];

// Window dimensions
const GLint Width = 800;
const GLint Height = 700;
const GLdouble start_angle = 70;

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

std::vector<GLdouble> last_time_player_shoot = {0.0,0.0};

//podia fazem um enum mas preguiça ...

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
void * font = GLUT_BITMAP_9_BY_15;

//Mouse
float gCurrentMouseX = 0;
float gCurrentMouseY = 0;
float gPastMouseX = 0;
float gPastMouseY = 0;

int LeftMouseButton = -1;
int LeftMouseState = -1;
int RightMouseButton = -1;
int RightMouseState = -1;

// Camera movement
float camera_coords[3] = {0.0,0.0,0.0};
float spectator_camera_coords[3] = {0.0,-400.0,400.0};
float look_spectator_camera_coords[3] = {0.0,0.0,0.0};
float old_look_spectator_camera_coords[3] = {0.0,0.0,0.0};
float camera_up_vec[3] = {0.0,0.0,0.0};
float camSpectatorXZAngle = 0;
float camSpectatorXYAngle = 0;
float camThirdPersonPlayer1XZAngle = 0;
float camThirdPersonPlayer1XYAngle = 0;
float camThirdPersonPlayer2XZAngle = 0;
float camThirdPersonPlayer2XYAngle = 0;



// Camera Modes
#define EYE_CAMERA 0
#define GUN_CAMERA 1
#define THIRD_PERSON_CAMERA 2
#define SPECTATOR_CAMERA 3
#define CAMERA_THIRD_PERSON_DISTANCE 30
#define SPECTATOR_CAMERA_SPEED 100
bool camera_spectator_mode = false;
bool toggle_player_camera = true;
short last_camera_type_num = GUN_CAMERA; // Used because of the Spectator Mode
short camera_type_num = GUN_CAMERA;

//Disable Features
bool toggle_light = false;
bool toggle_texture = false;
bool night_mode = false;

// Enable Multi Viewport
bool multiviewport = false;


/**
 * @brief Changes camera configs.
 */
void changeCamera(int angle, int w, int h, GLdouble near = 10, GLdouble far = 800)
{
    glMatrixMode (GL_PROJECTION);

    glLoadIdentity ();

    gluPerspective (angle, 
            (GLfloat)w / (GLfloat)h, near, far);

    glMatrixMode (GL_MODELVIEW);
}



/**
 * @brief Changes camera configs.
*/
void changeCameraType(int camera_type_num)
{

    if (camera_type_num == EYE_CAMERA) changeCamera(start_angle,glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    else if (camera_type_num == GUN_CAMERA) changeCamera(90,glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),1,g_arena.GetRadius()*2);
    else if (camera_type_num == THIRD_PERSON_CAMERA) changeCamera(90,glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),CAMERA_THIRD_PERSON_DISTANCE,300);
    else if (camera_type_num == SPECTATOR_CAMERA) changeCamera(start_angle,glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}



/**
 * @brief Starts Player Eye Camera Params.
 */
void init_player_eye_camera_coords(int player_id)
{
    camera_coords[0] = g_players[player_id-1].GetPosition().GetX(); 
    camera_coords[1] = -g_players[player_id-1].GetPosition().GetY();
    camera_coords[2] = g_players[player_id-1].GetPosition().GetZ() + PLAYER_HEIGHT;
}



/**
 * @brief Starts Player Gun Camera Params.
 */
void init_player_gun_camera_coords(int player_id)
{
    camera_coords[0] = g_players[player_id-1].GetPosition().GetX() - g_players[player_id-1].GetRadius() * -g_players[player_id-1].GetDirection().GetY(); 
    camera_coords[1] = -g_players[player_id-1].GetPosition().GetY() + g_players[player_id-1].GetRadius() * g_players[player_id-1].GetDirection().GetX(); //- g_players[0].GetRadius()* g_players[0].GetDirection().GetY();
    camera_coords[2] = g_players[player_id-1].GetPosition().GetZ() + PLAYER_HEIGHT;
}



/**
 * @brief Starts Player Third Person Camera Params.
 */
void init_player_third_person_camera_coords(int player_id)
{
    camera_coords[0] = g_players[player_id-1].GetPosition().GetX(); 
    camera_coords[1] = -g_players[player_id-1].GetPosition().GetY(); //- g_players[0].GetRadius()* g_players[0].GetDirection().GetY();
    camera_coords[2] = g_players[player_id-1].GetPosition().GetZ() + PLAYER_HEIGHT;
}



/**
 * @brief Starts Spectator Camera Params.
 */
void init_spectator_camera_coords(void)
{
    camera_coords[0] = spectator_camera_coords[0]; 
    camera_coords[1] = spectator_camera_coords[1];//-g_arena.GetRadius()*2.5; -400,400
    camera_coords[2] = spectator_camera_coords[2];//g_arena.GetRadius()*1.5;
}



/**
 * @brief Init Spectator Camera Direction Vector
 */

void init_spectator_camera_direction_vector(void)
{
    init_spectator_camera_coords();

    old_look_spectator_camera_coords[0] = look_spectator_camera_coords[0];
    old_look_spectator_camera_coords[1] = look_spectator_camera_coords[1];
    old_look_spectator_camera_coords[2] = look_spectator_camera_coords[2];

    camera_up_vec[0] = 0.0;
    camera_up_vec[1] = 0.0;
    camera_up_vec[2] = 1.0;

    float center_start[3] = {0.0,0.0,0.0};
    vector_3d_difference(center_start,camera_coords,look_spectator_camera_coords);
    normalize_3d(look_spectator_camera_coords);

    camSpectatorXYAngle = asin( look_spectator_camera_coords[1] ) * 180.0/M_PI;
    camSpectatorXZAngle = atan2( look_spectator_camera_coords[0], -look_spectator_camera_coords[2] ) * 180.0/M_PI;

}



/**
 * @brief Init Spectator Camera Direction Vector
 */

void init_third_person_camera_direction_vector(void)
{
    init_player_third_person_camera_coords(PLAYER1_ID);
    init_player_third_person_camera_coords(PLAYER2_ID);

    // look_spectator_camera_coords[0] = -cos(camSpectatorXYAngle * M_PI/180.0) * sin(camSpectatorXZAngle * M_PI/180.0);
    // look_spectator_camera_coords[1] = sin(camSpectatorXYAngle * M_PI/180.0);
    // look_spectator_camera_coords[2] = -cos(camSpectatorXYAngle * M_PI/180.0) * cos(camSpectatorXZAngle * M_PI/180.0);

    // camera_coords[0] + CAMERA_THIRD_PERSON_DISTANCE *  g_players[player_id-1].GetDirection().GetX() * sin((camThirdPersonXYAngle*M_PI/180)),
    // camera_coords[1] - CAMERA_THIRD_PERSON_DISTANCE * (g_players[player_id-1].GetDirection().GetY()) * sin(camThirdPersonXZAngle*M_PI/180)*cos((camThirdPersonXYAngle*M_PI/180)), 
    // camera_coords[2] + PLAYER_HEIGHT + CAMERA_THIRD_PERSON_DISTANCE *cos(camThirdPersonXZAngle*M_PI/180)*cos((camThirdPersonXYAngle*M_PI/180)),

    camThirdPersonPlayer1XYAngle = asin( g_players[PLAYER1_ID-1].GetDirection().GetX() ) * 180.0/M_PI;
    camThirdPersonPlayer1XZAngle = atan2( -(g_players[PLAYER1_ID-1].GetDirection().GetY()),PLAYER_HEIGHT) * 180.0/M_PI;

    camThirdPersonPlayer2XYAngle = asin( g_players[PLAYER1_ID-1].GetDirection().GetX()) * 180.0/M_PI;
    camThirdPersonPlayer2XZAngle = atan2(-(g_players[PLAYER1_ID-1].GetDirection().GetY()), PLAYER_HEIGHT) * 180.0/M_PI;

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
    gCurrentMouseX = x ;
    gCurrentMouseY = -y;

    // Podia tirar isso daqui ...
    if (RightMouseButton == RIGHT_CLICK && RightMouseState == MOUSE_PRESSED &&  camera_spectator_mode)
    {
        
        camSpectatorXZAngle -= (gCurrentMouseX - gPastMouseX) * MOUSE_SENSITIVY/2;
        camSpectatorXYAngle += (gCurrentMouseY - gPastMouseY) * MOUSE_SENSITIVY/2;

        look_spectator_camera_coords[0] = -cos(camSpectatorXYAngle * M_PI/180.0) * sin(camSpectatorXZAngle * M_PI/180.0);
        look_spectator_camera_coords[1] = sin(camSpectatorXYAngle * M_PI/180.0);
        look_spectator_camera_coords[2] = -cos(camSpectatorXYAngle * M_PI/180.0) * cos(camSpectatorXZAngle * M_PI/180.0);

        // printf("Look Vector -> X: %.4f  Y: %.4f  Z: %.4f\n",
        // look_spectator_camera_coords[0],
        // look_spectator_camera_coords[1],
        // look_spectator_camera_coords[2]);


        float anti_up_look[3] = {0.0,0.0,0.0};
        dot_product_3d(look_spectator_camera_coords,camera_up_vec,anti_up_look);
        if ( (anti_up_look[0]*anti_up_look[0] + 
              anti_up_look[1]*anti_up_look[1] +
              anti_up_look[2]*anti_up_look[2]) == 0)
        {
            look_spectator_camera_coords[0] = old_look_spectator_camera_coords[0];
            look_spectator_camera_coords[1] = old_look_spectator_camera_coords[1];
            look_spectator_camera_coords[2] = old_look_spectator_camera_coords[2];
        }
        else
        {
            old_look_spectator_camera_coords[0] = look_spectator_camera_coords[0];
            old_look_spectator_camera_coords[1] = look_spectator_camera_coords[1];
            old_look_spectator_camera_coords[2] = look_spectator_camera_coords[2];
        }
    }
    if (RightMouseButton == RIGHT_CLICK && RightMouseState == MOUSE_PRESSED && camera_type_num == THIRD_PERSON_CAMERA)
    {
        camThirdPersonPlayer1XZAngle -= (gCurrentMouseX - gPastMouseX) * MOUSE_SENSITIVY/2;
        camThirdPersonPlayer1XYAngle += (gCurrentMouseY - gPastMouseY) * MOUSE_SENSITIVY/2;
        camThirdPersonPlayer2XZAngle -= (gCurrentMouseX - gPastMouseX) * MOUSE_SENSITIVY/2;
        camThirdPersonPlayer2XYAngle += (gCurrentMouseY - gPastMouseY) * MOUSE_SENSITIVY/2;
    }

    glutPostRedisplay();
}



/**
 * @brief Get Mouse button and its state.
 * @param button button number
 * @param state state=0 -> Released || state=1-> Pressed
 * @param x Mouse X Coordinates
 * @param y Mouse Y Coordinates
 */
void mouseClick(int button, int state, int x,int y)
{
    if (button == GLUT_LEFT_BUTTON &&  state == GLUT_DOWN)
    {
        LeftMouseButton = LEFT_CLICK;
        LeftMouseState = MOUSE_PRESSED;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
    {
        LeftMouseButton = -1;
        LeftMouseState = MOUSE_RELEASED;
    }
    if (button == GLUT_RIGHT_BUTTON &&  state == GLUT_DOWN)
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
    std::vector<Bullet>& bullet_vec = g_players[1].GetBulletVec();
    for (unsigned int i=0; i<bullet_vec.size(); i++ )
    {
        Bullet& bullet = bullet_vec[i];
        if (!bullet.Move(g_arena,g_obstacles,g_players,timeDiference))
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
    std::vector<Bullet>& bullet_vec = g_players[0].GetBulletVec();
    for (unsigned int i=0; i<bullet_vec.size(); i++ )
    {
        Bullet& bullet = bullet_vec[i];
        if (!bullet.Move(g_arena,g_obstacles,g_players,timeDiference))
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
    //Treat keyPress
    ArenaPlayer& p2 = g_players[1];
    if(keyStatus[(int)('o')])
    {
        p2.Move(g_arena,g_obstacles,g_players,timeDiference);
    }
    if(keyStatus[(int)('l')])
    {
        p2.Move(g_arena,g_obstacles,g_players,-timeDiference);
    }
    if(keyStatus[(int)('k')])
    {
        if (!keyStatus[(int)'.'] && !p2.GetJumpDecay())
        {
            p2.Rotate(timeDiference);
        }
    }
    if(keyStatus[SPECIAL_KEY])
    {
        if (!keyStatus[(int)'.'] && !p2.GetJumpDecay())
        {
            p2.Rotate(-timeDiference);
        }
    }
    if (keyStatus[(int)('5')] && 
        (currentTime-last_time_player_shoot[1]) > WEAPON_FIRERATE
    )
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
    if  (keyStatus[(int)('8')])
    {
        p2.RotateGunRoll(timeDiference * MOUSE_SENSITIVY);
    }


    p2.IncreaseHeight(timeDiference,keyStatus[(int)'.']);

    // Queria fazer genérico, mas vai ficar hardcoded como jogador 2 
    // Porque estamos sem tempo
    ArenaPlayer p1 = g_players[0];
    p2.DecreaseHeight(timeDiference,p1);
    p2.UpdateDecayType(g_arena,g_obstacles,g_players);
}



/**
 * @brief Updates Player1 based on pressed Keys.
 * @param timeDiference diferença de tempo milissegundo entre duas execuções da mesma função
 * @param currentTime Tempo decorrido desde o inicio do jogo em milissegundo
 */
void Player1_Keys(GLdouble timeDiference, GLdouble currentTime)
{   
    ArenaPlayer& p1 = g_players[0];
    //Treat keyPress
    if(keyStatus[(int)('w')])
    {
        // printf("Jogador Andando\n");
        p1.Move(g_arena,g_obstacles,g_players,timeDiference);
    }
    if(keyStatus[(int)('s')])
    {
        p1.Move(g_arena,g_obstacles,g_players,-timeDiference);
    }
    if(keyStatus[(int)('a')])
    {
        if (!keyStatus[(int)('x')] && !p1.GetJumpDecay())
        {
            p1.Rotate(timeDiference);
        }
    }
    if(keyStatus[(int)('d')])
    {
        if (!keyStatus[(int)('x')] && !p1.GetJumpDecay())
        {
            p1.Rotate(-timeDiference);
        }
    }
    if (LeftMouseButton == LEFT_CLICK && LeftMouseState == MOUSE_PRESSED && 
        (currentTime-last_time_player_shoot[0]) > WEAPON_FIRERATE
    )
    {
        p1.Shoot();
        last_time_player_shoot[0] = currentTime;
    }

    // Movimento é Horizontal, quis fazer um movimento adaptativo em relação
    // à orientação do personagem, mas vou deixar isso parado por hora

    // printf("Current Mouse X %.2f\n",gCurrentMouseX);
    // printf("Past Mouse X %.2f\n",gPastMouseX);
    double mouse_x_angle = gCurrentMouseX-gPastMouseX;
    if ( mouse_x_angle < 0)
    {
        p1.RotateGunYaw(timeDiference * MOUSE_SENSITIVY);
    }
    if  ( mouse_x_angle > 0)
    {
        p1.RotateGunYaw(-timeDiference * MOUSE_SENSITIVY);
    }

    double mouse_y_angle = gCurrentMouseY-gPastMouseY;
    if ( mouse_y_angle < 0)
    {
        p1.RotateGunRoll(-timeDiference * MOUSE_SENSITIVY);
    }
    if  ( mouse_y_angle > 0)
    {
        p1.RotateGunRoll(timeDiference * MOUSE_SENSITIVY);
    }

    p1.UpdateDecayType(g_arena,g_obstacles,g_players);
    p1.IncreaseHeight(timeDiference,keyStatus[(int)('x')]);

    // Queria fazer genérico, mas vai ficar hardcoded como jogador 2 
    // Porque estamos sem tempo
    ArenaPlayer p2 = g_players[1];
    p1.DecreaseHeight(timeDiference,p2);
}



/**
 * @brief Updates Spectator Camera based on pressed Keys.
 * @param timeDiference diferença de tempo milissegundo entre duas execuções da mesma função
 * @param currentTime Tempo decorrido desde o inicio do jogo em milissegundo
 */
void SpectatorMode_Keys(double deltaTime)
{
    // velocidade da câmera
    double camera_speed = SPECTATOR_CAMERA_SPEED * deltaTime;

    if(keyStatus['w']) 
    {
        spectator_camera_coords[0] +=  look_spectator_camera_coords[0] * camera_speed;
        spectator_camera_coords[1] +=  look_spectator_camera_coords[1] * camera_speed;
        spectator_camera_coords[2] +=  look_spectator_camera_coords[2] * camera_speed;
    }
    if(keyStatus['s'])
    {
        spectator_camera_coords[0] -=  look_spectator_camera_coords[0] * camera_speed;
        spectator_camera_coords[1] -=  look_spectator_camera_coords[1] * camera_speed;
        spectator_camera_coords[2] -=  look_spectator_camera_coords[2] * camera_speed;
    }
}



/**
 * @brief Anima todos os jogadores presentes.
 * @param currentTime Tempo decorrido desde o inicio do jogo em milissegundo
 */
void AnimatePlayers(GLdouble currentTime)
{
    if ((currentTime-last_animation_attempt_time) >= LEG_ANIMATION_DELAY_MS)
    {
        for ( ArenaPlayer& player: g_players)
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
void RasterChars(GLfloat x, GLfloat y, GLfloat z, const char * text, double r, double g, double b)
{
    //Push to recover original attributes
    glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        //Draw text in the x, y, z position
        glColor3f(r,g,b);
        glRasterPos3f(x, y, z);
        const char* tmpStr;
        tmpStr = text;
        while( *tmpStr )
        {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *tmpStr);
            tmpStr++;
        }
    glPopAttrib();
}



/**
 * @brief Prints 2D text on the screen.
 * @param x Text X coordinate on the screen
 * @param y Text Y coordinate on the screen
 * @param player Player to print
 */
void PrintText(GLfloat x, GLfloat y, const char * text, double r, double g, double b)
{
    //Draw text considering a 2D space (disable all 3d features)
    glMatrixMode (GL_PROJECTION);
    //Push to recover original PROJECTION MATRIX
    glPushMatrix();
        glLoadIdentity();

        // Peguei o glOrtho do Trabalho anterior
        // Para não me preocupar com a conversão de nada
        glOrtho(
            -(VWidth/2),     // X coordinate of left edge             
            (VWidth/2),     // X coordinate of right edge            
            -(VHeight/2), //-(VHeight/2)     // Y coordinate of bottom edge            
            (VHeight/2),// (VHeight/2)     // Y coordinate of top edge
            -1,     // Z coordinate of the “near” plane            
            1
        );    // Z coordinate of the “far” plane
        //glOrtho (0, 1, 0, 1, -1, 1);
        // Salva e reseta a MODELVIEW para não ter que saber onde a câmera está
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            RasterChars(x, y, 0, text, r, g, b);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);

    glPopMatrix();
    glMatrixMode (GL_MODELVIEW);
}



/**
 * @brief Prints players score and which player won on the screen.
 * @param x Text X coordinate on the screen
 * @param y Text Y coordinate on the screen
 * @param player Player to print
 */
void ImprimePlacar(GLfloat x, GLfloat y, int player)
{

    //Cria a string a ser impressa
    double r = 1.0, g = 1.0, b = 1.0; 
    if (game_winner == NO_PLAYER)
    {
        sprintf(str, "P%d Health: %d",player, g_players[player-1].GetHealth());
        if (player == PLAYER1_ID)
        {
            r = 0.0; g = 1.0; b = 0.0;
        }
        else if (player == PLAYER2_ID)
        {
            r = 1.0; g = 0.0; b = 0.0;
        }
    }
    else if (game_winner == PLAYER1_WON) sprintf(str, "P%d Wins",PLAYER1_ID);
    else if (game_winner == PLAYER2_WON) sprintf(str, "P%d Wins",PLAYER2_ID);
    else if (game_winner == DRAW) sprintf(str, "Draw");

    if (game_winner != NO_PLAYER && camera_type_num == THIRD_PERSON_CAMERA)
    {
        // Como o Far plane fica cortado o texto acaba nao aparecendo
        // isso aqui é uma gambiarra mas funciona kk
        changeCamera(90,glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),1,350);
        camera_type_num = GUN_CAMERA;
        glutPostRedisplay();
    }

    //Define a cor e posicao onde vai comecar a imprimir
    PrintText(x, y, str, r,g,b); 
    if (camera_spectator_mode)
    {
        PrintText(-VWidth*0.5,VHeight*0.30, "X -> RED", 1,0,0);
        PrintText(-VWidth*0.5,VHeight*0.25, "Y -> GREEN", 0,1,0);
        PrintText(-VWidth*0.5,VHeight*0.20, "Z -> BLUE", 0,1,1);
    }   
}



/**
 * @brief Renders Arena Lights.
 */
void DrawArenaLights()
{
    GLfloat height = 250.0;
    
    GLfloat light_center[] = { 0.0, (GLfloat) g_arena.GetPosition().GetY(), height, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, light_center);
    // Center
    if(night_mode)
    {
        GLfloat light_center_light[] = { 0.01, 0.01, 0.01, 1.0 };
        glLightfv( GL_LIGHT0, GL_DIFFUSE, light_center_light); 
        glLightfv( GL_LIGHT0, GL_AMBIENT, light_center_light);
        glLightfv( GL_LIGHT0, GL_SPECULAR, light_center_light);
    }
    else
    {
        GLfloat light_center_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
        glLightfv( GL_LIGHT0, GL_DIFFUSE, light_center_diffuse);
        GLfloat light_center_ambient[] = { 0.25, 0.25, 0.25, 1.0};
        glLightfv( GL_LIGHT0, GL_AMBIENT, light_center_ambient); 
        GLfloat light_center_specular[] = { 1.0, 1.0, 1.0, 1.0};
        glLightfv( GL_LIGHT0, GL_SPECULAR, light_center_specular);      
    }


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
void renderScene(int player_id, bool fixed_camera = false, short camera_num = -1)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluLookAt(0,-400,400, 0,0,0, 0,1,0);
    if (!fixed_camera)
    {
        if (!toggle_player_camera)
        {
            init_spectator_camera_coords();
            gluLookAt(
                camera_coords[0],
                camera_coords[1],
                camera_coords[2],
                camera_coords[0] + look_spectator_camera_coords[0],
                camera_coords[1] + look_spectator_camera_coords[1],
                camera_coords[2] + look_spectator_camera_coords[2],
                camera_up_vec[0],camera_up_vec[1],camera_up_vec[2]
            );
        }
        else
        {
            if (camera_type_num == EYE_CAMERA)
            {
                init_player_eye_camera_coords(player_id);
                // Testing Player 1 Camera
                gluLookAt(
                    camera_coords[0],
                    camera_coords[1],
                    camera_coords[2] + PLAYER_HEIGHT,
                    camera_coords[0] - g_players[player_id-1].GetDirection().GetX(), 
                    camera_coords[1] + g_players[player_id-1].GetDirection().GetY(),
                    camera_coords[2] + g_players[player_id-1].GetDirection().GetZ() + PLAYER_HEIGHT ,
                    camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]
                );
            }
            else if (camera_type_num == GUN_CAMERA)
            {
                init_player_gun_camera_coords(player_id);
                // Testing Player 1 Camera
                gluLookAt(
                    camera_coords[0],
                    camera_coords[1],
                    camera_coords[2] + PLAYER_HEIGHT,
                    camera_coords[0] - g_players[player_id-1].GetDirection().GetX(), 
                    camera_coords[1] + g_players[player_id-1].GetDirection().GetY(),
                    camera_coords[2] + PLAYER_HEIGHT,
                    camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]
                );    
            }
            else if (camera_type_num == THIRD_PERSON_CAMERA)
            {
                float camThirdPersonXYAngle = 0;
                float camThirdPersonXZAngle = 0;

                if (player_id == PLAYER1_ID)
                {
                    camThirdPersonXYAngle = camThirdPersonPlayer1XYAngle;
                    camThirdPersonXZAngle = camThirdPersonPlayer1XZAngle;
                }
                else if ( player_id == PLAYER2_ID )
                {
                    camThirdPersonXYAngle = camThirdPersonPlayer2XYAngle;
                    camThirdPersonXZAngle = camThirdPersonPlayer2XZAngle;  
                }
                init_player_third_person_camera_coords(player_id);
                // Testing Player 1 Camera

                // zoom*sin(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180)),
                //     zoom*                         sin((camXYAngle*M_PI/180)),
                //     zoom*cos(camXZAngle*M_PI/180)*cos((camXYAngle*M_PI/180)),
                gluLookAt(
                    camera_coords[0] + CAMERA_THIRD_PERSON_DISTANCE *  g_players[player_id-1].GetDirection().GetX() * sin(camThirdPersonXZAngle*M_PI/180)*cos((camThirdPersonXYAngle*M_PI/180)),
                    camera_coords[1] - CAMERA_THIRD_PERSON_DISTANCE * (g_players[player_id-1].GetDirection().GetY()) * sin((camThirdPersonXYAngle*M_PI/180)), 
                    camera_coords[2] + PLAYER_HEIGHT + CAMERA_THIRD_PERSON_DISTANCE *cos(camThirdPersonXZAngle*M_PI/180)*cos((camThirdPersonXYAngle*M_PI/180)),
                    camera_coords[0], 
                    camera_coords[1],
                    camera_coords[2],
                    camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]
                );       
            }

            // g_players[0].GetDirection().PrintAttributes();
            // printf("\n------------ CAMERA DEBUG ------------\n");

            // printf(" camera_coords : (%.4f, %.4f, %.4f)\n", camera_coords[0], camera_coords[1], camera_coords[2]);
            // printf(" player_direction : (%.4f, %.4f, %.4f)\n", g_players[0].GetDirection().GetX(), g_players[0].GetDirection().GetY(), g_players[0].GetDirection().GetZ());
            // printf(" center  : (%.4f, %.4f, %.4f)\n", camera_coords[0] - g_players[0].GetDirection().GetX(), camera_coords[1] + g_players[0].GetDirection().GetY(), g_players[0].GetDirection().GetZ()  + PLAYER_HEIGHT );
            // printf(" up : (%.4f, %.4f, %.4f)\n", camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]);

        }
   }
    else
    {
        // Apenas para não estragar o funcionamento planejado
        if (camera_num == EYE_CAMERA)
            {
                init_player_eye_camera_coords(player_id);
                // Testing Player 1 Camera
                gluLookAt(
                    camera_coords[0],
                    camera_coords[1],
                    camera_coords[2] + PLAYER_HEIGHT,
                    camera_coords[0] - g_players[player_id-1].GetDirection().GetX(), 
                    camera_coords[1] + g_players[player_id-1].GetDirection().GetY(),
                    camera_coords[2] + g_players[player_id-1].GetDirection().GetZ() + PLAYER_HEIGHT ,
                    camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]
                );
            }
        else if (camera_num == GUN_CAMERA)
        {
            init_player_gun_camera_coords(player_id);
            // Testing Player 1 Camera
            gluLookAt(
                camera_coords[0],
                camera_coords[1],
                camera_coords[2] + PLAYER_HEIGHT,
                camera_coords[0] - g_players[player_id-1].GetDirection().GetX(), 
                camera_coords[1] + g_players[player_id-1].GetDirection().GetY(),
                camera_coords[2] + PLAYER_HEIGHT,
                camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]
            );    
        }
        else if (camera_num == THIRD_PERSON_CAMERA)
        {
            init_player_third_person_camera_coords(player_id);
            // Testing Player 1 Camera
            gluLookAt(
                camera_coords[0] + CAMERA_THIRD_PERSON_DISTANCE*g_players[player_id-1].GetDirection().GetX(),
                camera_coords[1] - CAMERA_THIRD_PERSON_DISTANCE*g_players[player_id-1].GetDirection().GetY(),
                camera_coords[2] + PLAYER_HEIGHT + CAMERA_THIRD_PERSON_DISTANCE,
                camera_coords[0], 
                camera_coords[1],
                camera_coords[2],
                camera_up_vec[0], camera_up_vec[1], camera_up_vec[2]
            );       
        }  
    }

    DrawArenaLights();

    DrawAxes(100);

    g_arena.DrawArena();

    for ( CircularObstacle& obstacle : g_obstacles)
    {
        obstacle.DrawObstacle();
    }
    for ( ArenaPlayer& player : g_players)
    {
        if (game_winner == PLAYER1_WON && player.GetId() == PLAYER2_ID) continue;
        if (game_winner == PLAYER2_WON && player.GetId() == PLAYER1_ID) continue;
        if (game_winner == DRAW) break; // Does not draw any player
        
        player.DrawPlayer();
        for ( Bullet& bullet : player.GetBulletVec())
        {
            bullet.DrawBullet();
        }
    }
    if (!fixed_camera)
    {
        if (!(game_finished))
        {
            ImprimePlacar(-VWidth*0.5,VHeight*0.45, PLACAR_PLAYER1);
            ImprimePlacar(-VWidth*0.5,VHeight*0.40, PLACAR_PLAYER2);
        }
        else ImprimePlacar(-VWidth*0.05,VHeight*0, game_winner); 
    }

}



/**
 * @brief Renders Viewports.
 */
void renderViewports(void)
{
    // Clear the screen.
    glClear (   GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (multiviewport)
        {
            glViewport (
                0,
                0,
                (GLsizei) Width/2.0, 
                (GLsizei) Height * (5.0/7.0)
            );
            renderScene(PLAYER1_ID);

            glViewport (
                (GLsizei) Width/2.0,
                0,
                (GLsizei) Width/2.0, 
                (GLsizei) Height * (5.0/7.0)
            );
            renderScene(PLAYER2_ID);

            changeCameraType(EYE_CAMERA);
            glViewport (
                0,
                (GLsizei) Height * (5.0/7.0),
                (GLsizei) Width/2.0, 
                (GLsizei) Height * (2.0/7.0)
            );
            renderScene(PLAYER1_ID,true,EYE_CAMERA);

            glViewport (
                (GLsizei) Width/2.0,
                (GLsizei) Height * (5.0/7.0),
                (GLsizei) Width/2.0, 
                (GLsizei) Height * (2.0/7.0)
            );
            renderScene(PLAYER2_ID,true,EYE_CAMERA);

            //Reset Camera settings
            changeCameraType(camera_type_num);
        }
    else
    {
        glViewport (
            0,
            0,
            (GLsizei) Width, 
            (GLsizei) Height
        );
        renderScene(PLAYER1_ID); 
    }


    glutSwapBuffers(); // Desenha the new frame of the game.
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
        ArenaPlayer& p1 = g_players[0];
        ArenaPlayer& p2 = g_players[1];
        double dtheta = atan2(
            p1.GetPosition().GetY()-p2.GetPosition().GetY(),
            p1.GetPosition().GetX()-p2.GetPosition().GetX()
        );
        dtheta = (dtheta/RADIANS);


        p1.GetOrientation().SetYaw(-90-dtheta+180);
        p2.GetOrientation().SetYaw(-90-dtheta);

        p1.Rotate(0); // Updates Direction Vector
        p2.Rotate(0); // Updates Direction Vector

        initial_players_angle.push_back(p1.GetOrientation().GetYaw());
        initial_players_angle.push_back(p2.GetOrientation().GetYaw());
    }

    last_players_recorded_pos.clear();
    last_animation_attempt_time=glutGet(GLUT_ELAPSED_TIME);
    
    for (unsigned int i = 0; i < g_players.size(); i++)
    {
        ArenaPlayer& player = g_players[i];

        if (game_finished)
        {
            player.GetBulletVec().clear(); // Clear Bullets
            player.SetHealth(PLAYER_HEALTH); // puts start Health
            if (initial_players_pos.size()) // puts in the start Position
            {
                player.GetPosition().SetX(initial_players_pos[i].GetX());
                player.GetPosition().SetY(initial_players_pos[i].GetY());
                player.GetPosition().SetZ(initial_players_pos[i].GetZ());
                player.SetLastPosition(initial_players_pos[i]);
            }
            player.GetOrientation().SetYaw(initial_players_angle[i]); // puts in the start Yaw
            player.Rotate(0); // Updates Direction vector
            player.SetGunYaw(0.0); // puts in the start gun yaw
            player.SetGunPitch(0.0);
            player.SetMovingStatus(false);
            player.SetLastAnimationAttemptPosition(player.GetPosition());
        }
        if(first_start)
        {
            initial_players_pos.push_back(player.GetPosition());
        }

        last_players_recorded_pos.push_back(player.GetPosition());
    }
    if (first_start) first_start=false; //first setup

    init_spectator_camera_direction_vector();
    init_third_person_camera_direction_vector();
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
            if (!toggle_player_camera) camera_type_num = SPECTATOR_CAMERA;
            else camera_type_num = last_camera_type_num;
            changeCameraType(camera_type_num);
            break;
        
        case 'g':
        case 'G':
            toggle_light = !toggle_light;
            if (toggle_light) glEnable(GL_LIGHTING);
            else glDisable(GL_LIGHTING);
            break;

        case 'n':
        case 'N':
            night_mode =! night_mode;
            break;
  
        //------------------Player 1------------------//
        case 'w':
        case 'W':
            keyStatus[(int)('w')] = 1; //Using keyStatus trick
            break;
        case 's':
        case 'S':
            keyStatus[(int)('s')] = 1; //Using keyStatus trick
            break;
        case 'a':
        case 'A':
            keyStatus[(int)('a')] = 1; //Using keyStatus trick
            break;
        case 'd':
        case 'D':
            keyStatus[(int)('d')] = 1; //Using keyStatus trick
            break;

        case 'x':
        case 'X':
            keyStatus[(int)('x')] = 1;
            break;

        //------------------Player 2------------------//
        case 'o':
        case 'O':
            keyStatus[(int)('o')] = 1; //Using keyStatus trick
            break;
        case 'l':
        case 'L':
            keyStatus[(int)('l')] = 1; //Using keyStatus trick
            break;
        case 'k':
        case 'K':
            keyStatus[(int)('k')] = 1; //Using keyStatus trick
            break;
        case SPECIAL_KEY:
        case CAPS_SPECIAL_KEY:
            keyStatus[(int)(SPECIAL_KEY)] = 1; //Using keyStatus trick
            break;
        case '2':
            keyStatus[(int)('2')] = 1; //Using keyStatus trick
            break;
        case '5':
            keyStatus[(int)('5')] = 1; //Using keyStatus trick
            break;
        case '4':
            keyStatus[(int)('4')] = 1; //Using keyStatus trick
            break;
        case '6':
            keyStatus[(int)('6')] = 1; //Using keyStatus trick
            break;
        case '8':
            keyStatus[(int)('8')] = 1; //Using keyStatus trick
            break;
        case '.':
            keyStatus[(int)('.')] = 1; //Using keyStatus trick
            break;

        //------------------Game------------------//
        case 'b':
        case 'B':
            if (toggle_player_camera) 
            {
                camera_type_num = THIRD_PERSON_CAMERA;
                last_camera_type_num = THIRD_PERSON_CAMERA;
                changeCameraType(camera_type_num);
            }
            break;

        case 'v':
        case 'V':
            if (toggle_player_camera) 
            {
                camera_type_num = GUN_CAMERA;
                last_camera_type_num = GUN_CAMERA;
                changeCameraType(camera_type_num);
            }
            break;
        
        case 'c':
        case 'C':
            if (toggle_player_camera) 
            {
                camera_type_num = EYE_CAMERA;
                last_camera_type_num = EYE_CAMERA;
                changeCameraType(camera_type_num);
            }
            break;

        case 'r':
        case 'R':
            init_game();
            game_finished = false; //Using keyStatus trick
            break;
        
        case ESC_KEY :
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
    //Initialize keyStatus
    for(i = 0; i < 256; i++)
       keyStatus[i] = 0; 
}



/**
 * @brief Glut Idle Function
 */
void idle(void)
{
    static GLdouble previousTime = glutGet(GLUT_ELAPSED_TIME);
    GLdouble currentTime, timeDiference;

    //Pega o tempo que passou do inicio da aplicacao
    currentTime = glutGet(GLUT_ELAPSED_TIME);

    // Calcula o tempo decorrido desde de a ultima frame.
    timeDiference = currentTime - previousTime;

    //Atualiza o tempo do ultimo frame ocorrido
    previousTime = currentTime;

    if (!game_finished)
    {
    
        if(g_players[0].GetHealth() == 0 || g_players[1].GetHealth() == 0)
        {
            if (g_players[0].GetHealth() == 0 && g_players[1].GetHealth() == 0) game_winner=DRAW;
            else if (g_players[0].GetHealth() == 0) game_winner=PLAYER2_WON;
            else if (g_players[1].GetHealth() == 0) game_winner=PLAYER1_WON;
            game_finished=true;
        }
        AnimatePlayers(currentTime);
        if (!camera_spectator_mode) 
        {
            Player1_Keys(timeDiference*TIME_S,currentTime);
            Player2_Keys(timeDiference*TIME_S,currentTime);
        }
        else SpectatorMode_Keys(timeDiference*TIME_S);
        Player1_Bullets(timeDiference*TIME_S);
        Player2_Bullets(timeDiference*TIME_S);

        
        glutPostRedisplay();
    }

    gPastMouseX = gCurrentMouseX;
    gPastMouseY = gCurrentMouseY;
}



/**
 * @brief Init Glut Params
 */
void gl_init(void)
{
    ResetKeyStatus();

    glClearColor (0.0, 0.0, 0.0, 0.0);
    //glShadeModel (GL_FLAT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_LIGHT4);
    

    glEnable(GL_DEPTH_TEST);
    changeCameraType(camera_type_num);
    
    toggle_light = !toggle_light;
    toggle_texture = !toggle_texture;
}



/**
 * @brief Loads Arema
 * @param path path to the arena file
 */
int load_svg(const char* path)
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

    std::optional<std::vector<CircularObstacle>> obstacles = obstacles_getter(circle_vec,*arena);
    std::optional<std::vector<ArenaPlayer>> players = players_getter(circle_vec,*arena);
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
        for ( ArenaPlayer& p : g_players)
        {
            printf("Player %d: \n",(i++)+1);
            p.PrintAttributes();
            printf("\n\n");
        }

        i = 0;
        for ( CircularObstacle& obs : g_obstacles)
        {
            printf("Obstacle %d: \n",(i++)+1);
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
    if (result) return result;

    // Initialize openGL with Double buffer and RGB color without transparency.
    // Its interesting to try GLUT_SINGLE instead of GLUT_DOUBLE.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 
    // Create the window.
    glutInitWindowSize(Width, Height);
    glutInitWindowPosition(150,50);
    glutCreateWindow("Joguin 3D");

    // Viewing Height=Width = Arena Diamater
    VHeight=VWidth=2*g_arena.GetRadius();

    // Define callbacks.
    glutDisplayFunc(renderViewports);
    glutKeyboardFunc(keyPress);
    glutIdleFunc(idle);
    glutKeyboardUpFunc(keyup);
    // glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseClick);
    glutMotionFunc(globalmouseMotion);
    glutPassiveMotionFunc(globalmouseMotion);

    init_game();
    gl_init();
    glutMainLoop();
 
    return 0;
}
