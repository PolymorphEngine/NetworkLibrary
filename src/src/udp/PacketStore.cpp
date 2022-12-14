/*
** EPITECH PROJECT, 2022
** PacketStore.cpp
** File description:
** PacketStore.cpp
*/


#include <iostream>
#include "udp/PacketStore.hpp"
#include "udp/SafePacketManager.hpp"


polymorph::network::udp::PacketStore::PacketStore(asio::io_context &context, std::map<OpId, bool> safeties)
    : _context(context), _safeties(std::move(safeties))
{
    if (_safeties.contains(0))
        std::cerr << "OpId 0 is reserved for internal use" << std::endl;
    if (_safeties.contains(1))
        std::cerr << "OpId 1 is reserved for internal use" << std::endl;

    _safeties[0] = true;
    _safeties[1] = false;
}

void polymorph::network::udp::PacketStore::addToSendList(polymorph::network::PacketHeader header,
                                                         std::vector<std::byte> data)
{
    std::lock_guard<std::mutex> lock(_toSendMutex);

    _toSend.emplace_back(header, std::move(data));
}

void polymorph::network::udp::PacketStore::confirmReceived(const polymorph::network::PacketId &id)
{
    std::lock_guard<std::mutex> lock(_safeSentMutex);

    std::erase_if(_safeSent, [&id](const auto &packet) {
        return packet->header.pId == id;
    });
}

void polymorph::network::udp::PacketStore::confirmSent(const asio::ip::udp::endpoint &endpoint, const polymorph::network::PacketId &id)
{
    std::lock_guard<std::mutex> lock(_toSendMutex);

    auto it = std::find_if(_toSend.begin(), _toSend.end(), [&id](const auto &packet) {
        return packet.first.pId == id;
    });
    if (it != _toSend.end()) {
        if (_safeties.contains(it->first.opId) && _safeties[it->first.opId])
            _markAsAC(it->first, it->second, endpoint);
        _toSend.erase(it);
    }
}

void
polymorph::network::udp::PacketStore::_markAsAC(polymorph::network::PacketHeader header, std::vector<std::byte> data, const asio::ip::udp::endpoint &recipient)
{
    std::lock_guard<std::mutex> lock(_safeSentMutex);

    asio::steady_timer timer(_context, _timeout);
    auto manager = std::make_shared<SafePacketManager>(std::move(timer), header, data, _maxRetries, recipient);
    auto weak = std::weak_ptr<SafePacketManager>(manager);
    manager->timer.async_wait([this, weak](const asio::error_code &error) {
        if (error == asio::error::operation_aborted || weak.expired())
            return;
        auto manager = weak.lock();
        std::cerr <<  "Timer expired for packet " + std::to_string(manager->header.pId) + "and error code is " + std::to_string(error.value()) << std::endl;
        if (manager->remainingRetries == 0) {
            this->confirmReceived(manager->header.pId);
            return;
        }
        manager->remainingRetries--;
        this->_resendCallback(manager);
    });
    _safeSent.push_back(std::move(manager));
}

void polymorph::network::udp::PacketStore::setResendCallback(
        std::function<void(std::shared_ptr<SafePacketManager>)> callback)
{
    _resendCallback = std::move(callback);

}
