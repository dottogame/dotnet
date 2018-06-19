#pragma once

#include <string>
#include <vector>

class lobby
{
public:
    std::string name = "LAN Play";

    std::vector<std::string> players;

    std::string owner;

    lobby(std::string creator_key)
    {
        players.push_back(creator_key);
        owner = creator_key;
    }
};
