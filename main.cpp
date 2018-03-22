#include <SFML\Network.hpp>

#define BUFF_SIZE 128

#include "AuthKit.hpp"
#include "Config.hpp"
#include "Endpoints.hpp"

// globals
bool running = true;

/**
 * take request and route to correct endpoint handle
*/
void route(
    char* data,
    sf::IpAddress& sender,
    unsigned short& port,
    sf::UdpSocket& socket,
    config* configuration
)
{
    if (data[0] == '!') endpoint::ping(sender, port, socket);
    else if (data[0] == 'r')
    {
        if (data[1] == 'l' && data[2] == 'c') endpoint::lobby_count(sender, port, configuration);
        if (data[1] == 'l' && data[2] == 'd') endpoint::lobby_details(data, sender, port, configuration);
    }

    else endpoint::get_stack(data, sender, port, socket);
}

int main(int argc, char **argv)
{
    sf::UdpSocket socket;
    config* configuration;

    // variables needed
    char data[BUFF_SIZE];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short port;

    // load config
    configuration = new config("./config.json");

    // bind to port. exit if failed.
    if (socket.bind(configuration->port) != sf::Socket::Done)
        return 1;

    // loop recieving packets
    while (running)
    {
        // recieve message and die if an error occurs
        if (socket.receive(
            data, BUFF_SIZE, received, sender, port
        ) != sf::Socket::Done) return 1;

        // if no prefix, check auth and make connection
        if (data[0] == '-') authkit::check(data, sender, port);

        // else handle request
        else route(data, sender, port, socket, configuration);
    }

    return 0;
}