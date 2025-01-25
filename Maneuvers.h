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
    RandomMove() = default; //TODO seed this value to continue determinism
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
        return push;
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        this->push = -player_velocity;
    };
private:
    b2Vec2 push = b2Vec2(0.0f, 0.0f);
};

class Accelerate : public Maneuver //TODO add this to the the simulation
{
public:
    Accelerate() = default;
    b2Vec2 doManeuver() const override
    {
        return push;
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        this->push = player_velocity;
    };
private:
    b2Vec2 push = b2Vec2(0.0f, 0.0f);
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

class Smart : public Maneuver
{
    public:
    Smart() = default;
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
        else
        {
            this->push = contact_point - player_point;
        }
    };
    private:
    b2Vec2 push = b2Vec2(0.0f, 0.0f);
};
class Sum : public Maneuver
{
    public:
    Sum() = default;
    b2Vec2 doManeuver() const override
    {
        return this->push;
    };
    void updateManeuver(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        collisions[iterator] = contact_point - player_point;
        iterator++;
        if (iterator >= 2){iterator = 0;};
        for (int i = 0; i < 2; i++){
            if (i == 0){ push = b2Vec2(0.0f, 0.0f);};
            this->push += collisions[i];
        }
    };
    private:
    b2Vec2 push = b2Vec2(0.0f, 0.0f);
    int iterator = 0;
    std::vector<b2Vec2> collisions = {b2Vec2(0.0f, 0.0f),b2Vec2(0.0f, 0.0f)};
};
#endif // MANEUVERS_H