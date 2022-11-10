/*
** EPITECH PROJECT, 2022
** SimpleHandshake.cpp
** File description:
** SimpleHandshake.cpp
*/

#include <gtest/gtest.h>
#include <thread>
#include "polymorph/network/udp/Server.hpp"
#include "polymorph/network/udp/Client.hpp"
#include "../utils.hpp"

TEST(udpE2E, ServerPrematureShutdown)
{
    //checks
    std::uint16_t input_data = 42;
    std::uint16_t output_data = 0;

    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    std::map<OpId, bool> safeties = {
            { 10, true }
    };

    std::unique_ptr<Client> client;

    {
        // Server Setup
        auto server = Server::create(4242, safeties);
        server->start();
        server->registerReceiveHandler<std::uint16_t>(10, [&output_data](const PacketHeader &, uint16_t payload) {
            output_data = payload;
            return true;
        });
        // Client Setup
        client = Client::create("127.0.0.1", 4242, safeties);

        // Client Infos
        SessionId id;
        bool connected = false;

        client->connect([&id, &connected](bool authorized, SessionId sId) {
            connected = authorized;
            id = sId;
        });

        PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
        ASSERT_TRUE(connected);
    }
    client->send(10, input_data);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_NE(input_data, output_data);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

#ifdef PNL_CLIENT_TEST
#if PNL_CLIENT_TEST == 1
TEST(udpE2E, SafetyClientSend)
{
    //checks
    std::uint32_t input_data = 4294967295;
    std::uint32_t output_data = 0;

    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    std::map<OpId, bool> safeties = {
            { 2, false }
    };

    // Client Setup
    Client client("127.0.0.1", 4242, safeties);
    auto clientConnector = std::make_shared<Connector>(client);
    client.setConnector(clientConnector);
    clientConnector->start();

    // Client Infos
    SessionId id;
    bool connected = false;

    client.connect([&id, &connected](bool authorized, SessionId sId) {
        connected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected);
    client.send(2, input_data);
    PNL_WAIT_COND(input_data != output_data, PNL_TIME_OUT)
    ASSERT_EQ(input_data, output_data);
}
#endif
#endif