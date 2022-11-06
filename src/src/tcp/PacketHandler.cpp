/*
** EPITECH PROJECT, 2022
** APacketHandler.cpp
** File description:
** APacketHandler.cpp
*/

#include <iostream>
#include "tcp/PacketHandler.hpp"
#include "polymorph/network/SerializerTrait.hpp"
#include "polymorph/network/exceptions/DeserializingException.hpp"

polymorph::network::tcp::APacketHandler::~APacketHandler()
{
    if (!_context.stopped())
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

void polymorph::network::tcp::APacketHandler::_registerReceiveHandler(polymorph::network::OpId opId, std::function<bool(const PacketHeader &, const std::vector<std::byte> &)> handler)
{
    _receiveCallbacks[opId].emplace_back(std::move(handler));
}
