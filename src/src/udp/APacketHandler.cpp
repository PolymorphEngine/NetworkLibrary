/*
** EPITECH PROJECT, 2022
** APacketHandler.cpp
** File description:
** APacketHandler.cpp
*/


#include <asio/ip/udp.hpp>
#include <iostream>
#include "udp/APacketHandler.hpp"

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
    if (_thread.joinable()) {
        _thread.join();
    }
}

void polymorph::network::udp::APacketHandler::_packetReceived(const asio::ip::udp::endpoint& from, const std::vector<std::byte> &bytes)
{
    PacketHeader header {0};

    try {
        header = SerializerTrait<PacketHeader>::deserialize(bytes);
    } catch (const exceptions::DeserializingException &e) {
        std::cerr << "Error while deserializing packet header: " << e.what() << std::endl;
        return;
    }
    _onPacketReceived(from, header, bytes);
    _broadcastReceivedPacket(header, bytes);
}

void polymorph::network::udp::APacketHandler::_run()
{
    _thread = std::thread([this]() {
        _context.run();
    });
}

void polymorph::network::udp::APacketHandler::unregisterReceiveHandlers(polymorph::network::OpId opId)
{
    _receiveCallbacks.erase(opId);
}

void polymorph::network::udp::APacketHandler::_callAndPopSendCallback(const asio::ip::udp::endpoint& to,
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

void polymorph::network::udp::APacketHandler::_registerReceiveHandler(polymorph::network::OpId opId, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> handler)
{
    _receiveCallbacks[opId].emplace_back(std::move(handler));
}
