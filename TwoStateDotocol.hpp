#pragma once

#include "sparsepp\spp.h"
#include <string>
#include <sstream>

class TwoStateDotocol
{
public:
    struct Connection
    {
    public:
        char* id;

        char pack_state;

        std::stringstream curr_packs;

        std::string prev_packs;
        std::string party_id;
    };

    spp::sparse_hash_map<char*, Connection*> con_list;
    spp::sparse_hash_map<char*, char*> ip_to_id;

    void flip(Connection* con)
    {
        if (con->pack_state == 'x') con->pack_state = 'y';
        else con->pack_state = 'x';
    }

    void pack(Connection* con, std::string msg)
    {
        con->curr_packs << msg << ";";
    }

    Connection* construct(char* id)
    {
        Connection* con = new Connection();
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

    std::string fetch(Connection* con, const char pack_state)
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

