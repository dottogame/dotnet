#pragma once

#include <string>
#include <vector>

namespace lobbies {

    int lobby_count = 0;

    spp::sparse_hash_map<std::string, lobby*> lobby_map;

    bool remove(std::string player_id)
    {
        if (lobby_map.find(player_id) != lobby_map.end())
        {
            lobby* lob = lobby_map[player_id];

            // assign a new owner if they were the owner
            if (lob->owner == player_id)
                lob->set_owner(lob->players[0]);

            // find player in lobby
            auto it = std::find(
                std::begin(lob->players),
                std::end(lob->players),
                player_id
            );

            // remove player (check if they're there for the sake of it)
            if (it != std::end(lob->players))
            {
                con->lob->players.erase(it);

                // delete the player's lobby map entry
                lobby_map.erase(player_id);

                // inform all of leave
                std::string msg = "vll/" + player_id;
                for (auto const& other_player_id : lob->players)
                    tsd::pack(tsd::con_list[other_player_id], msg);
            }

            return true;
        }

        else return false;
    }

    class lobby
    {
    public:
        std::string name = "LAN Play";

        std::vector<std::string> players_ids;

        std::string owner;

        lobby(std::string creator_id)
        {
            lobby_count++;
            add(creator_id);
            owner = creator_id;
        }

        void add(std::string player_id)
        {
            // remove them from their current lobby first
            if (lobby_map.find(player_id) != lobby_map.end())
                remove(player_id);

            // add player to players
            players.push_back(player_id);

            lobby_map.insert(std::pair<std::string, lobby*>(player_id, &this));

            // inform other players of join
            std::string msg = "vlj/" + con->id;
            for (auto const& player_con_id : players)
                tsd::pack(tsd::con_list[player_con_id], msg);
        }

        void rename(std::string new_name)
        {
            name = new_name;

            // inform all of rename
            std::string msg = "vlr/" + name;
            for (auto const& player_con_id : players)
                tsd::pack(tsd::con_list[player_con_id], msg);
        }

        void destroy()
        {
            lobby_count--;
            for (auto const& player_con_id : players)
            {
                lobby_map.erase(player_con_id);
                tsd::pack(tsd::con_list[player_con_id], "vld");
            }
        }

        void set_owner(std::string player_id)
        {
            // set new owner
            owner = player_id;

            // inform all players of new owner
            for (auto const& player_con_id : lob->players)
                tsd::pack(tsd::con_list[player_con_id], "vlo/" + player_id);
        }
    };
}
