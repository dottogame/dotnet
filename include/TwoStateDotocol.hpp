#pragma once

#include<winsock2.h>
#include <string>
#include <sstream>

#include "sparsepp\spp.h"

namespace tsd
{
    struct connection
    {
    public:
        char pack_state;

        std::stringstream curr_packs;

        std::string ip;
        std::string id;
        std::string packed;
    };

    spp::sparse_hash_map<std::string, connection*> con_list;
    spp::sparse_hash_map<std::string, std::string> ip_to_id;

    void flip(connection* con)
    {
        if (con->pack_state == 'x') con->pack_state = 'y';
        else con->pack_state = 'x';
    }

    void pack(connection* con, std::string msg)
    {
        con->curr_packs << msg << ";";
    }

    connection* construct(std::string id, sockaddr_in* client)
    {
        connection* con = new connection();
        std::string ip(inet_ntoa(client->sin_addr));

        con->pack_state = 'x';
        con->ip = ip;
        con->curr_packs << con->pack_state;

        // add a "auth accepted message to the pack"
        // protocol notation: rpa = Reply | Auth | Accepted (: end of pack)
        con->curr_packs << ":raa";

        con_list.insert(std::make_pair(id, con));
        ip_to_id.insert(std::make_pair(ip, id));
        return con;
    }

    std::string fetch(connection* con, const char pack_state)
    {
        // check if we're sending new stuff or old stuff
        if (pack_state == con->pack_state)
        {
            // serialize current packs and store 'em
            con->packed = con->curr_packs.str();

            // flip states
            flip(con);

            // reset our current pack (and intialize it with new state id)
            con->curr_packs.str(std::string());
            con->curr_packs << con->pack_state;
        }

        // requested old stuff
        return con->packed;
    }
}
