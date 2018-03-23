#pragma once

#include<vector>

#include "Lobby.hpp"
#include "Config.hpp"

class engine
{
public:
    bool running = true;

    std::vector<lobby*> lobbies;

    config* conf;
};