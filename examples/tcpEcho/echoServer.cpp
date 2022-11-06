#include <iostream>
#include <future>
#include "polymorph/network/tcp/Server.hpp"
#include "MessageDto.hpp"


int main(int ac, char **av)
{
    using namespace polymorph::network;
    using namespace polymorph::network::tcp;

    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " <port>" << std::endl;
        return 1;
    }

    // we create a server and bind its connector
    auto server = Server::create(std::stoi(av[1]));

    // we register a callback that will handle the received MessageDto and send it back to the client
    server->registerReceiveHandler<MessageDto>(MessageDto::opId, [&server](const PacketHeader &header, const MessageDto &payload) {
        std::cout << "Received: " << payload.message << std::endl;
        MessageDto toResend = payload;
        server->sendTo<MessageDto>(MessageDto::opId, toResend, header.sId, [](const PacketHeader &header, const MessageDto &payload) {
            std::cout << "Message has been echoed" << std::endl;
        });
        return true; // The received data is correct, we do not want to disconnect the client
    });

    // we start the server
    server->start();

    // make the main thread sleep forever
    std::promise<void> p;
    p.get_future().wait();

    return 0;
}