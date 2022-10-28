/*
** EPITECH PROJECT, 2022
** TcpToUdp.cpp
** File description:
** TcpToUdp.cpp
*/

#include <gtest/gtest.h>
#include "Polymorph/Network/tcp/Server.hpp"
#include "Polymorph/Network/tcp/Client.hpp"
#include "../utils.hpp"
#include "Polymorph/Network/dto/SessionTransferRequestDto.hpp"
#include "Polymorph/Network/dto/SessionTransferResponseDto.hpp"
#include "Polymorph/Network/udp/Server.hpp"
#include "Polymorph/Network/udp/Client.hpp"

TEST(sessionTransferE2E, TcpToUdp)
{
    using namespace polymorph::network;

//checks
    SessionTransferRequestDto request;


// Server Setup
SessionStore serverStore;
tcp::Server tcpServer(4242, serverStore);
tcpServer.start();

// Client Setup
tcp::Client tcpClient("127.0.0.1", 4242);

// Client Infos
SessionId id;
bool tcpConnected = false;

std::atomic<bool> authKeyReceived(false);
AuthorizationKey authKey;

tcpClient.connect([&id, &tcpConnected](bool authorized, SessionId sId) {
    id = sId;
    tcpConnected = true;
});

PNL_WAIT_COND_LOOP(!tcpConnected, PNL_TIME_OUT, 5)
ASSERT_TRUE(tcpConnected);
tcpClient.registerReceiveHandler<SessionTransferResponseDto>(SessionTransferResponseDto::opId, [&authKey, &authKeyReceived](const PacketHeader &, const SessionTransferResponseDto &response) {
    authKey = response.authKey;
    authKeyReceived = true;
    return true;
});
tcpClient.send(SessionTransferRequestDto::opId, request);
PNL_WAIT(PNL_TIME_OUT)
ASSERT_TRUE(authKeyReceived);

// Server Setup
std::map<OpId, bool> safeties;
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

udpClient.connectWithSession(id, authKey, [&udpConnected](bool authorized, SessionId) {
    udpConnected = authorized;
});
PNL_WAIT(PNL_TIME_OUT)
ASSERT_TRUE(udpConnected);
}