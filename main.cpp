#include <SFML\Network.hpp>

#define BUFF_SIZE 128

#include "AuthKit.hpp"
#include "Config.hpp"
#include "Endpoints.hpp"
#include "Engine.hpp"

/**
 * take request and route to correct endpoint handle
*/
void route(
    char* data,
    sf::IpAddress& sender,
    unsigned short& port,
    sf::UdpSocket& socket,
    engine* eng
)
{
    if (data[0] == '!') socket.send("pong!", 5, sender, port);
    else if (data[0] == 'r')
    {
        // lobby access endpoints
        if (data[1] == 'l' && data[2] == 'c') endpoint::lobby_count(sender, port, eng);
        if (data[1] == 'l' && data[2] == 'i') endpoint::lobby_info(data, sender, port, eng);
        if (data[1] == 'l' && data[2] == 'j') endpoint::lobby_join(data, sender, port, eng);
        if (data[1] == 'l' && data[2] == 'l') endpoint::lobby_leave(sender, port, eng);

        // lobby control endpoints
        if (data[1] == 'l' && data[2] == 'a') endpoint::lobby_add(data, sender, port, eng);
        if (data[1] == 'l' && data[2] == 'r') endpoint::lobby_rename(data, sender, port, eng);
        if (data[1] == 'l' && data[2] == 'd') endpoint::lobby_delete(sender, port, eng);
        if (data[1] == 'l' && data[2] == 'o') endpoint::lobby_owner(data, sender, port, eng);

        // endpoint to relay data (make calls to other clients)
        if (data[1] == 'm' && data[2] == 'r') endpoint::match_relay(data, sender, port, eng);

        // connection control endpoints
        if (data[1] == 'd' && data[2] == 'c') endpoint::disconnect(sender, port, eng);
    }

    else endpoint::get_stack(data, sender, port, socket);
}

int main(int argc, char **argv)
{
    sf::UdpSocket socket;

    // variables needed
    char data[BUFF_SIZE];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short port;

    // init engine & load config
    engine* eng = new engine();
    eng->conf = new config("./config.json");

    // bind to port. exit if failed.
    int p = eng->conf->port;
    if (socket.bind(p) != sf::Socket::Done) return 1;

    // loop recieving packets
    while (eng->running)
    {
        // recieve message and die if an error occurs
        if (socket.receive(
            data, BUFF_SIZE, received, sender, port
        ) != sf::Socket::Done) return 1;

        // if no prefix, check auth and make connection
        if (data[0] == '-') authkit::check(data, sender, port, socket);

        // else handle request
        else route(data, sender, port, socket, eng);
    }

    return 0;
}