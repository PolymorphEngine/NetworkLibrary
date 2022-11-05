/*
** EPITECH PROJECT, 2020
** ClientImpl.hpp
** File description:
** header for ClientImpl.c
*/


#pragma once

#include <queue>
#include <iostream>
#include "PacketHandler.hpp"
#include "polymorph/network/dto/ACKDto.hpp"
#include "polymorph/network/tcp/Client.hpp"

namespace polymorph::network::tcp
{

    /**
     * @class   TCP ClientImpl class
     * @note    TCP exclusive
     */
    class ClientImpl : public Client, virtual public PacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            /**
             * @brief   Constructor of the TCP ClientImpl class
             *
             * @param   host    The server host ip address
             * @param   port    The server port
             */
            ClientImpl(std::string host, std::uint16_t port);

            ~ClientImpl() override;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
             * @property ClientImpl socket
             */
            asio::ip::tcp::socket _socket;

            /**
             * @property Remote server endpoint
             */
            asio::ip::tcp::endpoint _serverEndpoint;

            /**
             * @property    The current packet being received
             * @brief       Is used in the header of every packet to communicate the id of the packet
             */
            PacketId _currentPacketId = 0;

            /**
             * @property    The current session id
             * @brief       Is used in the header of every packet sent to communicate the session id
             */
            SessionId _currentSession = 0;

            /**
             * @property    Atomic boolean to know if the ClientImpl is connected
             */
            std::atomic<bool> _isConnected = false;

            /**
             * @property    Atomic boolean to know if the ClientImpl is connecting
             */
            std::atomic<bool> _isConnecting = true;

            /**
             * @property    Queue of packets to send
             */
            std::queue<std::pair<std::vector<std::byte>, std::function<void (const PacketHeader &, const std::vector<std::byte> &)>>> _sendQueue;

            /**
             * @property    Atomic boolean to know if the ClientImpl is already sending a packet
             */
            std::atomic<bool> _writeInProgress;

            /**
             * @property    Mutex to lock the send queue and ensure thread safety
             */
            std::mutex _sendQueueMutex;

            /**
             * @property    Buffer to receive packets
             */
            std::vector<std::byte> _receiveBuffer;

            /**
             * @property    Internal buffer to receive packets
             */
            std::array<std::byte, 1024> _internalBuffer;



//////////////////////--------------------------/////////////////////////

/////////////////////////////// METHODS /////////////////////////////////
        public:

            /**
             * @brief   Connect to server and execute a callback when done
             * @param   callback    Callback executed when the connection is done (or not)
             * @note    bool of std::function correspond to the auth status (true = success, false otherwise)
             */
            void connect(std::function<void(bool, SessionId)> callback) override;

            /**
             * @brief   Connect from a SessionId
             * @note    This method is used to communicate with the server with a authenticated session
             */
            void connectWithSession(SessionId session, AuthorizationKey authKey, std::function<void(bool, SessionId)> callback) override;


        private:
            /**
             * @brief   Method that use the queue to send packets
             */
            void _doSend();

            /**
             * @brief   Method to handle the reception of a packet, deserialize it and call the callback
             */
            void _doReceive();

            void _send(OpId opId, const std::vector<std::byte> &data, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback) override;


//////////////////////--------------------------/////////////////////////

    };
}