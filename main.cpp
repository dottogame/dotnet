#include <SFML\Network.hpp>

#define PORT 46980
#define BUFF_SIZE 128

int main(int argc, char **argv)
{
    sf::UdpSocket socket;
    
    if (socket.bind(PORT) != sf::Socket::Done)
    {
        // TODO: Handle error
    }
    
    return 0;
}