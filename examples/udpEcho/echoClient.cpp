#include <iostream>
#include "Polymorph/Network/udp/Client.hpp"
#include "MessageDto.hpp"


int main(int ac, char **av)
{
    using namespace polymorph::network;
    using namespace polymorph::network::udp;

    if (ac != 3) {
        std::cerr << "Usage: " << av[0] << " <host> <port>" << std::endl;
        return 1;
    }

    std::map<OpId, bool> safeties = {
        { MessageDto::opId, true } // we want the client to resend the message if it doesn't receive an ACK
    };
    // we create a client and bind its connector
    Client client(av[1], std::stoi(av[2]), safeties);
    auto connector = std::make_shared<Connector>(client);
    client.setConnector(connector);
    connector->start();

    // check variables for the example
    std::atomic<bool> received(false);
    std::atomic<bool> connected(false);
    MessageDto dto { .message = "Hello World!" };

    // we register a callback that will handle the received MessageDto
    client.registerReceiveHandler<MessageDto>(MessageDto::opId, [&received](const PacketHeader &, const MessageDto &payload) {
        received = true;
        std::cout << "Received: " << payload.message << std::endl;
    });

    // Then we connect to the server and pass a callback that will be called when the connection is established
    client.connect([&connected](bool authorized, SessionId) {
        if (authorized) {
            std::cout << "Connected" << std::endl;
            connected = true;
        } else {
            std::cout << "Connection failed" << std::endl;
        }
    });

    // we wait for the connection to be established or the timeout to be reached (5 * 100ms)
    int connected_max_tries = 5;
    while (!connected && --connected_max_tries > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    assert(connected); // abort if we couldn't connect

    // we send the MessageDto to the server
    client.send<MessageDto>(MessageDto::opId, dto, [](const PacketHeader &header, const MessageDto &payload) {
        std::cout << "Message has been sent" << std::endl;
    });

    // we wait the registered callback to be called
    while(!received)
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // sleep until we received the echoed message

    return 0;
}