/*
** EPITECH PROJECT, 2022
** Connector.cpp
** File description:
** Connector.cpp
*/

#include <iostream>
#include "Polymorph/Network/udp/Connector.hpp"
#include "Polymorph/Network/SerializerTrait.hpp"

polymorph::network::udp::Connector::Connector(polymorph::network::udp::IPacketHandler &handler)
    : _packetHandler(handler), _socket(handler.getPreparedSocket()), _receiveBuffer(), _writeInProgress(false)
{}

void polymorph::network::udp::Connector::send(const asio::ip::udp::endpoint& to,
                                              const std::vector<std::byte> &data)
{
    _sendQueue.emplace(to, data);
    if (!_writeInProgress) {
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
    _socket.async_receive(asio::buffer(_receiveBuffer),
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
    _socket.async_send_to(asio::buffer(_sendQueue.front().second), _sendQueue.front().first,
       [this](const asio::error_code &error, std::size_t bytesSent) {
           if (error) {
               std::cerr << "Error while sending packet: " << error.message() << std::endl;
               return;
           }
           _sendQueue.pop();
           if (!_sendQueue.empty()) {
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

    if (header.opId == 1) {
        _packetHandler.packetReceived(_socket.remote_endpoint(), data);
    } else {
        _packetHandler.ackReceived(_socket.remote_endpoint(), header.pId);
    }
}