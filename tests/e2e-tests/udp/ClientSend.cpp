/*
** EPITECH PROJECT, 2022
** SimpleHandshake.cpp
** File description:
** SimpleHandshake.cpp
*/

#include <gtest/gtest.h>
#include <thread>
#include "Polymorph/Network/udp/Server.hpp"
#include "Polymorph/Network/udp/Client.hpp"
#include "utils.hpp"

TEST(udpE2E, ClientSend)
{
    //checks
    std::uint16_t input_data = 42;
    std::uint16_t output_data = 0;

    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    std::map<OpId, bool> safeties = {
            { 2, true }
    };

    // Server Setup
    Server server(4242, safeties);
    auto serverConnector = std::make_shared<Connector>(server);
    server.setConnector(serverConnector);
    serverConnector->start();
    server.registerReceiveHandler<std::uint16_t>(2, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
    });

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

    PNL_WAIT_COND_LOOP(!connected, 5, PNL_TIME_OUT)
    ASSERT_TRUE(connected);
    client.send(2, input_data);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_data, output_data);
}

TEST(udpE2E, ClientSendCallback)
{
    //checks
    std::uint16_t input_data = 42;
    std::uint16_t output_data = 0;
    bool passed = false;

    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    std::map<OpId, bool> safeties = {
            { 2, true }
    };

    // Server Setup
    Server server(4242, safeties);
    auto serverConnector = std::make_shared<Connector>(server);
    server.setConnector(serverConnector);
    serverConnector->start();
    server.registerReceiveHandler<std::uint16_t>(2, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
    });

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

    PNL_WAIT_COND_LOOP(!connected, 5, PNL_TIME_OUT)
    ASSERT_TRUE(connected);
    client.send<std::uint16_t>(2, input_data, [&passed](const PacketHeader &header, const std::uint16_t &payload) {
        passed = true;
    });
    // server.sendTo(2, input_data, id); NOT WORKING LA PTN DE SA
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_data, output_data);
    ASSERT_TRUE(passed);
}