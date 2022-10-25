/*
** EPITECH PROJECT, 2022
** Connector.cpp
** File description:
** Connector.cpp
*/

#include <iostream>
#include <mutex>
#include "Polymorph/Network/udp/Connector.hpp"
#include "Polymorph/Network/SerializerTrait.hpp"
#include "Polymorph/Network/dto/ACKDto.hpp"

polymorph::network::udp::Connector::Connector(polymorph::network::udp::IPacketHandler &handler)
    : _packetHandler(handler), _socket(handler.getPreparedSocket()), _receiveBuffer(), _writeInProgress(false)
{}

void polymorph::network::udp::Connector::send(const asio::ip::udp::endpoint& to,
                                              const std::vector<std::byte> &data)
{
    std::unique_lock<std::mutex> lock(_sendQueueMutex);
    _sendQueue.emplace(to, data);
    if (!_writeInProgress) {
        lock.unlock();
        _writeInProgress = true;
        _doSend();
    }
}

void polymorph::network::udp::Connector::start()
{
    _doReceive();
    _packetHandler.start();
}

void polymorph::network::udp::Connector::_doReceive()
{
    _socket.async_receive_from(asio::buffer(_receiveBuffer), _endpoint,
       [this](const asio::error_code &error, std::size_t bytesReceived) {
           if (error) {
               std::cerr << "Error while receiving data: " << error.message() << std::endl;
               return;
           }
           std::vector<std::byte> data(_receiveBuffer.begin(), _receiveBuffer.begin() + bytesReceived);
           _determinePacket(data);
           _doReceive();
       });
}

void polymorph::network::udp::Connector::_doSend()
{
    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    _socket.async_send_to(asio::buffer(_sendQueue.front().second), _sendQueue.front().first,
       [this](const asio::error_code &error, std::size_t) {
           if (error) {
               std::cerr << "Error while sending packet: " << error.message() << std::endl;
               return;
           }
           std::unique_lock<std::mutex> lock(_sendQueueMutex);
           auto header = SerializerTrait<PacketHeader>::deserialize(_sendQueue.front().second);
           _packetHandler.packetSent(_sendQueue.front().first, header, _sendQueue.front().second);
           _sendQueue.pop();
           if (!_sendQueue.empty()) {
                lock.unlock();
               _doSend();
           } else {
               _writeInProgress = false;
           }
       });
}

void polymorph::network::udp::Connector::_determinePacket(const std::vector<std::byte> &data)
{
    PacketHeader header {0};

    try {
        header = SerializerTrait<PacketHeader>::deserialize(data);
    } catch (const exceptions::DeserializingException &e) {
        std::cerr << "Error while deserializing packet header: " << e.what() << std::endl;
        return;
    }

    if (header.opId == ACKDto::opId) {
        auto packet = SerializerTrait<Packet<ACKDto>>::deserialize(data);
        _packetHandler.ackReceived(_endpoint, packet.payload.id);
    } else {
        _packetHandler.packetReceived(_endpoint, data);
    }
}