/*
** EPITECH PROJECT, 2022
** APacketHandler.cpp
** File description:
** APacketHandler.cpp
*/


#include <asio/ip/udp.hpp>
#include <iostream>
#include <mutex>
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
    std::unique_lock<std::mutex> lock(_receiveCallbacksRemoveQueueMutex);
    _receiveCallbacksToRemoveQueue.push_back(opId);
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
    _updateReceiveCallbacksMap();

    if (_receiveCallbacks.contains(header.opId)) {
        std::vector<std::shared_ptr<std::function<int(const PacketHeader &, const std::vector<std::byte> &)>>> callbacksToPop;

        for (auto &callback : _receiveCallbacks[header.opId]) {
            if (!callback) {
                callbacksToPop.push_back(callback);
                continue;
            }
            auto res = (*callback)(header, bytes);

            if (res == 0)
                callbacksToPop.push_back(callback);
        }
        for (auto &callback : callbacksToPop) {
            auto it = std::find(_receiveCallbacks[header.opId].begin(), _receiveCallbacks[header.opId].end(), callback);
            _receiveCallbacks[header.opId].erase(it);
        }
    }
}

void polymorph::network::udp::APacketHandler::_registerReceiveHandler(polymorph::network::OpId opId, std::function<int(const PacketHeader &, const std::vector<std::byte> &)> handler)
{
    std::unique_lock<std::mutex> lock(_receiveCallbacksAddQueueMutex);

    _receiveCallbacksAddQueue[opId].emplace_back(std::make_shared<std::function<int(const PacketHeader &, const std::vector<std::byte> &)>>(std::move(handler)));
}

void polymorph::network::udp::APacketHandler::_updateReceiveCallbacksMap()
{
    {
        std::scoped_lock lock(_receiveCallbacksAddQueueMutex);
        for (auto &pair : _receiveCallbacksAddQueue) {
            std::copy(pair.second.begin(), pair.second.end(), std::back_inserter(_receiveCallbacks[pair.first]));
            _receiveCallbacksAddQueue[pair.first].clear();
        }
    }
    {
        std::scoped_lock lock(_receiveCallbacksRemoveQueueMutex);
        for (auto toRemove : _receiveCallbacksToRemoveQueue) {
            if (_receiveCallbacks.contains(toRemove)) {
                _receiveCallbacks[toRemove].clear();
            }
        }
    }
}
