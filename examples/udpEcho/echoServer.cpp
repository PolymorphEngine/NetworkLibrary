#include <iostream>
#include <future>
#include "Polymorph/Network/udp/Server.hpp"
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
    // we create a SessionStore that will attribute the sessions
    SessionStore sessionStore;
    // we create a server and bind its connector
    Server server(std::stoi(av[1]), safeties, sessionStore);
    auto connector = std::make_shared<Connector>(server);
    server.setConnector(connector);

    // we register a callback that will handle the received MessageDto and send it back to the client
    server.registerReceiveHandler<MessageDto>(MessageDto::opId, [&server](const PacketHeader &header, const MessageDto &payload) {
        std::cout << "Received: " << payload.message << std::endl;
        MessageDto toResend = payload;
        server.sendTo<MessageDto>(MessageDto::opId, toResend, header.sId, [](const PacketHeader &header, const MessageDto &payload) {
            std::cout << "Message has been echoed" << std::endl;
        });
    });

    // we start the server
    connector->start();

    // make the main thread sleep forever
    std::promise<void> p;
    p.get_future().wait();

    return 0;
}