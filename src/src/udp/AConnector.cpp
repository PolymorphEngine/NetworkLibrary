/*
** EPITECH PROJECT, 2022
** Connector.cpp
** File description:
** Connector.cpp
*/

#include <iostream>
#include <mutex>
#include "udp/AConnector.hpp"
#include "polymorph/network/SerializerTrait.hpp"
#include "polymorph/network/dto/ACKDto.hpp"

polymorph::network::udp::AConnector::AConnector(asio::ip::udp::socket &socket)
    :_socket(socket), _receiveBuffer(), _writeInProgress(false)
{}

void polymorph::network::udp::AConnector::send(const asio::ip::udp::endpoint& to,
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

void polymorph::network::udp::AConnector::startConnection()
{
    _doReceive();
}

void polymorph::network::udp::AConnector::_doReceive()
{
    _receiveInProgress = false;
    _socket.async_receive_from(asio::buffer(_receiveBuffer), _endpoint,
        [this](const asio::error_code &error, std::size_t bytesReceived) {
            if (error == asio::error::operation_aborted || _stopping)
                return;
            if (error) {
                std::cerr << "Error while receiving data: " << error.message() << std::endl;
                return;
            }
            _receiveInProgress = true;
            std::vector<std::byte> data(_receiveBuffer.begin(), _receiveBuffer.begin() + bytesReceived);
            _determinePacket(data);
            _doReceive();
        }
    );
}

void polymorph::network::udp::AConnector::_doSend()
{
    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    _socket.async_send_to(asio::buffer(_sendQueue.front().second), _sendQueue.front().first,
        [this](const asio::error_code &error, std::size_t) {
            if (error == asio::error::operation_aborted || _stopping)
                return;
            if (error) {
                std::cerr << "Error while sending packet: " << error.message() << std::endl;
                return;
            }
            std::unique_lock<std::mutex> lock(_sendQueueMutex);
            auto header = SerializerTrait<PacketHeader>::deserialize(_sendQueue.front().second);
            _packetSent(_sendQueue.front().first, header, _sendQueue.front().second);
            _sendQueue.pop();
            if (!_sendQueue.empty()) {
                 lock.unlock();
                _doSend();
            } else {
                _writeInProgress = false;
            }
        }
   );
}

void polymorph::network::udp::AConnector::_determinePacket(const std::vector<std::byte> &data)
{
    PacketHeader header {0};

    try {
        header = SerializerTrait<PacketHeader>::deserialize(data);
    } catch (const exceptions::DeserializingException &e) {
        std::cerr << "Error while deserializing packet header: " << e.what() << std::endl;
        return;
    }

    if (header.opId == ACKDto::opId) {
        _ackReceived(_endpoint, header, data);
    } else {
        _packetReceived(_endpoint, data);
    }
}

bool polymorph::network::udp::AConnector::isWriteInProgress() const
{
    return _writeInProgress;
}

bool polymorph::network::udp::AConnector::isReceiveInProgress() const
{
    return _receiveInProgress;
}

void polymorph::network::udp::AConnector::stop()
{
    _stopping = true;
}
