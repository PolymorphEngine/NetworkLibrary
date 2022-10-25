/*
** EPITECH PROJECT, 2022
** APacketHandler.cpp
** File description:
** APacketHandler.cpp
*/

#include <iostream>
#include "Polymorph/Network/tcp/APacketHandler.hpp"
#include "Polymorph/Network/SerializerTrait.hpp"
#include "Polymorph/Network/exceptions/DeserializingException.hpp"

polymorph::network::tcp::APacketHandler::APacketHandler(asio::ip::tcp::endpoint endpoint)
    : _context(), _socket(_context, endpoint)
{
}

polymorph::network::tcp::APacketHandler::~APacketHandler()
{
    _context.stop();
    if (_thread.joinable())
        _thread.join();
}

bool
polymorph::network::tcp::APacketHandler::packetReceived(const PacketHeader&, const std::vector<std::byte> &bytes)
{
    PacketHeader header {0};

    try {
        header = SerializerTrait<PacketHeader>::deserialize(bytes);
        if (_receiveCallbacks.contains(header.opId)) {
            for (auto &callback : _receiveCallbacks[header.opId]) {
                if (!callback(header, bytes))
                    return false;
            }
        }
    } catch (const exceptions::DeserializingException &e) {
        std::cerr << "Error while deserializing packet header: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void polymorph::network::tcp::APacketHandler::unregisterReceiveHandlers(polymorph::network::OpId opId)
{
    _receiveCallbacks.erase(opId);
}

void polymorph::network::tcp::APacketHandler::_run()
{
    _thread = std::thread([this]() {
        _context.run();
    });
}