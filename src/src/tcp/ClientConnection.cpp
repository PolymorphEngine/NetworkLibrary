/*
** EPITECH PROJECT, 2022
** ClientConnection.cpp
** File description:
** ClientConnection.cpp
*/


#include <iostream>
#include "tcp/ClientConnection.hpp"
#include "Polymorph/Network/SerializerTrait.hpp"
#include "Polymorph/Network/dto/ConnectionDto.hpp"
#include "authorizationKey.hpp"
#include "Polymorph/Network/dto/ConnectionResponseDto.hpp"
#include "Polymorph/Network/exceptions/UnauthorizedException.hpp"


polymorph::network::tcp::ClientConnection::ClientConnection(asio::ip::tcp::socket socket, SessionStore &sessionStore, IConnectionPool &pool, IPacketHandler &packetHandler)
        : _sessionAttributor(sessionStore), _connectionPool(pool), _packetHandler(packetHandler), _stopped(false), _connected(false), _writeInProgress(false), _socket(std::move(socket))
{}

void polymorph::network::tcp::ClientConnection::start()
{
    _connectionPool.join(shared_from_this());
    _doReceive();
}

void polymorph::network::tcp::ClientConnection::send(std::vector <std::byte> data, std::function<void(const PacketHeader &, const std::vector <std::byte> &)> callback)
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
    auto p = _sendQueue.front();
    auto self(shared_from_this());
    _socket.async_send(asio::buffer(p.first), [this, self](const asio::error_code &error, std::size_t /* bytesSent */) mutable {
        if (error || _stopped) {
            _stopped = true;
            _connectionPool.leave(shared_from_this());
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
            _connectionPool.leave(shared_from_this());
            return;
        }
        _receiveBuffer.insert(_receiveBuffer.end(), _internalBuffer.begin(), _internalBuffer.begin() + bytesReceived);
        while (_receiveBuffer.size() > sizeof(PacketHeader) ) {
            auto header = SerializerTrait<PacketHeader>::deserialize(_receiveBuffer);
            if (_receiveBuffer.size() >= sizeof(PacketHeader) + header.pSize) {
                if (!_determinePacket(header, std::vector<std::byte>(_receiveBuffer.begin() + sizeof(PacketHeader), _receiveBuffer.begin() + sizeof(PacketHeader) + header.pSize)))
                    return;
                _receiveBuffer.erase(_receiveBuffer.begin(), _receiveBuffer.begin() + sizeof(PacketHeader) + header.pSize);
            } else
                break;
        }
        _doReceive();
    });
}

void polymorph::network::tcp::ClientConnection::_handleHandshakePacket(const PacketHeader &header,
                                                                       const std::vector <std::byte> &bytes)
{
    auto packet = SerializerTrait<Packet<ConnectionDto>>::deserialize(bytes);

    if (authorizationKey::areSame(packet.payload.authKey, authorizationKey::zero)) {
        _sessionId = _sessionAttributor.registerClient(_socket.remote_endpoint());
        ConnectionResponseDto response{ .authorized = true};
        // TODO change _packetHandler type to Server and call sendTo
        return;
    }

    try {
        _sessionAttributor.registerAuthoredClient(_socket.remote_endpoint(), packet.payload.sessionId, packet.payload.authKey);
        ConnectionResponseDto response{ .authorized = true};
        // TODO change _packetHandler type to Server and call sendTo
    } catch(const exceptions::UnauthorizedException &e) {
        std::cerr << "Error while registering client: " << e.what() << std::endl;
        _sessionId = _sessionAttributor.registerClient(_socket.remote_endpoint());
        ConnectionResponseDto response{ .authorized = false};
        // TODO change _packetHandler type to Server and call sendTo
    }
}

bool polymorph::network::tcp::ClientConnection::_broadcastReceivedPacket(const PacketHeader &header,
                                                                         const std::vector <std::byte> &bytes)
{
    if (!_packetHandler.packetReceived(header, bytes)) {
        _stopped = true;
        _connectionPool.leave(shared_from_this());
        return false;
    };
    return true;
}

bool polymorph::network::tcp::ClientConnection::_determinePacket(const polymorph::network::PacketHeader &header,
                                                                 const std::vector<std::byte> &bytes)
{
    if (header.opId == ConnectionDto::opId) {
        _handleHandshakePacket(header, bytes);
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
