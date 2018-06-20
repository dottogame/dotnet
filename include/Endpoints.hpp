#pragma once

#include <SFML\Network.hpp>
#include <algorithm>

#include "Engine.hpp"
#include "Config.hpp"
#include "TwoStateDotocol.hpp"

namespace endpoint
{
    /* ENDPOINTS */
    void lobby_count(connection* con, config* conf)
    {
        // send lobby list message: "reply lobby <lobby count>"
        std::string resp = "rl" + std::to_string(lobbies::lobby_count);
        if (conf->lobbies) tsd::pack(con, resp);

        // send (no lobbies) message: "reply lobby none"
        else tsd::pack(con, "rl0");
    }

    void lobby_info(connection* con, config* conf)
    {
        // send lobby list message: "reply lobby details: [lobby details]"
        if (conf->lobbies) tsd::pack(con, "rld/" + 0);

        // send (no lobbies) message: "reply lobby error: <error>"
        else tsd::pack(con, "rle/this server has no lobbies");
    }

    void lobby_join(int lobby_id, connection* con)                                                             )
    {
        // add player to lobby (notifies others)
        lobbies::[lobby_id]->add(con->id);

        // inform player of success
        tsd::pack(con, "rlj/ok");
    }

    void lobby_leave(connection* con)
    {
        if (lobbies::remove()) tsd::pack(con, "rll/ok");
        else tsd::pack(con, "ell/you are not in a lobby");
    }

    void lobby_add(connection* con, config* conf)
    {
        if (!conf->lobbies)
        {
            tsd::pack(con, "eld/lobbies are disabled on this server");
            return;
        }

        lobbies::lobby* lob = new lobbies::lobby(con->id);
        tsd::pack(con, "rlj/ok");
    }

    void lobby_rename(char* data, connection* con)
    {
        if (lobbies::lobby_map.find(con->id) != lobbies::lobby_map.end())
        {
            // get name
            std::string dat(data);
            std::string name = dat.substr(3);

            // rename lobby
            if (lobbies::lobby_map[con->id]->owner == con->id)
            {
                lobbies::lobby_map[con->id]->rename(name);
                tsd::pack(con, "rlr/ok");
            }

            else tsd::pack(con, "elr/you are not the lobby owner");
        }

        else tsd::pack(con, "elr/you are not in a lobby");
    }

    void lobby_delete(connection* con, config* conf)
    {
        if (!conf->lobbies)
        {
            tsd::pack(con, "eld/lobbies are disabled on this server");
            return;
        }

        if (lobbies::lobby_map.find(con->id) == lobbies::lobby_map.end())
        {
            tsd::pack(con, "eld/you are not in a lobby");
            return;
        }

        if (lobbies::lobby_map[con->id]->owner == con->id)
        {
            tsd::pack(con, "eld/you are not the lobby owner");
            return;
        }

        // destroy the lobby
        lobbies::lobby_map[con->id]->destroy();

        // delete the lobby object
        delete &lobbies::lobby_map[con->id];

        // inform player of success
        tsd::pack(con, "rld/ok");
    }

    void lobby_owner(char* data, connection* con)
    {
        if (lobbies::lobby_map.find(con->id) == lobbies::lobby_map.end())
        {
            tsd::pack(con, "elo/you are not in a lobby");
            return;
        }

        lobby* lob = lobbies::lobby_map[con->id];
        if (lob->owner != con->id)
        {
            tsd::pack(con, "elo/you are not the lobby owner");
            return;
        }

        // shouldn't happen but save the power and return
        if (lob->owner == con->id) return;

        // get new owner
        std::string dat(data);
        std::string owner = dat.substr(3);

        if (lob->players.find(owner) == lob->players.end())
        {
            tsd::pack(con, "elo/you are not the lobby owner");
            return;
        }

        // set new owner (notifies others)
        lob->set_owner(owner);

        // inform player of success
        tsd::pack(con, "rlo/ok");
    }

    void match_relay(char* data, connection* con)
    {
        if (lobbies::lobby_map.find(con->id) == lobbies::lobby_map.end())
        {
            tsd::pack(con, "elo/you are not in a lobby");
            return;
        }

        std::string dat(data);
        std::string msg = "vlr/" + dat.substr(3);

        lobby* lob = lobbies::lobby_map[con->id];

        // inform all of new owner
        for (auto const& player_con_id : lob->players)
            tsd::pack(tsd::con_list[player_con_id], msg);

        tsd::pack(con, "rlo/ok");
    }

    void match_ghost_relay(char* data, connection* con)
    {
        if (lobbies::lobby_map.find(con->id) == lobbies::lobby_map.end()) return;

        std::string dat(data);
        std::string msg = "g/" + dat.substr(3);

        lobby* lob = lobbies::lobby_map[con->id];

        // relay to all
        for (auto const& player_con_id : lob->players)
            tsd::pack(tsd::con_list[player_con_id], msg);
    }

    void disconnect(connection* con, config* conf)
    {
        if (lobbies::lobby_map.find(con->id) == lobbies::lobby_map.end()) return;

        lobbies::lobby lob* = lobbies::lobby_map[con->id];
        lobbies::remove(con->id);
        if (conf->lobbies && lob->players.size() == 0)
        {
            // destroy empty lobby
            lob->destroy();

            // delete the lobby object
            delete &lobbies::lobby_map[con->id];
        }
    }
}
