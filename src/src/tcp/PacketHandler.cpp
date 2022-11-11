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
    int res;
    std::vector<std::shared_ptr<std::function<int(const PacketHeader &header, const std::vector<std::byte> &bytes)>>> callbacksToPop;


    try {
        header = SerializerTrait<PacketHeader>::deserialize(bytes);
    } catch (const exceptions::DeserializingException &e) {
        std::cerr << "Error while deserializing packet header: " << e.what() << std::endl;
        return false;
    }

    _updateReceiveCallbacksMap();
    if (_receiveCallbacks.contains(header.opId)) {
        for (auto &callback : _receiveCallbacks[header.opId]) {
            res = (*callback)(header, bytes);
            if (res == -1)
                return false;
            if (res == 0)
                callbacksToPop.push_back(callback);
        }
    }
    for (auto &toPop: callbacksToPop) {
        auto it = std::find(_receiveCallbacks[header.opId].begin(), _receiveCallbacks[header.opId].end(), toPop);
        _receiveCallbacks[header.opId].erase(it);
    }
    return true;
}

void polymorph::network::tcp::APacketHandler::unregisterReceiveHandlers(polymorph::network::OpId opId)
{
    std::unique_lock<std::mutex> lock(_receiveCallbacksRemoveMutex);
    _receiveCallbacksToRemoveQueue.push_back(opId);
}

void polymorph::network::tcp::APacketHandler::_run()
{
    _thread = std::thread([this]() {
        _context.run();
    });
}

void polymorph::network::tcp::APacketHandler::_registerReceiveHandler(polymorph::network::OpId opId, std::function<int(const PacketHeader &, const std::vector<std::byte> &)> handler)
{
    std::unique_lock<std::mutex> lock(_receiveCallbacksAddQueueMutex);
    _receiveCallbacksAddQueue[opId].emplace_back(std::make_shared<std::function<int(const PacketHeader &, const std::vector<std::byte> &)>>(std::move(handler)));
}

void polymorph::network::tcp::APacketHandler::_updateReceiveCallbacksMap()
{
    {
        std::scoped_lock lock(_receiveCallbacksAddQueueMutex);
        for (auto &pair : _receiveCallbacksAddQueue) {
            std::copy(pair.second.begin(), pair.second.end(), std::back_inserter(_receiveCallbacks[pair.first]));
            _receiveCallbacksAddQueue[pair.first].clear();
        }
    }
    {
        std::scoped_lock lock(_receiveCallbacksRemoveMutex);
        for (auto toRemove : _receiveCallbacksToRemoveQueue) {
            if (_receiveCallbacks.contains(toRemove)) {
                _receiveCallbacks[toRemove].clear();
            }
        }
    }
}