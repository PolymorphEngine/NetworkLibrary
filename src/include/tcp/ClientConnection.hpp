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
#include "Polymorph/Network/SessionStore.hpp"
#include "Polymorph/Network/PacketHeader.hpp"
#include "Polymorph/Network/tcp/IConnectionPool.hpp"
#include "Polymorph/Network/tcp/IPacketHandler.hpp"

namespace polymorph::network::tcp
{

    class ClientConnection : public std::enable_shared_from_this<ClientConnection>
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            ClientConnection(asio::ip::tcp::socket socket, SessionStore &sessionStore, IConnectionPool &pool, IPacketHandler &packetHandler);

            ~ClientConnection() = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            SessionStore &_sessionAttributor;
            IConnectionPool &_connectionPool;
            IPacketHandler &_packetHandler;

            std::atomic<bool> _stopped;

            std::atomic<bool> _connected;

            std::queue<std::pair<std::vector<std::byte>, std::function<void (const PacketHeader &, const std::vector<std::byte> &)>>> _sendQueue;
            std::atomic<bool> _writeInProgress;
            std::mutex _sendQueueMutex;

            std::vector<std::byte> _receiveBuffer;
            std::array<std::byte, 1024> _internalBuffer;

            asio::ip::tcp::socket _socket;

            SessionId _sessionId;
            PacketId _packetId = 0;


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            void start();

            void send(std::vector<std::byte> data, std::function<void (const PacketHeader &, const std::vector<std::byte> &)> callback = nullptr);

            bool isConnected();

            SessionId getSessionId();

            PacketId getPacketId();

        private:
            void _doSend();

            void _doReceive();

            void _handleHandshakePacket(const PacketHeader &header, const std::vector<std::byte> &bytes);

            bool _broadcastReceivedPacket(const PacketHeader &header, const std::vector<std::byte> &bytes);

            bool _determinePacket(const PacketHeader &header, const std::vector<std::byte> &bytes);


//////////////////////--------------------------/////////////////////////

    };
}
