#include <SFML\Network.hpp>
#include <iostream>

#include "AuthKit.hpp"
#include "TwoStateDotocol.hpp"
#include "Config.hpp"

#define PORT 46980
#define BUFF_SIZE 128

// globals
bool running = true;

sf::UdpSocket socket;

config* main_config;

/**
    handles incoming requests and sends responses
*/
void process(char* data, sf::IpAddress& sender, unsigned short& port)
{
    main_config = new config("./config.json");

    // check if packet is a request
    if (data[0] == 'r')
    {

    }
    // client is requesting response stack
    else
    {
        // the key to get an id is the ip and port concatenated
        std::string key = sender.toString() + std::to_string(port);

        // check if we have the id for this ip. ignore by returning if not
        auto it = tsd::ip_to_id.find(key);
        if (it == tsd::ip_to_id.end()) return;

        // grab user connection and send message
        auto con = tsd::con_list[tsd::ip_to_id[key]];
        auto msg = tsd::fetch(con, data[0]);
        const char* buff = msg.c_str();
        socket.send(buff, msg.size(), sender, port);
    }
}

int main(int argc, char **argv)
{    
    if (socket.bind(PORT) != sf::Socket::Done)
    {
        // TODO: Handle error
        return 1;
    }
    
    char data[BUFF_SIZE];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short port;

    // loop recieving packets
    while (running)
    {
        // recieve message and die if an error occurs
        if (socket.receive(data, BUFF_SIZE, received, sender, port) != sf::Socket::Done) return 1;

        // if no prefix, check auth and make connection
        if (data[0] == '-') authkit::check(data, sender, port);

        // else process request
        else process(data, sender, port);
    }

    return 0;
}