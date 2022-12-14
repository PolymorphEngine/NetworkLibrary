/*
** EPITECH PROJECT, 2022
** ServerPacketManager.cpp
** File description:
** ServerPacketManager.cpp
*/

#include "udp/ServerPacketManager.hpp"
#include "udp/PacketStore.hpp"

bool polymorph::network::udp::ServerPacketManager::hasClient(const asio::ip::udp::endpoint& endpoint)
{
    std::lock_guard<std::mutex> lock(_clientsStoresMutex);
    return _packetStores.contains(endpoint);
}

bool polymorph::network::udp::ServerPacketManager::registerClient(const asio::ip::udp::endpoint& endpoint)
{
    if (hasClient(endpoint))
        return false;

    std::lock_guard<std::mutex> lock(_clientsStoresMutex);

    auto ret = _packetStores.emplace(endpoint, std::make_unique<PacketStore>(_io_context, _safeties)).second;
    if  (ret)
        _packetStores[endpoint]->setResendCallback(_resendCallback);
    return ret;
}

bool polymorph::network::udp::ServerPacketManager::removeClient(
        const asio::ip::udp::endpoint &endpoint)
{
    if (!hasClient(endpoint))
        return false;

    std::lock_guard<std::mutex> lock(_clientsStoresMutex);
    return _packetStores.erase(endpoint) == 1;
}

polymorph::network::udp::PacketStore&
polymorph::network::udp::ServerPacketManager::storeOf(const asio::ip::udp::endpoint &endpoint)
{
    std::lock_guard<std::mutex> lock(_clientsStoresMutex);
    return *_packetStores.at(endpoint);
}

polymorph::network::PacketId
polymorph::network::udp::ServerPacketManager::packetIdOf(const asio::ip::udp::endpoint &endpoint)
{
    std::lock_guard<std::mutex> lock(_packetIdsMutex);
    if (_currentPacketIds.contains(endpoint))
        return ++_currentPacketIds[endpoint];
    else {
        _currentPacketIds[endpoint] = 1;
        return 1;
    }
}
