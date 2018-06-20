#pragma once

#include <SFML\Network.hpp>
#include <algorithm>

#include "Config.hpp"
#include "TwoStateDotocol.hpp"
#include "Lobby.hpp"

namespace endpoint
{
    /* ENDPOINTS */
    void lobby_count(tsd::connection* con, config* conf)
    {
        // send lobby list message: "reply lobby <lobby count>"
        std::string resp = "rl" + std::to_string(lobbies::lobby_list.size());
        if (conf->lobbies) tsd::pack(con, resp);

        // send (no lobbies) message: "reply lobby none"
        else tsd::pack(con, "rl0");
    }

    void lobby_info(tsd::connection* con, config* conf)
    {
        // send lobby list message: "reply lobby details: [lobby details]"
        if (conf->lobbies) tsd::pack(con, "rld/" + 0);

        // send (no lobbies) message: "reply lobby error: <error>"
        else tsd::pack(con, "rle/this server has no lobbies");
    }

    void lobby_join(tsd::connection* con, int lobby_id)
    {
        // add player to lobby (notifies others)
        lobbies::add(lobbies::lobby_list[lobby_id], con->id);

        // inform player of success
        tsd::pack(con, "rlj/ok");
    }

    void lobby_leave(tsd::connection* con)
    {
        if (lobbies::remove(con->id)) tsd::pack(con, "rll/ok");
        else tsd::pack(con, "ell/you are not in a lobby");
    }

    void lobby_add(tsd::connection* con, config* conf)
    {
        if (!conf->lobbies)
        {
            tsd::pack(con, "eld/lobbies are disabled on this server");
            return;
        }

        lobbies::lobby* lob = new lobbies::lobby();
        lob->owner = con->id;
        lobbies::add(lob, con->id);
        lobbies::lobby_list.push_back(lob);

        tsd::pack(con, "rlj/ok");
    }

    void lobby_rename(tsd::connection* con, char* data)
    {
        if (lobbies::lobby_map.find(con->id) != lobbies::lobby_map.end())
        {
            // get name
            std::string dat(data);
            std::string name = dat.substr(3);

            // rename lobby
            if (lobbies::lobby_map[con->id]->owner == con->id)
            {
                lobbies::rename(lobbies::lobby_map[con->id], name);
                tsd::pack(con, "rlr/ok");
            }

            else tsd::pack(con, "elr/you are not the lobby owner");
        }

        else tsd::pack(con, "elr/you are not in a lobby");
    }

    void lobby_delete(tsd::connection* con, config* conf)
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
        lobbies::destroy(lobbies::lobby_map[con->id]);

        // TODO delete lobby object?

        // inform player of success
        tsd::pack(con, "rld/ok");
    }

    void lobby_owner(tsd::connection* con, char* data)
    {
        if (lobbies::lobby_map.find(con->id) == lobbies::lobby_map.end())
        {
            tsd::pack(con, "elo/you are not in a lobby");
            return;
        }

        lobbies::lobby* lob = lobbies::lobby_map[con->id];
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

        auto it = std::find(
            std::begin(lob->players_ids),
            std::end(lob->players_ids),
            con->id
        );

        if (it == lob->players_ids.end())
        {
            tsd::pack(con, "elo/you are not the lobby owner");
            return;
        }

        // set new owner (notifies others)
        lobbies::set_owner(lob, owner);

        // inform player of success
        tsd::pack(con, "rlo/ok");
    }

    void match_relay(tsd::connection* con, char* data)
    {
        if (lobbies::lobby_map.find(con->id) == lobbies::lobby_map.end())
        {
            tsd::pack(con, "elo/you are not in a lobby");
            return;
        }

        std::string dat(data);
        std::string msg = "vlr/" + dat.substr(3);

        lobbies::lobby* lob = lobbies::lobby_map[con->id];

        // relay message
        for (auto const& player_con_id : lob->players_ids)
            tsd::pack(tsd::con_list[player_con_id], msg);

        tsd::pack(con, "rlo/ok");
    }

    void match_ghost_relay(tsd::connection* con, char* data)
    {
        if (lobbies::lobby_map.find(con->id) == lobbies::lobby_map.end()) return;

        std::string dat(data);
        std::string msg = "vgr/" + dat.substr(3);

        lobbies::lobby* lob = lobbies::lobby_map[con->id];

        // relay to all
        for (auto const& player_con_id : lob->players_ids)
            tsd::pack(tsd::con_list[player_con_id], msg);
    }

    void disconnect(tsd::connection* con, config* conf)
    {
        if (lobbies::lobby_map.find(con->id) == lobbies::lobby_map.end()) return;

        lobbies::lobby* lob = lobbies::lobby_map[con->id];
        lobbies::remove(con->id);
        if (conf->lobbies && lob->players_ids.size() == 0)
        {
            // destroy empty lobby
            lobbies::destroy(lob);

            // TODO delete lobby object?
        }
    }
}
