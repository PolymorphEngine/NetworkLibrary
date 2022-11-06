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

TEST(sessionTransferE2E, TcpToUdp)
{
    using namespace polymorph::network;

    //checks
    SessionTransferRequestDto request;


    // Server Setup
    auto tcpServer = tcp::Server::create(4242);
    tcpServer->start();

    // Client Setup
    auto tcpClient = tcp::Client::create("127.0.0.1", 4242);

    // Client Infos
    SessionId id;
    bool tcpConnected = false;

    std::atomic<bool> authKeyReceived(false);
    AuthorizationKey authKey;

    tcpClient->connect([&id, &tcpConnected](bool authorized, SessionId sId) {
        id = sId;
        tcpConnected = true;
    });

    PNL_WAIT_COND_LOOP(!tcpConnected, PNL_TIME_OUT, 5)
    ASSERT_TRUE(tcpConnected);
    tcpClient->registerReceiveHandler<SessionTransferResponseDto>(SessionTransferResponseDto::opId,
        [&authKey, &authKeyReceived](const PacketHeader &,
                                    const SessionTransferResponseDto &response) {
            authKey = response.authKey;
            authKeyReceived = true;
            return true;
        }
    );
    tcpClient->send(SessionTransferRequestDto::opId, request);
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_TRUE(authKeyReceived);

// Server Setup
    auto sessionStore = tcpServer->getSessionStore();
    std::map<OpId, bool> safeties;
    auto udpServer = udp::Server::create(4242, safeties);
    udpServer->setSessionStore(sessionStore);
    udpServer->start();

// Client Setup
    auto udpClient = udp::Client::create("127.0.0.1", 4242, safeties);

// Client Infos
    bool udpConnected = false;

    udpClient->connectWithSession(id, authKey, [&udpConnected](bool authorized, SessionId) {
        udpConnected = authorized;
    });
    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_TRUE(udpConnected);
}