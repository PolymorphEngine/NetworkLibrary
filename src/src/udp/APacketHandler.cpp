/*
** EPITECH PROJECT, 2022
** APacketHandler.cpp
** File description:
** APacketHandler.cpp
*/


#include <asio/ip/udp.hpp>
#include <iostream>
#include "Polymorph/Network/udp/APacketHandler.hpp"

polymorph::network::udp::APacketHandler::APacketHandler(asio::ip::udp::endpoint endpoint)
    : _context(), _socket(_context, endpoint)
{
    if (_receiveCallbacks.contains(0)) {
        std::cerr << "OpId 0 is reserved for internal use" << std::endl;
        _receiveCallbacks.erase(0);
    }
    if (_receiveCallbacks.contains(1)) {
        std::cerr << "OpId 1 is reserved for internal use" << std::endl;
        _receiveCallbacks.erase(1);
    }
}

polymorph::network::udp::APacketHandler::~APacketHandler()
{
    _context.stop();
    _thread.join();
}

void polymorph::network::udp::APacketHandler::packetReceived(asio::ip::udp::endpoint from, const std::vector<std::byte> &bytes)
{
    PacketHeader header{0};

    try {
        header = SerializerTrait<PacketHeader>::deserialize(bytes);
    } catch (const exceptions::DeserializingException &e) {
        std::cerr << "Error while deserializing packet header: " << e.what() << std::endl;
        return;
    }
    _broadcastReceivedPacket(header, bytes);
}

asio::ip::udp::socket &polymorph::network::udp::APacketHandler::getPreparedSocket()
{
    return _socket;
}

void polymorph::network::udp::APacketHandler::start()
{
    _thread = std::thread([this]() {
        _context.run();
    });
}

void polymorph::network::udp::APacketHandler::unregisterReceiveHandlers(polymorph::network::OpId opId)
{
    _receiveCallbacks.erase(opId);
}

void polymorph::network::udp::APacketHandler::setConnector(std::shared_ptr<Connector> connector)
{
    _connector = std::move(connector);
}

void
polymorph::network::udp::APacketHandler::_addSendCallback(const asio::ip::udp::endpoint& to, polymorph::network::PacketId id,
        std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback)
{
    _sentCallbacks[std::make_pair(to, id)] = std::move(callback);
}

void polymorph::network::udp::APacketHandler::_callAndPopSendCallback(asio::ip::udp::endpoint to,
        const polymorph::network::PacketHeader &header, const std::vector<std::byte> &bytes)
{
    auto it = _sentCallbacks.find(std::make_pair(to, header.pId));

    if (it != _sentCallbacks.end()) {
        it->second(header, bytes);
        _sentCallbacks.erase(it);
    }
}

void polymorph::network::udp::APacketHandler::_broadcastReceivedPacket(const polymorph::network::PacketHeader &header,
        const std::vector<std::byte> &bytes)
{
    if (_receiveCallbacks.contains(header.opId)) {
        for (auto &callback : _receiveCallbacks[header.opId]) {
            callback(header, bytes);
        }
    }
}
