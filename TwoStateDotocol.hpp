#pragma once

#include "sparsepp\spp.h"
#include <string>
#include <sstream>

namespace tsd
{
    struct connection
    {
    public:
        char* id;

        char pack_state;

        std::stringstream curr_packs;

        std::string prev_packs;
        std::string party_id;
    };

    spp::sparse_hash_map<char*, connection*> con_list;
    spp::sparse_hash_map<std::string, char*> ip_to_id;

    void flip(connection* con)
    {
        if (con->pack_state == 'x') con->pack_state = 'y';
        else con->pack_state = 'x';
    }

    void pack(connection* con, std::string msg)
    {
        con->curr_packs << msg << ";";
    }

    connection* construct(char* id)
    {
        connection* con = new connection();
        // init the connection and start a pack
        con->id = id;
        con->pack_state = 'x';
        con->curr_packs << con->pack_state;

        // add a "auth accepted message to the pack"
        // protocol notation: rpa = Reply | Auth | Accepted (: end of pack)
        con->curr_packs << ":raa";

        con_list.insert(std::make_pair(id, con));
        // TODO store in IP list
        return con;
    }

    std::string fetch(connection* con, const char pack_state)
    {
        // check if we're sending new stuff or old stuff
        if (pack_state == con->pack_state)
        {
            // serialize current packs and store 'em
            con->prev_packs = con->curr_packs.str();

            // flip states
            flip(con);

            // reset our current pack (and intialize it with new state id)
            con->curr_packs.str(std::string());
            con->curr_packs << con->pack_state;
        }

        // if we requested new stuff, they'd be in prev_packs now
        return con->prev_packs;
    }
};

