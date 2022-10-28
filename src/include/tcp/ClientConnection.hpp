/*
** EPITECH PROJECT, 2020
** ClientConnection.hpp
** File description:
** header for ClientConnection.c
*/


#pragma once

#include <memory>
#include <asio/ip/udp.hpp>
#include <queue>
#include "polymorph/network/SessionStore.hpp"
#include "polymorph/network/PacketHeader.hpp"
#include "polymorph/network/tcp/IConnectionPool.hpp"
#include "polymorph/network/tcp/IPacketHandler.hpp"
#include "polymorph/network/tcp/Server.hpp"

namespace polymorph::network::tcp
{

    class ClientConnection : public std::enable_shared_from_this<ClientConnection>, public IClientConnection
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            ClientConnection(asio::ip::tcp::socket socket, SessionStore &sessionStore, std::weak_ptr<IConnectionPool> pool, Server &packetHandler);

            ~ClientConnection() = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            SessionStore &_sessionAttributor;
            std::weak_ptr<IConnectionPool> _connectionPool;
            Server &_packetHandler;

            std::atomic<bool> _stopped;

            std::atomic<bool> _connected;

            std::queue<std::pair<std::vector<std::byte>, std::function<void (const PacketHeader &, const std::vector<std::byte> &)>>> _sendQueue;
            std::atomic<bool> _writeInProgress;
            std::mutex _sendQueueMutex;

            std::vector<std::byte> _receiveBuffer;
            std::array<std::byte, 1024> _internalBuffer{};

            asio::ip::tcp::socket _socket;

            SessionId _sessionId{};
            PacketId _packetId = 0;


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            void start() override;

            void send(std::vector<std::byte> data, std::function<void (const PacketHeader&, const std::vector<std::byte> &)> callback) override;

            bool isConnected() override;

            SessionId getSessionId() override;

            PacketId getPacketId() override;

        private:
            void _doSend();

            void _doReceive();

            void _handleHandshakePacket(const PacketHeader &header, const std::vector<std::byte> &bytes);

            void _handleSessionTransferPacket(const PacketHeader &header, const std::vector<std::byte> &bytes);

            bool _broadcastReceivedPacket(const PacketHeader &header, const std::vector<std::byte> &bytes);

            bool _determinePacket(const PacketHeader &header, const std::vector<std::byte> &bytes);


//////////////////////--------------------------/////////////////////////

    };
}
