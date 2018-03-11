#include <SFML\Network.hpp>
#include <iostream>

#include "AuthKit.hpp"
#include "TwoStateDotocol.hpp"

#define PORT 46980
#define BUFF_SIZE 128

/**
    handles incoming requests and sends responses
*/
void process(char* data, sf::IpAddress& sender, unsigned short& port)
{

}

int main(int argc, char **argv)
{
    bool running = true;
    sf::UdpSocket socket;
    
    if (socket.bind(PORT) != sf::Socket::Done)
    {
        // TODO: Handle error
    }
    
    char data[BUFF_SIZE];
    std::size_t received;
    sf::IpAddress sender;
    unsigned short port;

    while (running)
    {
        // lol for now we just die if an error occurs
        if (socket.receive(data, BUFF_SIZE, received, sender, port) != sf::Socket::Done) return 1;
        if (data[0] == '-') authkit::check(data, sender, port); // TODO check auth and make connection
        else process(data, sender, port);
    }

    return 0;
}