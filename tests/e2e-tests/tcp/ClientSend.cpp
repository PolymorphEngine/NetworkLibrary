/*
** EPITECH PROJECT, 2022
** SimpleHandshake.cpp
** File description:
** SimpleHandshake.cpp
*/

#include <gtest/gtest.h>
#include <thread>
#include "polymorph/network/tcp/Server.hpp"
#include "polymorph/network/tcp/Client.hpp"
#include "../utils.hpp"

TEST(tcpE2E, ClientSend)
{
    //checks
    std::uint16_t input_data = 42;
    std::uint16_t output_data = 0;

    using namespace polymorph::network;
    using namespace polymorph::network::tcp;

    // Server Setup
    auto server = Server::create(4242);
    server->start();
    server->registerReceiveHandler<std::uint16_t>(3, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
        return true;
    });

    // Client Setup
    auto client = Client::create("127.0.0.1", 4242);

    // Client Infos
    SessionId id;
    bool connected = false;

    client->connect([&id, &connected](bool authorized, SessionId sId) {
        connected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected);
    client->send(3, input_data);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_data, output_data);
}

TEST(tcpE2E, ClientSendCallback)
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
    server->registerReceiveHandler<std::uint16_t>(3, [&output_data](const PacketHeader &, uint16_t payload) {
        output_data = payload;
        return true;
    });

    // Client Setup
    auto client = Client::create("127.0.0.1", 4242);

    // Client Infos
    SessionId id;
    bool connected = false;

    client->connect([&id, &connected](bool authorized, SessionId sId) {
        connected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!connected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(connected);
    client->send<std::uint16_t>(3, input_data, [&passed](const PacketHeader &header, const std::uint16_t &payload) {
        passed = true;
    });
    // server.sendTo(2, input_data, id); NOT WORKING LA PTN DE SA
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_EQ(input_data, output_data);
    ASSERT_TRUE(passed);
}

#ifdef PNL_CLIENT_TEST
#if PNL_CLIENT_TEST == 1
TEST(tcpE2E, SafetyClientSend)
{
    //checks
    std::uint32_t input_data = 4294967295;
    std::uint32_t output_data = 0;

    using namespace polymorph::network;
    using namespace polymorph::network::tcp;

    // Client Setup
    SessionStore serverStore;
    Client client("127.0.0.1", 4242, serverStore);

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