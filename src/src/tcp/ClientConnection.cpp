/*
** EPITECH PROJECT, 2022
** ClientConnection.cpp
** File description:
** ClientConnection.cpp
*/


#include <iostream>
#include <utility>
#include "tcp/ClientConnection.hpp"
#include "polymorph/network/SerializerTrait.hpp"
#include "polymorph/network/dto/ConnectionDto.hpp"
#include "authorizationKey.hpp"
#include "polymorph/network/dto/ConnectionResponseDto.hpp"
#include "polymorph/network/exceptions/UnauthorizedException.hpp"
#include "polymorph/network/dto/SessionTransferRequestDto.hpp"
#include "polymorph/network/dto/SessionTransferResponseDto.hpp"
#include "tcp/ServerImpl.hpp"


polymorph::network::tcp::ClientConnection::ClientConnection(asio::ip::tcp::socket socket, SessionStore &sessionStore, std::weak_ptr<IConnectionPool> pool, ServerImpl &packetHandler)
        : _sessionAttributor(sessionStore), _connectionPool(std::move(pool)), _packetHandler(packetHandler), _stopped(false), _connected(false), _writeInProgress(false), _socket(std::move(socket))
{}

void polymorph::network::tcp::ClientConnection::start()
{
    if (!_connectionPool.expired())
        _connectionPool.lock()->join(shared_from_this());
    _doReceive();
}

void polymorph::network::tcp::ClientConnection::send(std::vector<std::byte> data, std::function<void(const PacketHeader &, const std::vector <std::byte> &)> callback)
{
    std::unique_lock<std::mutex> lock(_sendQueueMutex);

    _sendQueue.emplace(std::move(data), std::move(callback));
    if (!_writeInProgress.exchange(true)) {
        lock.unlock();
        _doSend();
    }
}

void polymorph::network::tcp::ClientConnection::_doSend()
{
    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    auto& p = _sendQueue.front();
    auto self(shared_from_this());
    _socket.async_send(asio::buffer(p.first), [this, self](const asio::error_code &error, std::size_t /* bytesSent */) {
        if (error || _stopped) {
            _stopped = true;
            if (!_connectionPool.expired())
                _connectionPool.lock()->leave(shared_from_this());
            return;
        }
        std::unique_lock<std::mutex> lock(_sendQueueMutex);
        auto p = _sendQueue.front();
        auto header = SerializerTrait<PacketHeader>::deserialize(p.first);
        if (p.second)
            p.second(header, p.first);
        _sendQueue.pop();
        if (_sendQueue.empty()) {
            _writeInProgress = false;
            return;
        }
        lock.unlock();
        _doSend();
    });
}

void polymorph::network::tcp::ClientConnection::_doReceive()
{
    auto self(shared_from_this());
    _socket.async_receive(asio::buffer(_internalBuffer), [this, self](const asio::error_code &error, std::size_t bytesReceived) {
        if (error || _stopped) {
            _stopped = true;
            if (!_connectionPool.expired())
                _connectionPool.lock()->leave(shared_from_this());
            return;
        }
        _receiveBuffer.insert(_receiveBuffer.end(), _internalBuffer.begin(), _internalBuffer.begin() + bytesReceived);
        while (_receiveBuffer.size() > sizeof(PacketHeader) ) {
            auto header = SerializerTrait<PacketHeader>::deserialize(_receiveBuffer);
            if (_receiveBuffer.size() >= sizeof(PacketHeader) + header.pSize) {
                if (!_determinePacket(header, _receiveBuffer))
                    return;
                _receiveBuffer.erase(_receiveBuffer.begin(), _receiveBuffer.begin() + sizeof(PacketHeader) + header.pSize);
            } else
                break;
        }
        _doReceive();
    });
}

void polymorph::network::tcp::ClientConnection::_handleHandshakePacket(const PacketHeader &,
                                                                       const std::vector <std::byte> &bytes)
{
    auto packet = SerializerTrait<Packet<ConnectionDto>>::deserialize(bytes);

    if (authorizationKey::areSame(packet.payload.authKey, authorizationKey::zero)) {
        _sessionId = _sessionAttributor.registerClient(_socket.remote_endpoint());
        _connected = true;
        ConnectionResponseDto response{ .authorized = true};
        _packetHandler.sendTo(ConnectionResponseDto::opId, response, _sessionId);
        return;
    }

    try {
        _sessionAttributor.registerAuthoredClient(_socket.remote_endpoint(), packet.payload.sessionId, packet.payload.authKey);
        _sessionId = packet.payload.sessionId;
        _connected = true;
        ConnectionResponseDto response{ .authorized = true};
        _packetHandler.sendTo(ConnectionResponseDto::opId, response, _sessionId);
    } catch(const exceptions::UnauthorizedException &e) {
        std::cerr << "Error while registering client: " << e.what() << std::endl;
        _sessionId = _sessionAttributor.registerClient(_socket.remote_endpoint());
        _connected = true;
        ConnectionResponseDto response{ .authorized = false};
        _packetHandler.sendTo(ConnectionResponseDto::opId, response, _sessionId);
    }
}

bool polymorph::network::tcp::ClientConnection::_broadcastReceivedPacket(const PacketHeader &header,
                                                                         const std::vector <std::byte> &bytes)
{
    if (!_packetHandler.packetReceived(header, bytes)) {
        _stopped = true;
        if (!_connectionPool.expired())
            _connectionPool.lock()->leave(shared_from_this());
        return false;
    }
    return true;
}

bool polymorph::network::tcp::ClientConnection::_determinePacket(const polymorph::network::PacketHeader &header,
                                                                 const std::vector<std::byte> &bytes)
{
    if (header.opId == ConnectionDto::opId) {
        _handleHandshakePacket(header, bytes);
        return true;
    } else if (header.opId == SessionTransferRequestDto::opId) {
        _handleSessionTransferPacket(header, bytes);
        return true;
    } else {
        return _broadcastReceivedPacket(header, bytes);
    }
}

bool polymorph::network::tcp::ClientConnection::isConnected()
{
    return _connected;
}

polymorph::network::SessionId polymorph::network::tcp::ClientConnection::getSessionId()
{
    return _sessionId;
}

polymorph::network::PacketId polymorph::network::tcp::ClientConnection::getPacketId()
{
    return ++_packetId;
}

void polymorph::network::tcp::ClientConnection::_handleSessionTransferPacket(const polymorph::network::PacketHeader &header,
                                                                             const std::vector<std::byte> &bytes)
{
    auto packet = SerializerTrait<Packet<SessionTransferRequestDto>>::deserialize(bytes);
    auto key = _sessionAttributor.generateUdpAuthorizationKey(packet.header.sId);
    SessionTransferResponseDto response{ .authKey = key};
    _packetHandler.sendTo(SessionTransferResponseDto::opId, response, packet.header.sId);
}
