/*
** EPITECH PROJECT, 2022
** Client.cpp
** File description:
** Client.cpp
*/


#include "polymorph/network/tcp/Client.hpp"
#include "polymorph/network/dto/ConnectionDto.hpp"
#include "polymorph/network/dto/ConnectionResponseDto.hpp"

polymorph::network::tcp::Client::Client(std::string host, std::uint16_t port)
        : _serverEndpoint(asio::ip::make_address_v4(host), port), _socket(_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0))
{

}

void polymorph::network::tcp::Client::connect(std::function<void(bool, SessionId)> callback)
{
    _socket.async_connect(_serverEndpoint, [this, callback](std::error_code ec) {
        if (ec == asio::error::operation_aborted) {
            return;
        }
        if (ec) {
            std::cerr << "Error while connecting to server: " << ec.message() << std::endl;
            callback(false, 0);
            return;
        }

        _doReceive();

        ConnectionDto dto{ 0, 0};
        send<ConnectionDto>(ConnectionDto::opId, dto);
        _isConnecting = false;

        registerReceiveHandler<ConnectionResponseDto>(ConnectionResponseDto::opId, [this, callback](const PacketHeader &header, const ConnectionResponseDto &payload) {
            _isConnected = payload.authorized;
            callback(payload.authorized, header.sId);
            return true;
        });
    });
    _run();
}

void polymorph::network::tcp::Client::connectWithSession(polymorph::network::SessionId session,
                                                         polymorph::network::AuthorizationKey authKey, std::function<void(bool, SessionId)> callback)
{
    _socket.async_connect(_serverEndpoint, [this, callback, session, authKey](std::error_code ec) {
        if (ec == asio::error::operation_aborted) {
            return;
        }
        if (ec) {
            std::cerr << "Error while connecting to server: " << ec.message() << std::endl;
            callback(false, 0);
            return;
        }

        _doReceive();

        ConnectionDto dto{ .sessionId = session, .authKey = authKey };
        send<ConnectionDto>(ConnectionDto::opId, dto);
        _isConnecting = false;

        registerReceiveHandler<ConnectionResponseDto>(ConnectionResponseDto::opId, [this, callback](const PacketHeader &header, const ConnectionResponseDto &payload) {
            _isConnected = payload.authorized;
            callback(payload.authorized, header.sId);
            return true;
        });
    });
    _run();
}

void polymorph::network::tcp::Client::_doSend()
{
    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    auto& p = _sendQueue.front();
    _socket.async_send(asio::buffer(p.first), [this](const asio::error_code &error, std::size_t /* bytesSent */) mutable {
        if (error) {
            std::cerr << "Error while sending packet: " << error.message() << std::endl;
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

void polymorph::network::tcp::Client::_doReceive()
{
    _socket.async_receive(asio::buffer(_internalBuffer), [this](const asio::error_code &error, std::size_t bytesReceived) {
        if (error == asio::error::operation_aborted || error == asio::error::eof)
            return;
        if (error) {
            std::cerr << "Error while receiving packet: " << error.message() << std::endl;
            return;
        }
        _receiveBuffer.insert(_receiveBuffer.end(), _internalBuffer.begin(), _internalBuffer.begin() + bytesReceived);
        while (_receiveBuffer.size() > sizeof(PacketHeader) ) {
            auto header = SerializerTrait<PacketHeader>::deserialize(_receiveBuffer);
            if (_receiveBuffer.size() >= sizeof(PacketHeader) + header.pSize) {
                APacketHandler::packetReceived(header, _receiveBuffer);
                _receiveBuffer.erase(_receiveBuffer.begin(), _receiveBuffer.begin() + sizeof(PacketHeader) + header.pSize);
            } else
                break;
        }
        _doReceive();
    });
}

polymorph::network::tcp::Client::~Client()
{
    if (!_context.stopped())
        _context.stop();
}
