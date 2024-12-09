#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <unordered_map>
#include <bitset>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <array>
#include <random>

#include "Maneuvers.h"
#include "Tactics.h"

b2Body *CreateWall(b2World *world, const b2Vec2 &position, const b2Vec2 &size)
{
    // Hand this meters, not pixels (divide by 30)
    b2BodyDef bodyDef;
    bodyDef.position = position;
    b2Body *body = world->CreateBody(&bodyDef);

    b2PolygonShape box;
    box.SetAsBox(size.x / 2, size.y / 2);

    body->CreateFixture(&box, 0.0f);

    return body;
}

b2Body *CreateCircle(b2World *world, int id, const b2Vec2 &position, float radius, float density, float friction, float restitution)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = position;
    uintptr_t pointer = (uintptr_t)id;
    bodyDef.userData.pointer = pointer;
    b2Body *body = world->CreateBody(&bodyDef);

    b2CircleShape circle;
    circle.m_radius = radius;

    b2FixtureDef fixtureDef; // define the fixture, which defines the physics to apply to the body
    fixtureDef.shape = &circle;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.restitution = restitution;

    body->CreateFixture(&fixtureDef);

    return body;
}

b2Vec2 ApplyMovementCap(b2Vec2 force, float movement_cap)
{
    if (force.Length() > movement_cap)
    {
        force.Normalize();
        force *= movement_cap;
    }
    return force;
}

int main()
{

    uint32_t round = 0;
    bool paused = true;

    // constants
    std::cout << "Retrieving settings..." << std::endl;
    // std::string temp_maneuver_legend;
    // std::string temp_tactic_legend;
    bool temp_draw = true;
    bool temp_play = true;
    bool temp_text = true;
    bool temp_cache = true;
    bool temp_verbose = false;
    int temp_seed = 0;
    int temp_num_agents = 100;
    float temp_agent_radius = 0.5f;
    int temp_max_rounds = 1000;

    // physics constants
    b2Vec2 temp_gravity(0.0f, 0.0f);
    float temp_time_step = 1.0f / 60.0f;
    int32 temp_velocity_iterations = 6;
    int32 temp_position_iterations = 2;
    float temp_density = 1.0f;
    float temp_friction = 0.3f;
    float temp_restitution = 0.5f;
    b2Vec2 temp_start_position = b2Vec2(1.0f, 1.0f);
    b2Vec2 temp_start_force = b2Vec2(1.0f, 1.0f);
    float temp_speed_cap = 1.0f;
    float temp_acceleration_cap = 0.05f;

    // rendering constants
    int temp_window_x = 40.0f;
    int temp_window_y = 30.0f;
    int temp_framerate = 120;

    std::ifstream config_in("../Configs/Settings.csv");

    std::string line;
    // ignore the first line
    std::getline(config_in, line);

    while (std::getline(config_in, line))
    {
        std::stringstream config_string(line);

        std::string setting, setting_value;

        std::getline(config_string, setting, ';');
        std::getline(config_string, setting_value, ';');

        // if (setting == "tactics"){
        //     temp_tactic_legend = setting_value;
        // }
        // else if (setting == "maneuvers"){
        //     temp_maneuver_legend = setting_value;
        // }
        if (setting == "no_draw")
        {
            temp_draw = stoi(setting_value) == 0;
        }
        else if (setting == "no_play")
        {
            temp_play = stoi(setting_value) == 0;
        }
        else if (setting == "no_text")
        {
            temp_text = stoi(setting_value) == 0;
        }
        else if (setting == "no_cache")
        {
            temp_cache = stoi(setting_value) == 0;
        }
        else if (setting == "verbose")
        {
            temp_verbose = stoi(setting_value) == 1;
        }
        else if (setting == "pause_on_start")
        {
            paused = stoi(setting_value) == 1;
        }
        else if (setting == "seed")
        {
            temp_seed = std::stoi(setting_value);
        }
        else if (setting == "num_agents")
        {
            temp_num_agents = std::stoi(setting_value);
        }
        else if (setting == "agent_radius")
        {
            temp_agent_radius = std::stof(setting_value);
        }
        else if (setting == "max_rounds")
        {
            temp_max_rounds = std::stoi(setting_value);
        }
        else if (setting == "gravity_x")
        {
            temp_gravity.x = std::stof(setting_value);
        }
        else if (setting == "gravity_y")
        {
            temp_gravity.y = std::stof(setting_value);
        }
        else if (setting == "time_step")
        {
            temp_time_step = std::stof(setting_value);
        }
        else if (setting == "velocity_iterations")
        {
            temp_velocity_iterations = std::stoi(setting_value);
        }
        else if (setting == "position_iterations")
        {
            temp_position_iterations = std::stoi(setting_value);
        }
        else if (setting == "density")
        {
            temp_density = std::stof(setting_value);
        }
        else if (setting == "friction")
        {
            temp_friction = std::stof(setting_value);
        }
        else if (setting == "restitution")
        {
            temp_restitution = std::stof(setting_value);
        }
        else if (setting == "start_position_x")
        {
            temp_start_position.x = std::stof(setting_value);
        }
        else if (setting == "start_position_y")
        {
            temp_start_position.y = std::stof(setting_value);
        }
        else if (setting == "start_force_x")
        {
            temp_start_force.x = std::stof(setting_value);
        }
        else if (setting == "start_force_y")
        {
            temp_start_force.y = std::stof(setting_value);
        }
        else if (setting == "speed_cap")
        {
            temp_speed_cap = std::stof(setting_value);
        }
        else if (setting == "acceleration_cap")
        {
            temp_acceleration_cap = std::stof(setting_value);
        }
        else if (setting == "window_x")
        {
            temp_window_x = std::stoi(setting_value);
        }
        else if (setting == "window_y")
        {
            temp_window_y = std::stoi(setting_value);
        }
        else if (setting == "framerate")
        {
            temp_framerate = std::stoi(setting_value);
        }
        else
        {
            std::cerr << "Invalid setting: " << setting << std::endl;
            return 1;
        }
    }

    config_in.close();
    std::cout << "Retrieved settings!" << std::endl
              << "Setting up game..." << std::endl;

    // game settings
    // const std::string MANEUVER_LEGEND = temp_maneuver_legend;
    // const std::string TACTIC_LEGEND = temp_tactic_legend;
    const bool DRAW = temp_draw;
    const bool PLAY = temp_play;
    const bool TEXT = temp_text;
    const bool CACHE = temp_cache;
    const bool VERBOSE = temp_verbose;
    const int SEED = temp_seed;
    std::mt19937 generator(SEED);
    const int NUMAGENTS = temp_num_agents;
    const float AGENTRADIUS = temp_agent_radius;
    const int MAXROUNDS = temp_max_rounds + temp_num_agents - 1;

    // directory settings
    const auto dirIter = std::filesystem::directory_iterator("../Data");
    int dirCount = 0;

    for (auto &entry : dirIter)
    {
        if (entry.is_directory())
        {
            ++dirCount;
        }
    }
    const std::string SAVELOCATION = "..\\Data\\Iteration_" + std::to_string(dirCount) + "\\";

    if (CACHE == true)
    {
        std::filesystem::create_directories(SAVELOCATION);

        std::string agent_in = "..\\Configs\\Agents.csv";
        std::string agent_out = SAVELOCATION + "Agents.csv";
        std::string settings_in = "..\\Configs\\Settings.csv";
        std::string settings_out = SAVELOCATION + "Settings.csv";

        std::filesystem::copy(agent_in, agent_out);
        std::filesystem::copy(settings_in, settings_out);
    }

    // physics settings
    const b2Vec2 GRAVITY(temp_gravity.x, temp_gravity.y);
    const float TIMESTEP = temp_time_step / temp_framerate;
    const int32 VELOCITYITERATIONS = temp_velocity_iterations;
    const int32 POSITIONITERATIONS = temp_position_iterations;
    const float DENSITY = temp_density;

    const float FRICTION = temp_friction;
    const float RESTITUTION = temp_restitution;
    const b2Vec2 STARTPOSITION(temp_start_position.x, temp_start_position.y);
    const b2Vec2 STARTFORCE(temp_start_force.x, temp_start_force.y);
    const float SPEEDCAP = temp_speed_cap;
    const float ACCELERATIONCAP = temp_acceleration_cap;

    // rendering settings
    const int SCALE = 30.0f;
    const int WINDOWX = temp_window_x;
    const int WINDOWY = temp_window_y;
    const int FRAMERATE = temp_framerate;

    // game constants, for reference even though its unused
    const unsigned char PAYOFFMATRIX[2][2][2] = {
        // accessed via payoffMatrix[choice_1][choice_2][player]
        // 0 = cooperate, 1 = defect
        {{3, 3}, {0, 5}},
        {{5, 0}, {1, 1}}};

    std::unordered_map<uint64_t, uint8_t> history;

    sf::Font font;
    if (!font.loadFromFile("Roboto-Bold.ttf"))
    {
        std::cout << "failed to load font" << std::endl;
    }

    sf::RenderWindow window(sf::VideoMode(WINDOWX * SCALE, WINDOWY * SCALE),"");
    window.setFramerateLimit(FRAMERATE);

    b2World *world = new b2World(GRAVITY);

    // create walls
    CreateWall(world, b2Vec2(0.0f, 0.0f), b2Vec2(WINDOWX * 2, 1.0f));
    CreateWall(world, b2Vec2(0.0f, 0.0f), b2Vec2(1.0f, WINDOWY * 2));
    CreateWall(world, b2Vec2(0.0f, WINDOWY), b2Vec2(WINDOWX * 2, 1.0f));
    CreateWall(world, b2Vec2(WINDOWX, 0.0f), b2Vec2(1.0f, WINDOWY * 2));

    std::cout << "Game set!" << std::endl
              << "Creating agents..." << std::endl;

    b2Body *bodies[NUMAGENTS];
    for (int i = 0; i < NUMAGENTS; i++)
    {
        bodies[i] = nullptr;
    } // initialize for later trickle effect

    // initialize maneuvers
    std::vector<Maneuver *> maneuvers;
    std::string maneuver_names[NUMAGENTS];
    sf::Color fill[NUMAGENTS];

    // initialize tactics
    std::vector<Tactic *> tactics;
    std::string tactic_names[NUMAGENTS];
    sf::Color outline[NUMAGENTS];

    std::ifstream agent_in_file("../Configs/Agents.csv");

    // ignore the first line
    std::getline(agent_in_file, line);

    while (std::getline(agent_in_file, line))
    {
        std::stringstream agent_string(line);
        // agent_id,maneuvers,maneuver_colors,tactics,tactic_colors
        std::string agent_id_string, maneuver, maneuver_color, maneuver_color_binary, tactic, tactic_color, tactic_color_binary;

        std::getline(agent_string, agent_id_string, ';');
        std::getline(agent_string, maneuver, ';');
        std::getline(agent_string, maneuver_color, ';');
        std::getline(agent_string, maneuver_color_binary, ';');
        std::getline(agent_string, tactic, ';');
        std::getline(agent_string, tactic_color, ';');
        std::getline(agent_string, tactic_color_binary, ';');

        int agent_id = std::stoi(agent_id_string);
        int r, g, b, a; // needed to convert from the array string of colors to integers

        // Equivalent to instantiating maneuvers
        if (maneuver == "Up")
        {
            maneuvers.push_back(new Up());
        }
        else if (maneuver == "Down")
        {
            maneuvers.push_back(new Down());
        }
        else if (maneuver == "Left")
        {
            maneuvers.push_back(new Left());
        }
        else if (maneuver == "Right")
        {
            maneuvers.push_back(new Right());
        }
        else if (maneuver == "RandomMove")
        {
            maneuvers.push_back(new RandomMove());
        }
        else if (maneuver == "Still")
        {
            maneuvers.push_back(new Still());
        }
        else if (maneuver == "Chase")
        {
            maneuvers.push_back(new Chase());
        }
        else if (maneuver == "Flee")
        {
            maneuvers.push_back(new Flee());
        }
        else if (maneuver == "WinChase")
        {
            maneuvers.push_back(new WinChase());
        }
        else if (maneuver == "LossFlee")
        {
            maneuvers.push_back(new LossFlee());
        }
        else
        {
            std::cerr << "Invalid maneuver type: " << maneuver << std::endl;
            return 1;
        }
        maneuver_names[agent_id] = maneuver;
        fill[agent_id] = sf::Color(std::stoul(maneuver_color_binary));

        // Equivalent to instantiating tactics
        if (tactic == "Defect")
        {
            tactics.push_back(new Defect());
        }
        else if (tactic == "Cooperate")
        {
            tactics.push_back(new Cooperate());
        }
        else if (tactic == "TitForTat")
        {
            tactics.push_back(new TitForTat(NUMAGENTS));
        }
        else if (tactic == "NaiveTitForTat")
        {
            tactics.push_back(new NaiveTitForTat());
        }
        else if (tactic == "RandomChoice")
        {
            tactics.push_back(new RandomChoice());
        }
        else
        {
            std::cerr << "Invalid tactic type: " << tactic << std::endl;
            return 1;
        }
        tactic_names[agent_id] = tactic;
        outline[agent_id] = sf::Color(std::stoul(tactic_color_binary));
    }

    agent_in_file.close(); // Close the file when done
    std::cout << "Agents created!" << std::endl
              << "Adding agents to game..." << std::endl;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Space)
                {
                    paused = !paused;
                }
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        if (round > MAXROUNDS)
        {
            window.close();
        }
        if (round == NUMAGENTS)
        {
            std::cout << "Agents added to game." << std::endl
                      << "Playing game..." << std::endl;
        }

        if (paused == false)
        {

            world->Step(TIMESTEP, VELOCITYITERATIONS, POSITIONITERATIONS);

            if (round < NUMAGENTS) // trickle effect
            {
                bodies[round] = CreateCircle(world, round, STARTPOSITION, AGENTRADIUS, DENSITY, FRICTION, RESTITUTION);
                std::uniform_int_distribution<int> distribution(-3, 3);
                bodies[round]->ApplyLinearImpulse(b2Vec2(distribution(generator), distribution(generator)), bodies[round]->GetWorldCenter(), true);
            }

            if (PLAY == true && round > NUMAGENTS) // needed for the trickle effect to not bias results
            {
                std::vector<std::vector<bool>> collisions(NUMAGENTS, std::vector<bool>(NUMAGENTS, 0));
                b2Contact *contact = world->GetContactList(); // Get the head of the contact list

                while (contact != nullptr)
                {
                    b2Fixture *fixture_a = contact->GetFixtureA();
                    b2Fixture *fixture_b = contact->GetFixtureB();
                    b2Body *body_a = fixture_a->GetBody();
                    b2Body *body_b = fixture_b->GetBody();
                    uint16_t id_1 = body_a->GetUserData().pointer;
                    uint16_t id_2 = body_b->GetUserData().pointer;

                    // Check if the contact is actually enabled (true collision) and not already played
                    if (collisions[id_1][id_2] == 0 && contact->IsTouching())
                    {
                        // add the collision to the list
                        collisions[id_1][id_2] = 1;
                        // Access the contact manifold to get the contact points
                        b2WorldManifold world_manifold;
                        contact->GetWorldManifold(&world_manifold);
                        b2Vec2 contactPoint = world_manifold.points[0]; // I can do this because they are all circles
                        b2Vec2 id_1_position = body_a->GetPosition();
                        b2Vec2 id_1_velocity = body_a->GetLinearVelocity();
                        b2Vec2 id_2_position = body_b->GetPosition();
                        b2Vec2 id_2_velocity = body_b->GetLinearVelocity();

                        // b2Vec2 contactPoint, b2Vec2 playerPoint, b2Vec2 oppPoint, b2Vec2 playerVelocity, b2Vec2 oppVelocity, uint16_t opponent
                        //  this constitutes playing the game
                        bool choice_1 = tactics[id_1]->doTactic(contactPoint, id_1_position, id_2_position, id_1_velocity, id_2_velocity, id_2);
                        bool choice_2 = tactics[id_2]->doTactic(contactPoint, id_2_position, id_1_position, id_2_velocity, id_1_velocity, id_1);

                        // bool youchoice, bool oppchoice, uint16_t p1, uint16_t p2
                        tactics[id_1]->updateTactic(choice_1, choice_2, contactPoint, id_1_position, id_2_position, id_1_velocity, id_2_velocity, id_2);
                        tactics[id_2]->updateTactic(choice_2, choice_1, contactPoint, id_2_position, id_1_position, id_2_velocity, id_1_velocity, id_1);

                        // TODO: add conditional updates
                        maneuvers[id_1]->updateManeuver(choice_1, choice_2, contactPoint, id_1_position, id_2_position, id_1_velocity, id_2_velocity, id_2);
                        maneuvers[id_2]->updateManeuver(choice_2, choice_1, contactPoint, id_2_position, id_1_position, id_2_velocity, id_1_velocity, id_1);

                        // update the history
                        // to halve memory needs, we garuntee agent1 < agent2
                        if (id_1 > id_2)
                        {
                            uint16_t temp = id_1;
                            id_1 = id_2;
                            id_2 = temp;
                            bool temp_choice = choice_1;
                            choice_1 = choice_2;
                            choice_2 = temp_choice;
                            id_1_position = body_b->GetPosition();
                            id_2_position = body_a->GetPosition();
                        }

                        uint64_t history_key = (static_cast<uint64_t>(round) << 32) | (static_cast<uint64_t>(id_1) << 16) | static_cast<uint64_t>(id_2);

                        uint8_t game_state = (choice_1) << 1 | choice_2;

                        history[history_key] = game_state; // TODO add positions
                    }

                    // Move to the next contact in the list
                    contact = contact->GetNext();
                }

                // do the maneuvers
                for (int i = 0; i < NUMAGENTS; i++) // going to test agents moving each round first
                {
                    b2Vec2 force = maneuvers[i]->doManeuver();

                    // Acceleration cap
                    force = ApplyMovementCap(force, ACCELERATIONCAP);

                    // Speed cap

                    bodies[i]->SetLinearVelocity(ApplyMovementCap(bodies[i]->GetLinearVelocity(), SPEEDCAP));

                    bodies[i]->ApplyLinearImpulse(force, bodies[i]->GetWorldCenter(), true);
                }
            }

            if (DRAW == true)
            {
                // draw the agents
                window.clear();
                for (int i = 0; i < NUMAGENTS; i++)
                {
                    if (bodies[i] != nullptr)
                    {
                        b2Fixture *fixture = bodies[i]->GetFixtureList();
                        b2CircleShape *circle_shape = static_cast<b2CircleShape *>(fixture->GetShape());
                        float radius = circle_shape->m_radius;

                        sf::CircleShape circle(radius * SCALE);
                        circle.setOrigin(radius * SCALE, radius * SCALE);
                        circle.setPosition(SCALE * bodies[i]->GetPosition().x, SCALE * bodies[i]->GetPosition().y);
                        circle.setRotation(bodies[i]->GetAngle() * 180.0f / b2_pi);

                        circle.setFillColor(fill[i]);
                        circle.setOutlineThickness(-(radius / 3 * SCALE));
                        circle.setOutlineColor(outline[i]);
                        window.draw(circle);

                        if (TEXT == true)
                        {

                            sf::Text text;

                            int text_size = radius * SCALE;

                            // select the font
                            text.setFont(font); // font is a sf::Font

                            // set the string to display
                            text.setString(std::to_string(i));

                            // set the character size
                            text.setCharacterSize(text_size); // in pixels, not points!

                            // set the color
                            text.setFillColor(sf::Color::Black);

                            // set the position to the agents position, with an offset of the textsize to fit the shape
                            text.setPosition(SCALE * bodies[i]->GetPosition().x - text_size * 9 / 10, SCALE * bodies[i]->GetPosition().y - text_size / 2);

                            // inside the main loop, between window.clear() and window.display()
                            window.draw(text);
                        }
                    }; // lets me do the feed in effect
                }

                window.display();
            }

            round++;
        }
    }

    if (CACHE == true)
    {
        std::cout << "Game finished!" << std::endl
                  << "Caching history..." << std::endl;
        // Create the files
        std::ofstream history_file(SAVELOCATION + "Raw_History.csv");

        // write the history
        history_file << "key;value" << std::endl;
        for (const auto &pair : history)
        {
            history_file << pair.first << ";" << (int)pair.second << std::endl;
        }

        // close the files
        history_file.close();

        std::string decode = "cd .. && py Decode.py --dir " + SAVELOCATION + (VERBOSE ? " --verbose" : "");

        const char *command = decode.c_str();

        int result = std::system(command);

        if (result == 0)
        {
            std::cout << "Python script executed successfully.\n";
        }
        else
        {
            std::cerr << "Failed to execute Python script.\n";
        }

        std::string graph;

        graph = "cd .. && py Graph.py --dir " + SAVELOCATION + (DRAW ? "" : " --no_draw") + (VERBOSE ? " --verbose" : "");

        command = graph.c_str();

        result = std::system(command);

        if (result == 0)
        {
            std::cout << "Python script executed successfully.\n";
        }
        else
        {
            std::cerr << "Failed to execute Python script.\n";
        }
    }
    return 0;
}