// This will store the game strategies for the agents.

#ifndef TACTIC_H
#define TACTIC_H

#include "box2d/box2d.h"

class Tactic
{
    public:
        virtual ~Tactic() = default;
        virtual bool doTactic(b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) const = 0;
        virtual void updateTactic(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) = 0;
};

#endif // TACTIC_H