#pragma once

#include <SFML\Network.hpp>

namespace endpoint
{
    void ping(sf::IpAddress& sender, unsigned short& port, sf::UdpSocket& socket)
    {
        socket.send("pong!", 5, sender, port);
    }

    void lobby_count(sf::IpAddress& sender, unsigned short& port, config* configuration)
    {
        // the key to get an id is the ip and port concatenated
        std::string key = sender.toString() + std::to_string(port);

        // check if we have the id for this ip. ignore by returning if not
        auto it = tsd::ip_to_id.find(key);
        if (it == tsd::ip_to_id.end()) return;

        // grab user connection
        auto con = tsd::con_list[tsd::ip_to_id[key]];

        // send lobby list message: "reply lobby <lobby count>" 
        if (configuration->lobbies) tsd::pack(con, "rl");

        // send (no lobbies) message: "reply lobby none"
        else tsd::pack(con, "rl0");
    }

    void lobby_details(char* data, sf::IpAddress& sender, unsigned short& port, config* configuration)
    {
        // the key to get an id is the ip and port concatenated
        std::string key = sender.toString() + std::to_string(port);

        // check if we have the id for this ip. ignore by returning if not
        auto it = tsd::ip_to_id.find(key);
        if (it == tsd::ip_to_id.end()) return;

        // grab user connection
        auto con = tsd::con_list[tsd::ip_to_id[key]];

        // send lobby list message: "reply lobby details: [lobby details]" 
        if (configuration->lobbies) tsd::pack(con, "rld:" + 0);

        // send (no lobbies) message: "reply lobby error: <error>"
        else tsd::pack(con, "rle:this server has no lobbies");
    }

    void get_stack(char* data, sf::IpAddress& sender, unsigned short& port, sf::UdpSocket& socket)
    {
        // the key to get an id is the ip and port concatenated
        std::string key = sender.toString() + std::to_string(port);

        // check if we have the id for this ip. ignore by returning if not
        auto it = tsd::ip_to_id.find(key);
        if (it == tsd::ip_to_id.end()) return;

        // grab user connection and send message
        auto con = tsd::con_list[tsd::ip_to_id[key]];
        std::string msg = tsd::fetch(con, data[0]);
        const char* buff = msg.c_str();
        socket.send(buff, msg.size(), sender, port);
    }
}