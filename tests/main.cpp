#include <iostream>
#include <functional>
#include <any>
#include <map>
#include "polymorph/network/udp/Client.hpp"
#include "polymorph/network/udp/Server.hpp"
#include "polymorph/network/udp/AConnector.hpp"
#include "e2e-tests/utils.hpp"


int main(){
    //checks
    std::uint16_t input_data = 42;
    std::uint8_t input_char = 'A';
    std::uint16_t output_data = 0;
    std::uint8_t output_char = ' ';

    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    std::map<OpId, bool> safeties = {
            { 2, true },
            { 3, true }
    };

    // Server Setup
    Server server(4242, safeties);
    auto serverConnector = std::make_shared<AConnector>(server);
    server.setConnector(serverConnector);
    serverConnector->start();

    // Client Setup
    Client client("127.0.0.1", 4242, safeties);
    auto clientConnector = std::make_shared<AConnector>(client);
    client.setConnector(clientConnector);
    clientConnector->start();
    client.registerReceiveHandler<std::uint16_t>(2, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
    });
    client.registerReceiveHandler<std::uint8_t>(3, [&output_char](const PacketHeader &, std::uint8_t payload) {
        output_char = payload;
    });

    // Client Infos
    SessionId id;
    bool connected = false;

    client.connect([&id, &connected](bool authorized, SessionId sId) {
        connected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!connected, 5, PNL_TIME_OUT)
    server.send(3, input_char);
    server.send(2, input_data);
    PNL_WAIT(PNL_TIME_OUT)
    return 0;
}