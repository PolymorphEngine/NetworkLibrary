/*
** EPITECH PROJECT, 2022
** TcpToUdp.cpp
** File description:
** TcpToUdp.cpp
*/

#include <gtest/gtest.h>
#include "polymorph/network/tcp/Server.hpp"
#include "polymorph/network/tcp/Client.hpp"
#include "../utils.hpp"
#include "polymorph/network/dto/SessionTransferRequestDto.hpp"
#include "polymorph/network/dto/SessionTransferResponseDto.hpp"
#include "polymorph/network/udp/Server.hpp"
#include "polymorph/network/udp/Client.hpp"

TEST(sessionTransferE2E, UdpToTcp)
{
    using namespace polymorph::network;

    //checks
    SessionTransferRequestDto request;
    std::atomic<bool> authKeyReceived(false);
    AuthorizationKey authKey;

    // Server Setup
    std::map<OpId, bool> safeties;
    auto udpServer = udp::Server::create(4242, safeties);
    udpServer->start();

    // Client Setup
    udp::Client udpClient("127.0.0.1", 4242, safeties);

    // Client Infos
    bool udpConnected = false;
    SessionId id;

    udpClient.connect([&udpConnected, &id](bool authorized, SessionId sId) {
        udpConnected = authorized;
        id = sId;
    });

    PNL_WAIT_COND_LOOP(!udpConnected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(udpConnected);

    udpClient.registerReceiveHandler<SessionTransferResponseDto>(SessionTransferResponseDto::opId, [&authKey, &authKeyReceived](const PacketHeader &, const SessionTransferResponseDto &response) {
        authKey = response.authKey;
        authKeyReceived = true;
    });
    udpClient.send(SessionTransferRequestDto::opId, request);

    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_TRUE(authKeyReceived);


    // Server Setup
    auto sessionStore = udpServer->getSessionStore();
    auto tcpServer = tcp::Server::create(4242);
    tcpServer->setSessionStore(sessionStore);
    tcpServer->start();

    // Client Setup
    auto tcpClient = tcp::Client::create("127.0.0.1", 4242);

    bool tcpConnected = false;


    tcpClient->connectWithSession(id, authKey, [&tcpConnected](bool authorized, SessionId sId) {
        tcpConnected = true;
    });

    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_TRUE(tcpConnected);
}