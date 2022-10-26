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

    class Client : public APacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            Client(std::string host, std::uint16_t port);

            ~Client() override = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            asio::ip::tcp::socket _socket;
        asio::ip::tcp::endpoint _serverEndpoint;

            PacketId _currentPacketId = 0;
            SessionId _currentSession = 0;
            std::atomic<bool> _isConnected = false;
            std::atomic<bool> _isConnecting = true;

            std::queue<std::pair<std::vector<std::byte>, std::function<void (const PacketHeader &, const std::vector<std::byte> &)>>> _sendQueue;
            std::atomic<bool> _writeInProgress;
            std::mutex _sendQueueMutex;

            std::vector<std::byte> _receiveBuffer;
            std::array<std::byte, 1024> _internalBuffer;



//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
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
            void connect(std::function<void(bool, SessionId)> callback);

            void connectWithSession(SessionId session, AuthorizationKey authKey, std::function<void(bool, SessionId)> callback);


        private:
            void _doSend();

            void _doReceive();

            bool _broadcastReceivedPacket(const PacketHeader &header, const std::vector<std::byte> &bytes);


//////////////////////--------------------------/////////////////////////

    };
}