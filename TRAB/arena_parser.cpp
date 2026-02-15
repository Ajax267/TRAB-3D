#include "arena_parser.h"

std::optional<CircularArena> arena_getter(std::vector<CircleDefinition>& svg_circles)
{
    for (CircleDefinition& circle : svg_circles)
    {
        if (circle.GetColorName() == "blue")
        {
            return CircularArena(
                circle.GetPosition().GetX(),
                circle.GetPosition().GetY(),
                circle.GetRadius(),
                circle.GetColorName()
            );
        }
    }

    return std::nullopt;
}

std::optional<std::vector<ArenaPlayer>> players_getter(
    std::vector<CircleDefinition>& svg_circles,
    CircularArena& arena
)
{
    std::vector<ArenaPlayer> players_vec;
    int id = 1;
    for (CircleDefinition& circle : svg_circles)
    {
        if (circle.GetColorName() != "blue" && circle.GetColorName() != "black")
        {
            players_vec.push_back(
                ArenaPlayer(
                    circle.GetPosition().GetX() - arena.GetPosition().GetX(),
                    circle.GetPosition().GetY() - arena.GetPosition().GetY(),
                    0 - arena.GetPosition().GetZ(),
                    0.0,0.0,0.0,
                    circle.GetColorName(),
                    0,0,0,
                    circle.GetRadius(),
                    id++
                )
            );

        }
    }


    if (players_vec.size() > 0)
    {
        if (players_vec[0].GetColorName() != "green")
        {
            std::swap(players_vec[0], players_vec[1]);
            std::swap(players_vec[0].GetID(), players_vec[1].GetID());
        }
    
        return players_vec;
    }

    return std::nullopt;
}

std::optional<std::vector<CircularObstacle>> obstacles_getter(
    std::vector<CircleDefinition>& svg_circles,
    CircularArena& arena
)
{
    std::vector<CircularObstacle> obstacles_vec;
    for (CircleDefinition& circle : svg_circles)
    {
        if (circle.GetColorName() == "black")
        {
            obstacles_vec.push_back(
                CircularObstacle(
                    circle.GetPosition().GetX() - arena.GetPosition().GetX(),
                    circle.GetPosition().GetY() - arena.GetPosition().GetY(),
                    circle.GetRadius(),
                    circle.GetColorName()
                )
            );
        }
    }

    if (obstacles_vec.size() > 0)
    {
        return obstacles_vec;
    }

    return std::nullopt;
}