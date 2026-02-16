#include "player.h"
#include "arena.h"
#include "arena_obstacles.h"

#include <GL/gl.h>
#include <GL/glu.h>

class Minimap
{
    private:
        CircularArena* g_arena = NULL;
        std::vector<CircularObstacle>* g_obstacles = NULL;
        std::vector<ArenaPlayer>* g_players = NULL;

    public:
        Minimap() {}
        Minimap(
            CircularArena g_arena,
            std::vector<CircularObstacle> g_obstacles,
            std::vector<ArenaPlayer> g_players
        ) :  g_arena(&g_arena),g_obstacles(&g_obstacles),g_players(&g_players)
        {};

        // Drawing
        void Draw2DArena();
        void Draw2DObstacle(CircularObstacle& obstacle);
        void Draw2DPlayer(ArenaPlayer& player, short player_id);
        void DrawMinimap(short player_id);

        void SetArena(CircularArena& g_arena) {this->g_arena = &g_arena;};
        void SetObstaclesVec(std::vector<CircularObstacle>& g_obstacles) {this->g_obstacles = &g_obstacles;};
        void SetPlayersVec(std::vector<ArenaPlayer>& g_players) {this->g_players = &g_players;};
};