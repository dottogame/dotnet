#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "TwoStateDotocol.hpp"

namespace lobbies {
    class lobby
    {
    public:
        std::vector<std::string> players_ids;

        std::string name = "LAN Play";
        std::string owner;
    };

    spp::sparse_hash_map<std::string, lobby*> lobby_map;
    std::vector<lobby*> lobby_list;

    void rename(lobby* lob, std::string new_name)
    {
        lob->name = new_name;

        // inform all of rename
        std::string msg = "vlr/" + new_name;
        for (auto const& player_con_id : lob->players_ids)
            tsd::pack(tsd::con_list[player_con_id], msg);
    }

    void destroy(lobby* lob)
    {
        lobby_list.erase(
            std::remove(
                lobby_list.begin(),
                lobby_list.end(),
                lob
            ),
            lobby_list.end()
        );

        for (auto const& player_con_id : lob->players_ids)
        {
            lobby_map.erase(player_con_id);
            tsd::pack(tsd::con_list[player_con_id], "vld");
        }
    }

    void set_owner(lobby* lob, std::string player_id)
    {
        // set new owner
        lob->owner = player_id;

        // inform all players of new owner
        for (auto const& player_con_id : lob->players_ids)
            tsd::pack(tsd::con_list[player_con_id], "vlo/" + player_id);
    }

    bool remove(std::string player_id)
    {
        if (lobby_map.find(player_id) != lobby_map.end())
        {
            lobby* lob = lobby_map[player_id];

            // assign a new owner if they were the owner
            if (lob->owner == player_id)
                set_owner(lob, lob->players_ids[0]);

            // find player in lobby
            auto it = std::find(
                std::begin(lob->players_ids),
                std::end(lob->players_ids),
                player_id
            );

            // remove player (check if they're there for the sake of checking)
            if (it != std::end(lob->players_ids))
            {
                lob->players_ids.erase(it);

                // delete the player's lobby map entry
                lobby_map.erase(player_id);

                // inform all of leave
                std::string msg = "vll/" + player_id;
                for (auto const& other_player_id : lob->players_ids)
                    tsd::pack(tsd::con_list[other_player_id], msg);
            }

            return true;
        }

        return false;
    }

    void add(lobby* lob, std::string player_id)
    {
        // remove them from their current lobby first
        if (lobby_map.find(player_id) != lobby_map.end())
            remove(player_id);

        // add player to players
        lob->players_ids.push_back(player_id);

        lobby_map.insert(std::pair<std::string, lobby*>(player_id, lob));

        // inform other players of join
        std::string msg = "vlj/" + player_id;
        for (auto const& player_con_id : lob->players_ids)
            tsd::pack(tsd::con_list[player_con_id], msg);
    }
}
