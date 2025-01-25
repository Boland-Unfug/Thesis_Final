#ifndef TACTICS_H
#define TACTICS_H

#include "Tactic.h"
#include <iostream>
#include <vector>
#include <random>

class Defect : public Tactic // represented by 'd'
{
public:
    Defect() = default;
    bool doTactic(uint16_t opponent) const override { return 1; };    // also sadly have to hand opponent in
    void updateTactic(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override {}; // I think useless? but necessary?
};

class Cooperate : public Tactic // represented by 'c'
{
public:
    Cooperate() = default;
    bool doTactic(uint16_t opponent) const override { return 0; };    // also sadly have to hand opponent in
    void updateTactic(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override {}; // I think useless? but necessary?
};

class TitForTat : public Tactic // This one needs the  move of this opponent
{
public:
    TitForTat() = default;
    
    TitForTat(int num_agents){
        for (int i = 0; i < num_agents; i++){
            choices.push_back(false);
        }
    }
    bool doTactic(uint16_t opponent) const override
    {
        return this->choices[opponent];
    };
    void updateTactic(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        this->choices[opponent] = opponent_choice;
    };
private:
    std::vector<bool> choices = {};
};

class NaiveTitForTat : public Tactic // This just needs the  move
{
public:
    
    NaiveTitForTat() = default;
    // I am about to do a coding sin TODO fix this later
    bool doTactic(uint16_t opponent) const override
    {
        return this->choice;
    };
    void updateTactic(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override
    {
        this->choice = opponent_choice;
    };
private:
    bool choice = 0;
};

class RandomChoice : public Tactic
{
public:
    RandomChoice() = default;
    bool doTactic(uint16_t opponent) const override
    {
        return rand() % 2;
    };
    void updateTactic(bool player_choice, bool opponent_choice, b2Vec2 contact_point, b2Vec2 player_point, b2Vec2 opponent_point, b2Vec2 player_velocity, b2Vec2 opponent_velocity, uint16_t opponent) override {};
};
#endif // TACTICS_H