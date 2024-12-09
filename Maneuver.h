// This will store the  strategies for the agents.

#ifndef MAUEVER_H
#define MAUEVER_H

#include <box2d/box2d.h>

class Maneuver
{
    public:
        virtual ~Maneuver() = default;
        // this will now need to take in the collision point and booth agent's choices
        virtual b2Vec2 doManeuver() const = 0; // I use unsigned char because it is the same size as bool and mmore flexible
        virtual void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opp_point, b2Vec2 player_velocity, b2Vec2 opp_velocity, uint16_t opponent) = 0;
};

#endif // MAUEVER_H

// what info do I want these to have?
// Contact data
// Is it fair to give them game data? or should I do game data conditions in main?