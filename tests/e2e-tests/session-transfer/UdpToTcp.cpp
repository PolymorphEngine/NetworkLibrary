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
    SessionStore serverStore;
    udp::Server udpServer(4242, safeties, serverStore);
    auto serverConnector = std::make_shared<udp::Connector>(udpServer);
    udpServer.setConnector(serverConnector);
    serverConnector->start();

    // Client Setup
    udp::Client udpClient("127.0.0.1", 4242, safeties);
    auto clientConnector = std::make_shared<udp::Connector>(udpClient);
    udpClient.setConnector(clientConnector);
    clientConnector->start();

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
    auto tcpServer = tcp::Server::create(4242);
    tcpServer->setSessionStore(&serverStore);
    tcpServer->start();

    // Client Setup
    tcp::Client tcpClient("127.0.0.1", 4242);

    bool tcpConnected = false;


    tcpClient.connectWithSession(id, authKey, [&tcpConnected](bool authorized, SessionId sId) {
        tcpConnected = true;
    });

    PNL_WAIT(PNL_TIME_OUT)
    ASSERT_TRUE(tcpConnected);
}