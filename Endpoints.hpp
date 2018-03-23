#pragma once

#include <SFML\Network.hpp>

#include "Config.hpp"
#include "TwoStateDotocol.hpp"

namespace endpoint
{
    /* UTILITY FUNCTIONS*/
    bool get_key(
        std::string& key, sf::IpAddress& sender, unsigned short& port
    )
    {
        // build key from id and port
        key = sender.toString() + std::to_string(port);

        // check if we have the id for this ip. ignore by returning if not
        auto it = tsd::ip_to_id.find(key);
        if (it == tsd::ip_to_id.end()) return false;
        return true;
    }

    void send_to_con(std::string& key, const char* msg)
    {
        auto con = tsd::con_list[tsd::ip_to_id[key]];
        tsd::pack(con, msg);
    }

    /* ENDPOINTS */
    void lobby_count(
        sf::IpAddress& sender, unsigned short& port, engine* eng
    )
    {
        std::string key;
        if (!get_key(key, sender, port)) return;

        // send lobby list message: "reply lobby <lobby count>"
        if (eng->conf->lobbies) send_to_con(key, "rl" + std::to_string(eng->lobbies.size));

        // send (no lobbies) message: "reply lobby none"
        else send_to_con(key, "rl0");
    }

    void lobby_info(
        char* data, sf::IpAddress& sender, unsigned short& port, engine* eng
    )
    {
        std::string key;
        if (!get_key(key, sender, port)) return;
        
        // send lobby list message: "reply lobby details: [lobby details]" 
        if (eng->conf->lobbies) send_to_con(key, "rld:" + 0);

        // send (no lobbies) message: "reply lobby error: <error>"
        else send_to_con(key, "rle:this server has no lobbies");
    }

    void get_stack(
        char* data, sf::IpAddress& sender, unsigned short& port,
        sf::UdpSocket& socket
    )
    {
        std::string key;
        if (!get_key(key, sender, port)) return;

        // grab user connection and send message
        auto con = tsd::con_list[tsd::ip_to_id[key]];
        std::string msg = tsd::fetch(con, data[0]);
        const char* buff = msg.c_str();
        socket.send(buff, msg.size(), sender, port);
    }

    void lobby_join(
        char* data, sf::IpAddress& sender, unsigned short& port,
        engine* eng
    )
    {

    }

    void lobby_leave(
        sf::IpAddress& sender, unsigned short& port, engine* eng
    )
    {

    }

    void lobby_add(
        char* data, sf::IpAddress& sender, unsigned short& port,
        engine* eng
    )
    {

    }

    void lobby_rename(
        char* data, sf::IpAddress& sender, unsigned short& port,
        engine* eng
    )
    {

    }

    void lobby_delete(
        sf::IpAddress& sender, unsigned short& port, engine* eng
    )
    {

    }

    void lobby_owner(
        char* data, sf::IpAddress& sender, unsigned short& port,
        engine* eng
    )
    {

    }

    void match_relay(
        char* data, sf::IpAddress& sender, unsigned short& port,
        engine* eng
    )
    {

    }

    void disconnect(
        sf::IpAddress& sender, unsigned short& port, engine* eng
    )
    {

    }
}