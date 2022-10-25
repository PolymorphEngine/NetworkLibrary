/*
** EPITECH PROJECT, 2022
** ServerSend.cpp
** File description:
** ServerSend.cpp
*/

#include <gtest/gtest.h>
#include <thread>
#include "Polymorph/Network/udp/Server.hpp"
#include "Polymorph/Network/udp/Client.hpp"
#include "utils.hpp"

TEST(udpE2E, ServerSend)
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

    // Client Setup
    Client client("127.0.0.1", 4242, safeties);
    auto clientConnector = std::make_shared<Connector>(client);
    client.setConnector(clientConnector);
    clientConnector->start();
    client.registerReceiveHandler<std::uint16_t>(2, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
    });

    // Client Infos
    SessionId id;
    bool connected = false;

    client.connect([&id, &connected](bool authorized, SessionId sId) {
        connected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected);
    server.send(2, input_data);
    // server.sendTo(2, input_data, id); NOT WORKING LA PTN DE SA
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_data, output_data);
}

TEST(udpE2E, OpIdDispatchServerSend)
{
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
    auto serverConnector = std::make_shared<Connector>(server);
    server.setConnector(serverConnector);
    serverConnector->start();

    // Client Setup
    Client client("127.0.0.1", 4242, safeties);
    auto clientConnector = std::make_shared<Connector>(client);
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

    PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected);
    server.send(3, input_char);
    server.send(2, input_data);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_char, output_char);
    ASSERT_EQ(input_data, output_data);
}

TEST(udpE2E, ServerDispatchToAllClients)
{
    //checks
    bool client1Passed = false;
    bool client2Passed = false;
    std::uint16_t checkPayload = 42;

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

    // Client1 Setup
    Client client1("127.0.0.1", 4242, safeties);
    auto client1Connector = std::make_shared<Connector>(client1);
    client1.setConnector(client1Connector);
    client1Connector->start();
    client1.registerReceiveHandler<std::uint16_t>(2, [&client1Passed, &checkPayload](const PacketHeader &, uint16_t payload) {
        ASSERT_EQ(payload, checkPayload);
        client1Passed = true;
    });

    // Client2 Setup
    Client client2("127.0.0.1", 4242, safeties);
    auto client2Connector = std::make_shared<Connector>(client2);
    client2.setConnector(client2Connector);
    client2Connector->start();
    client2.registerReceiveHandler<std::uint16_t>(2, [&client2Passed, &checkPayload](const PacketHeader &, uint16_t payload) {
        ASSERT_EQ(payload, checkPayload);
        client2Passed = true;
    });

    // Client Infos
    std::atomic<SessionId> id1;
    std::atomic<SessionId> id2;
    std::atomic<bool> connected1 = false;
    std::atomic<bool> connected2 = false;

    client1.connect([&id1, &connected1](bool authorized, SessionId sId) {
        connected1 = authorized;
        id1 = sId;
    });

    client2.connect([&id2, &connected2](bool authorized, SessionId sId) {
        connected2 = authorized;
        id2 = sId;
    });

    PNL_WAIT_COND_LOOP(!connected1 || !connected2, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected1);
    ASSERT_TRUE(connected2);
    server.send(2, checkPayload);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_TRUE(client1Passed);
    ASSERT_TRUE(client2Passed);
}

TEST(udpE2E, ServerSendOnlyOneClient)
{
    //checks
    bool client1Passed = false;
    bool client2Passed = true;
    std::uint16_t checkPayload = 42;

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

    // Client1 Setup
    Client client1("127.0.0.1", 4242, safeties);
    auto client1Connector = std::make_shared<Connector>(client1);
    client1.setConnector(client1Connector);
    client1Connector->start();
    client1.registerReceiveHandler<std::uint16_t>(2, [&client1Passed, &checkPayload](const PacketHeader &, uint16_t payload) {
        ASSERT_EQ(payload, checkPayload);
        client1Passed = true;
    });

    // Client2 Setup
    Client client2("127.0.0.1", 4242, safeties);
    auto client2Connector = std::make_shared<Connector>(client2);
    client2.setConnector(client2Connector);
    client2Connector->start();
    client2.registerReceiveHandler<std::uint16_t>(2, [&client2Passed, &checkPayload](const PacketHeader &, uint16_t payload) {
        ASSERT_EQ(payload, checkPayload);
        client2Passed = false;
    });

    // Client Infos
    std::atomic<SessionId> id1;
    std::atomic<SessionId> id2;
    std::atomic<bool> connected1 = false;
    std::atomic<bool> connected2 = false;

    client1.connect([&id1, &connected1](bool authorized, SessionId sId) {
        connected1 = authorized;
        id1 = sId;
    });

    client2.connect([&id2, &connected2](bool authorized, SessionId sId) {
        connected2 = authorized;
        id2 = sId;
    });

    PNL_WAIT_COND_LOOP(!connected1 || !connected2, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected1);
    ASSERT_TRUE(connected2);
    server.sendTo(2, checkPayload, id1);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_TRUE(client1Passed);
    ASSERT_TRUE(client2Passed);
}

TEST(udpE2E, ServerSendCallback)
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

    // Client Setup
    Client client("127.0.0.1", 4242, safeties);
    auto clientConnector = std::make_shared<Connector>(client);
    client.setConnector(clientConnector);
    clientConnector->start();
    client.registerReceiveHandler<std::uint16_t>(2, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
    });

    // Client Infos
    SessionId id;
    bool connected = false;

    client.connect([&id, &connected](bool authorized, SessionId sId) {
        connected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected);
    server.sendTo<std::uint16_t>(2, input_data, id, [&passed](const PacketHeader &header, const std::uint16_t &payload) {
        passed = true;
    });
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_data, output_data);
    ASSERT_TRUE(passed);
}

#ifdef PNL_SERVER_TEST
#if PNL_CLIENT_TEST == 1
TEST(udpE2E, SafetyServerReceive)
{
    //checks
    std::uint32_t output_data = 0;

    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    std::map<OpId, bool> safeties = {
            { 2, false }
    };

    // Server Setup
    Server server(4242, safeties);
    auto serverConnector = std::make_shared<Connector>(server);
    server.setConnector(serverConnector);
    serverConnector->start();

    server.registerReceiveHandler<std::uint32_t>(2, [&output_data](const PacketHeader &, const std::uint32_t &payload) {
        output_data = payload;
    });
    PNL_WAIT_COND(output_data != 0, PNL_TIME_OUT)
}
#endif
#endif