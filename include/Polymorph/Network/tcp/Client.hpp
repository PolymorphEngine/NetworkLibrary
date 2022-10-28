/*
** EPITECH PROJECT, 2020
** Client.hpp
** File description:
** header for Client.c
*/


#pragma once

#include <queue>
#include <iostream>
#include "APacketHandler.hpp"
#include "Polymorph/Network/dto/ACKDto.hpp"

namespace polymorph::network::tcp
{

    /**
     * @class   TCP Client class
     * @note    TCP exclusive
     */
    class Client : public APacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            /**
             * @brief   Constructor of the TCP Client class
             *
             * @param   host    The server host ip address
             * @param   port    The server port
             */
            Client(std::string host, std::uint16_t port);

            ~Client() override;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
             * @property Client socket
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
             * @property    Atomic boolean to know if the client is connected
             */
            std::atomic<bool> _isConnected = false;

            /**
             * @property    Atomic boolean to know if the client is connecting
             */
            std::atomic<bool> _isConnecting = true;

            /**
             * @property    Queue of packets to send
             */
            std::queue<std::pair<std::vector<std::byte>, std::function<void (const PacketHeader &, const std::vector<std::byte> &)>>> _sendQueue;

            /**
             * @property    Atomic boolean to know if the client is already sending a packet
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
             * @brief   Send a payload to the server
             *
             * @tparam  T           The type of the payload to pack
             * @param   opId        The operation id of the Packet
             * @param   payload     The actual Packet data
             * @param   callback    Callback function used when the data has been (or not) delivered
             */
            template<typename T>
            void send(OpId opId, T &payload, std::function<void(const PacketHeader &, const T &)> callback = nullptr)
            {
                if (!_isConnecting && !_isConnected && opId != ACKDto::opId) {
                    std::cerr << "Trying to send a packet before client is connected" << std::endl;
                    return;
                }
                ++_currentPacketId;
                Packet<T> packet = {};
                packet.header.pId = _currentPacketId;
                packet.header.opId = opId;
                packet.header.sId = _currentSession;
                packet.payload = payload;
                std::vector<std::byte> sPacket = SerializerTrait<Packet<T>>::serialize(packet);

                std::unique_lock<std::mutex> lock(_sendQueueMutex);
                _sendQueue.push(std::make_pair(sPacket, [callback](const PacketHeader &header, const std::vector<std::byte> &payload) {
                    if (callback != nullptr) {
                        callback(header, SerializerTrait<T>::deserialize(payload));
                    }
                }));

                if (!_writeInProgress.exchange(true)) {
                    lock.unlock();
                    _doSend();
                }
            }

            /**
             * @brief   Connect to server and execute a callback when done
             * @param   callback    Callback executed when the connection is done (or not)
             * @note    bool of std::function correspond to the auth status (true = success, false otherwise)
             */
            void connect(std::function<void(bool, SessionId)> callback);

            /**
             * @brief   Connect from a SessionId
             * @note    This method is used to communicate with the server with a authenticated session
             */
            void connectWithSession(SessionId session, AuthorizationKey authKey, std::function<void(bool, SessionId)> callback);


        private:
            /**
             * @brief   Method that use the queue to send packets
             */
            void _doSend();

            /**
             * @brief   Method to handle the reception of a packet, deserialize it and call the callback
             */
            void _doReceive();

            // TODO: remove ?
            bool _broadcastReceivedPacket(const PacketHeader &header, const std::vector<std::byte> &bytes);


//////////////////////--------------------------/////////////////////////

    };
}