/*
** EPITECH PROJECT, 2022
** ServerSend.cpp
** File description:
** ServerSend.cpp
*/

#include <gtest/gtest.h>
#include <thread>
#include "polymorph/network/tcp/Server.hpp"
#include "polymorph/network/tcp/Client.hpp"
#include "../utils.hpp"

TEST(tcpE2E, ServerSend)
{
    //checks
    std::uint16_t input_data = 42;
    std::uint16_t output_data = 0;

    using namespace polymorph::network;
    using namespace polymorph::network::tcp;

    // Server Setup
    auto server = Server::create(4242);
    server->start();

    // Client Setup
    auto client = Client::create("127.0.0.1", 4242);

    client->registerReceiveHandler<std::uint16_t>(10, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
        return true;
    });

    // Client Infos
    SessionId id;
    bool connected = false;

    client->connect([&id, &connected](bool authorized, SessionId sId) {
        connected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected);
    server->send<std::uint16_t>(10, input_data);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_data, output_data);
}

TEST(tcpE2E, OpIdDispatchServerSend)
{
    //checks
    std::uint16_t input_data = 42;
    std::uint8_t input_char = 'A';
    std::uint16_t output_data = 0;
    std::uint8_t output_char = ' ';

    using namespace polymorph::network;
    using namespace polymorph::network::tcp;

    // Server Setup
    auto server = Server::create(4242);
    server->start();

    // Client Setup
    auto client = Client::create("127.0.0.1", 4242);
        client->registerReceiveHandler<std::uint16_t>(10, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
        return true;
    });
    client->registerReceiveHandler<std::uint8_t>(11, [&output_char](const PacketHeader &, std::uint8_t payload) {
        output_char = payload;
        return true;
    });

    // Client Infos
    SessionId id;
    bool connected = false;

    client->connect([&id, &connected](bool authorized, SessionId sId) {
        connected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected);
    server->send(11, input_char);
    server->send(10, input_data);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_char, output_char);
    ASSERT_EQ(input_data, output_data);
}

TEST(tcpE2E, ServerDispatchToAllClients)
{
    //checks
    bool client1Passed = false;
    bool client2Passed = false;
    std::uint16_t checkPayload = 42;

    using namespace polymorph::network;
    using namespace polymorph::network::tcp;

    // Server Setup
    auto server = Server::create(4242);
    server->start();

    // Client1 Setup
    auto client1 = Client::create("127.0.0.1", 4242);
    client1->registerReceiveHandler<std::uint16_t>(10, [&client1Passed, &checkPayload](const PacketHeader &, const uint16_t& payload) {
        client1Passed = (payload == checkPayload);
        return true;
    });

    // Client2 Setup
    auto client2 = Client::create("127.0.0.1", 4242);
        client2->registerReceiveHandler<std::uint16_t>(10, [&client2Passed, &checkPayload](const PacketHeader &, uint16_t payload) {
        client2Passed = (payload == checkPayload);
        return true;
    });

    // Client Infos
    std::atomic<SessionId> id1;
    std::atomic<SessionId> id2;
    std::atomic<bool> connected1 = false;
    std::atomic<bool> connected2 = false;

    client1->connect([&id1, &connected1](bool authorized, SessionId sId) {
        connected1 = authorized;
        id1 = sId;
    });

    client2->connect([&id2, &connected2](bool authorized, SessionId sId) {
        connected2 = authorized;
        id2 = sId;
    });

    PNL_WAIT_COND_LOOP(!connected1 || !connected2, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected1);
    ASSERT_TRUE(connected2);
    server->send(10, checkPayload);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_TRUE(client1Passed);
    ASSERT_TRUE(client2Passed);
}

TEST(tcpE2E, ServerSendOnlyOneClient)
{
    //checks
    bool client1Passed = false;
    bool client2Passed = true;
    std::uint16_t checkPayload = 42;

    using namespace polymorph::network;
    using namespace polymorph::network::tcp;

    // Server Setup
    auto server = Server::create(4242);
    server->start();

    // Client1 Setup
    auto client1 = Client::create("127.0.0.1", 4242);
    client1->registerReceiveHandler<std::uint16_t>(10, [&client1Passed, &checkPayload](const PacketHeader &, uint16_t payload) {
        client1Passed = (payload == checkPayload);
        return true;
    });

    // Client2 Setup
    auto client2 = Client::create("127.0.0.1", 4242);
    client2->registerReceiveHandler<std::uint16_t>(10, [&client2Passed](const PacketHeader &, uint16_t payload) {
        client2Passed = false;
        return true;
    });

    // Client Infos
    std::atomic<SessionId> id1;
    std::atomic<SessionId> id2;
    std::atomic<bool> connected1 = false;
    std::atomic<bool> connected2 = false;

    client1->connect([&id1, &connected1](bool authorized, SessionId sId) {
        connected1 = authorized;
        id1 = sId;
    });

    client2->connect([&id2, &connected2](bool authorized, SessionId sId) {
        connected2 = authorized;
        id2 = sId;
    });

    PNL_WAIT_COND_LOOP(!connected1 || !connected2, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected1);
    ASSERT_TRUE(connected2);
    server->sendTo(10, checkPayload, id1);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_TRUE(client1Passed);
    ASSERT_TRUE(client2Passed);
}

TEST(tcpE2E, ServerSendCallback)
{
    //checks
    std::uint16_t input_data = 42;
    std::uint16_t output_data = 0;
    bool passed = false;

    using namespace polymorph::network;
    using namespace polymorph::network::tcp;

    // Server Setup
    auto server = Server::create(4242);
    server->start();

    // Client Setup
    auto client = Client::create("127.0.0.1", 4242);
    client->registerReceiveHandler<std::uint16_t>(10, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
        return true;
    });

    // Client Infos
    SessionId id;
    bool connected = false;

    client->connect([&id, &connected](bool authorized, SessionId sId) {
        connected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected);
    server->sendTo<std::uint16_t>(10, input_data, id, [&passed](const PacketHeader &header, const std::uint16_t &payload) {
        passed = true;
    });
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_data, output_data);
    ASSERT_TRUE(passed);
}

#ifdef PNL_SERVER_TEST
#if PNL_CLIENT_TEST == 1
TEST(tcpE2E, SafetyServerReceive)
{
    //checks
    std::uint32_t output_data = 0;

    using namespace polymorph::network;
    using namespace polymorph::network::tcp;

    // Server Setup
    SessionStore serverStore;
    Server server(4242, serverStore);
    server.start();

    server.registerReceiveHandler<std::uint32_t>(2, [&output_data](const PacketHeader &, const std::uint32_t &payload) {
        output_data = payload;
    });
    PNL_WAIT_COND(output_data != 0, PNL_TIME_OUT)
}
#endif
#endif