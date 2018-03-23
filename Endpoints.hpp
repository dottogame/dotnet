#pragma once

#include <SFML\Network.hpp>
#include <algorithm>

#include "Engine.hpp"
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
        std::string resp = "rl" + std::to_string(eng->lobbies.size());
        if (eng->conf->lobbies) send_to_con(key, resp.c_str());

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
        else send_to_con(key, "rle/this server has no lobbies");
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
        std::string key;
        if (!get_key(key, sender, port)) return;
        
        auto con = tsd::con_list[tsd::ip_to_id[key]];

        con->lob = eng->lobbies[(int) data[3]];
        eng->lobbies[(int)data[3]]->players.push_back(key);

        // inform all of join
        std::string msg = "elj/" + key;
        for (auto const& key : eng->lobbies[(int)data[3]]->players)
            tsd::pack(tsd::con_list[tsd::ip_to_id[key]], msg);

        tsd::pack(con, "rlj/ok");
    }

    void lobby_leave(
        sf::IpAddress& sender, unsigned short& port, engine* eng
    )
    {
        std::string key;
        if (!get_key(key, sender, port)) return;

        auto con = tsd::con_list[tsd::ip_to_id[key]];
        
        // inform all of leave
        std::string msg = "ell/";
        msg += con->id;
        for (auto const& key : con->lob->players)
            tsd::pack(tsd::con_list[tsd::ip_to_id[key]], msg);

        // find player in lobby and remove
        auto it = std::find(
            std::begin(con->lob->players),
            std::end(con->lob->players),
            key
        );

        if (it != std::end(con->lob->players))
            con->lob->players.erase(it);

        // delete pointer
        con->lob = NULL;

        tsd::pack(con, "rll/ok");
    }

    void lobby_add(
        char* data, sf::IpAddress& sender, unsigned short& port,
        engine* eng
    )
    {
        std::string key;
        if (!get_key(key, sender, port)) return;

        auto con = tsd::con_list[tsd::ip_to_id[key]];

        con->lob = new lobby(key);
        eng->lobbies.push_back(con->lob);
        tsd::pack(con, "rlj/ok");
    }

    void lobby_rename(
        char* data, sf::IpAddress& sender, unsigned short& port,
        engine* eng
    )
    {
        std::string key;
        if (!get_key(key, sender, port)) return;

        auto con = tsd::con_list[tsd::ip_to_id[key]];

        // get name
        std::string dat(data);
        std::string msg = "elr/" + dat.substr(3);

        // inform all of rename
        for (auto const& key : con->lob->players)
            tsd::pack(tsd::con_list[tsd::ip_to_id[key]], msg);

        tsd::pack(con, "rlr/ok");
    }

    void lobby_delete(
        sf::IpAddress& sender, unsigned short& port, engine* eng
    )
    {
        std::string key;
        if (!get_key(key, sender, port)) return;

        auto con = tsd::con_list[tsd::ip_to_id[key]];

        // delete lobby instance
        auto it = std::find(
            std::begin(eng->lobbies),
            std::end(eng->lobbies),
            con->lob
        );

        if (it != std::end(eng->lobbies))
            eng->lobbies.erase(it);

        // remove lobby pointer
        con->lob = NULL;

        // inform all of deletion
        std::string msg = "eld";
        for (auto const& key : con->lob->players)
        {
            auto conn = tsd::con_list[tsd::ip_to_id[key]];
            tsd::pack(conn, msg);

            // delete their lobby instance
            conn->lob = NULL;
        }

        tsd::pack(con, "rld/ok");
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

    void match_ghost_relay(
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