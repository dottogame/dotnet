#include <SFML\Network.hpp>

#define PORT 46980
#define BUFF_SIZE 128

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

    while (running) {
        // lol for now we just die if an error occurs
        if (socket.receive(data, BUFF_SIZE, received, sender, port) != sf::Socket::Done) return 1;

    }

    return 0;
}