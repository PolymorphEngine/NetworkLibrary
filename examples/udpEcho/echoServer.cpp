#include <iostream>
#include <future>
#include "polymorph/network/udp/Server.hpp"
#include "MessageDto.hpp"


int main(int ac, char **av)
{
    using namespace polymorph::network;
    using namespace polymorph::network::udp;

    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " <port>" << std::endl;
        return 1;
    }

    std::map<OpId, bool> safeties = {
        { MessageDto::opId, true } // we want the server to resend the message if it doesn't receive an ACK
    };
    auto server = Server::create(std::stoi(av[1]), safeties);

    // we register a callback that will handle the received MessageDto and send it back to the client
    server->registerReceiveHandler<MessageDto>(MessageDto::opId, [&server](const PacketHeader &header, const MessageDto &payload) {
        std::cout << "Received: " << payload.message << std::endl;
        MessageDto toResend = payload;
        server->sendTo<MessageDto>(MessageDto::opId, toResend, header.sId, [](const PacketHeader &header, const MessageDto &payload) {
            std::cout << "Message has been echoed" << std::endl;
        });
    });

    // we start the server
    server->start();

    // make the main thread sleep forever
    std::promise<void> p;
    p.get_future().wait();

    return 0;
}