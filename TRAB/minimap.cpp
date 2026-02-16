#include "minimap.h"


void Minimap::Draw2DArena()
{
    glPushMatrix();
        glTranslatef(
            g_arena->GetPosition().GetX(),
           -g_arena->GetPosition().GetY(),
            0
        );
        DrawCirc(
            g_arena->GetRadius(),
            g_arena->GetRGB().GetR(),
            g_arena->GetRGB().GetG(),
            g_arena->GetRGB().GetB(),
            GL_LINE_LOOP
        );
    glPopMatrix();
}

void Minimap::Draw2DObstacle(CircularObstacle& obstacle)
{
    glPushMatrix();
        glTranslatef(
            obstacle.GetPosition().GetX(),
            -obstacle.GetPosition().GetY(),
            0
        );
        DrawCirc(
            obstacle.GetRadius(),
            obstacle.GetRGB().GetR(),
            obstacle.GetRGB().GetG(),
            obstacle.GetRGB().GetB(),
            GL_LINE_LOOP
        );
    glPopMatrix();
}

void Minimap::Draw2DPlayer(ArenaPlayer& player, short player_id)
{
    glPushMatrix();

        glTranslatef(
            player.GetPosition().GetX(),
            -player.GetPosition().GetY(),
            0 //Ignorar no Z se não some do mapa kk
        );

        if (player.GetId() == player_id)
        {
            DrawCircWithBorder(
                player.GetRadius(),
                0.0,1.0,0.0
            );
        }
        else
        {
            DrawCircWithBorder(
                player.GetRadius(),
                1.0,0.0,0.0
            );    
        }


    glPopMatrix();

}
void Minimap::DrawMinimap(short player_id)
{
    glPushMatrix();

        Draw2DArena();
        
        for( CircularObstacle& obstacle : *g_obstacles)
        {
            Draw2DObstacle(obstacle);
        }

        for( ArenaPlayer& player: *g_players)
        {
            Draw2DPlayer(player,player_id);
        }

    glPopMatrix();
};