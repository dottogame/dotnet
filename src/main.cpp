#define BUFF_SIZE 128

#include "leapsocket/leapwin.hpp"

#include "AuthKit.hpp"
#include "Config.hpp"
#include "Endpoints.hpp"

leapwin* leap;
config* conf;

bool running = true;

/**
 * take request and route to correct endpoint handle
*/
void pack_handle()
{
    if (leap->buf[0] == '!') leap->send("pong!", leap->si_other);
    else if (leap->buf[0] == '-') authkit::check(leap->buf, leap->si_other, leap);
    else
    {
        // get ip
        std::string ip(inet_ntoa(leap->si_other.sin_addr));

        // get connection
        tsd::connection* con;
        if (tsd::ip_to_id.find(ip) != tsd::ip_to_id.end())
            con = tsd::con_list[ip];

        // ignore if no connection found
        else return;

        // handle call with correct endpoint
        if (leap->buf[0] == 'r')
        {
            // lobby access endpoints
            if (leap->buf[1] == 'l' && leap->buf[2] == 'c') endpoint::lobby_count(con, conf);
            if (leap->buf[1] == 'l' && leap->buf[2] == 'i') endpoint::lobby_info(con, conf);
            if (leap->buf[1] == 'l' && leap->buf[2] == 'j') endpoint::lobby_join(con, (int) leap->buf[3]);
            if (leap->buf[1] == 'l' && leap->buf[2] == 'l') endpoint::lobby_leave(con);

            // lobby control endpoints
            if (leap->buf[1] == 'l' && leap->buf[2] == 'a') endpoint::lobby_add(con, conf);
            if (leap->buf[1] == 'l' && leap->buf[2] == 'r') endpoint::lobby_rename(con, leap->buf);
            if (leap->buf[1] == 'l' && leap->buf[2] == 'd') endpoint::lobby_delete(con, conf);
            if (leap->buf[1] == 'l' && leap->buf[2] == 'o') endpoint::lobby_owner(con, leap->buf);

            // endpoint to relay data (make calls to other clients)
            if (leap->buf[1] == 'm' && leap->buf[2] == 'r') endpoint::match_relay(con, leap->buf);
            if (leap->buf[1] == 'm' && leap->buf[2] == 'g') endpoint::match_ghost_relay(con, leap->buf);

            // connection control endpoints
            if (leap->buf[1] == 'd' && leap->buf[2] == 'c') endpoint::disconnect(con, conf);
        }

        else leap->send(tsd::fetch(con, leap->buf[0]).c_str(), leap->si_other);
    }
}


void err_handle(int err_code)
{
    exit(err_code);
}

int main()
{
    // load config
    conf = new config("./config.json");

    // create udp socket
    leap = new leapwin(8888, pack_handle, err_handle);

    // loop handling incoming messages
    while (running) leap->fetch();

    // close and terminate
    leap->close();
    lobbies::lobby_map.clear();
    return 0;
}
