#ifndef MANEUVERS_H
#define MANEUVERS_H

#include <iostream>
#include <random>

#include "Maneuver.h"

class Up : public Maneuver
{
public:
    Up() = default;
    b2Vec2 doManeuver() const override
    {
        return b2Vec2(0.0f, -0.1f);
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        // do nothing
    };
};

class Down : public Maneuver
{
public:
    Down() = default;
    b2Vec2 doManeuver() const override
    {
        return b2Vec2(0.0f, 0.1f);
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        // do nothing
    };
};

class Left : public Maneuver
{
public:
    Left() = default;
    b2Vec2 doManeuver() const override
    {
        return b2Vec2(-0.1f, 0.0f);
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        // do nothing
    };
};

class Right : public Maneuver
{
public:
    Right() = default;
    b2Vec2 doManeuver() const override
    {
        return b2Vec2(0.1f, 0.0f);
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        // do nothing
    };
};

class RandomMove : public Maneuver
{
public:
    RandomMove() = default;
    b2Vec2 doManeuver() const override
    {
        return b2Vec2((rand() % 3) - 1, (rand() % 3) - 1);
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        // do nothing
    };
};

class Still : public Maneuver
{
public:
    Still() = default;
    b2Vec2 doManeuver() const override
    {
        return b2Vec2(0.0f, 0.0f);
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        // do nothing
    };
};

class Chase : public Maneuver
{
public:
    Chase() = default;
    b2Vec2 doManeuver() const override
    {
        return this->push;
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        this->push = contact_point - player_point;
    };
    private:
    b2Vec2 push = b2Vec2(0.0f, 0.0f);
};

class Flee : public Maneuver
{
public:
    Flee() = default;
    b2Vec2 doManeuver() const override
    {
        return this->push;
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        this->push = player_point - contact_point;
    };
    private:
    b2Vec2 push = b2Vec2(0.0f, 0.0f);
};

class WinChase : public Maneuver
{
public:
    WinChase() = default;
    b2Vec2 doManeuver() const override
    {
        return this->push;
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        if (opponent_choice == 0)
        {
            this->push = contact_point - player_point;
        }
    };
    private:
    b2Vec2 push = b2Vec2(0.0f, 0.0f);
};

class LossFlee : public Maneuver
{
public:
    LossFlee() = default;
    b2Vec2 doManeuver() const override
    {
        return this->push;
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        if (opponent_choice == 1)
        {
            this->push = player_point - contact_point;
        }
    };
    private:
    b2Vec2 push = b2Vec2(0.0f, 0.0f);
};
#endif // MANEUVERS_H