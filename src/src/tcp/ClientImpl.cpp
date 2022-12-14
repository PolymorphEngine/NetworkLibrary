/*
** EPITECH PROJECT, 2022
** ClientImpl.cpp
** File description:
** ClientImpl.cpp
*/


#include "tcp/ClientImpl.hpp"
#include "polymorph/network/dto/ConnectionDto.hpp"
#include "polymorph/network/dto/ConnectionResponseDto.hpp"
#include "polymorph/network/dto/DisconnectionDto.hpp"

std::unique_ptr<polymorph::network::tcp::Client> polymorph::network::tcp::Client::create(std::string host, std::uint16_t port)
{
    return std::make_unique<ClientImpl>(host, port);
}

polymorph::network::tcp::ClientImpl::ClientImpl(std::string host, std::uint16_t port)
        : APacketHandler(), _serverEndpoint(asio::ip::make_address_v4(host), port), _socket(_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0))
{

}

void polymorph::network::tcp::ClientImpl::connect(std::function<void(bool, SessionId)> callback)
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

        registerReceiveHandler<ConnectionResponseDto>(ConnectionResponseDto::opId, [this, callback](const PacketHeader &header, const ConnectionResponseDto &payload) {
            _isConnected = payload.authorized;
            _currentSession = header.sId;
            callback(payload.authorized, header.sId);
            return true;
        });

        ConnectionDto dto{ 0, 0};
        send<ConnectionDto>(ConnectionDto::opId, dto);
        _isConnecting = false;
    });
    _run();
}

void polymorph::network::tcp::ClientImpl::connectWithSession(polymorph::network::SessionId session,
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

        registerReceiveHandler<ConnectionResponseDto>(ConnectionResponseDto::opId, [this, callback](const PacketHeader &header, const ConnectionResponseDto &payload) {
            _isConnected = payload.authorized;
            callback(payload.authorized, header.sId);
            return true;
        });


        ConnectionDto dto{ .sessionId = session, .authKey = authKey };
        send<ConnectionDto>(ConnectionDto::opId, dto);
        _isConnecting = false;
    });
    _run();
}

void polymorph::network::tcp::ClientImpl::_doSend()
{
    std::lock_guard<std::mutex> lock(_sendQueueMutex);
    auto& p = _sendQueue.front();
    _socket.async_send(asio::buffer(p.first), [this](const asio::error_code &error, std::size_t /* bytesSent */) mutable {
        if (error == asio::error::operation_aborted) {
            _writeInProgress = false;
            return;
        }
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

void polymorph::network::tcp::ClientImpl::_doReceive()
{
    _receiveInProgress = false;
    _socket.async_receive(asio::buffer(_internalBuffer), [this](const asio::error_code &error, std::size_t bytesReceived) {
        if (error == asio::error::operation_aborted || error == asio::error::eof || _stopping)
            return;
        if (error) {
            std::cerr << "Error while receiving packet: " << error.message() << std::endl;
            return;
        }
        _receiveInProgress = true;
        _receiveBuffer.insert(_receiveBuffer.end(), _internalBuffer.begin(), _internalBuffer.begin() + bytesReceived);
        while (_receiveBuffer.size() > sizeof(PacketHeader) ) {
            auto header = SerializerTrait<PacketHeader>::deserialize(_receiveBuffer);
            if (_receiveBuffer.size() >= sizeof(PacketHeader) + header.pSize) {
                APacketHandler::packetReceived(header, _receiveBuffer);
                if (header.opId == DisconnectionDto::opId) {
                    _stopping = true;
                    _receiveInProgress = false;
                    return;
                }
                _receiveBuffer.erase(_receiveBuffer.begin(), _receiveBuffer.begin() + sizeof(PacketHeader) + header.pSize);
            } else
                break;
        }
        _doReceive();
    });
}

polymorph::network::tcp::ClientImpl::~ClientImpl()
{
    if (!_isConnected)
        return;
    polymorph::network::DisconnectionDto dto;
    std::promise<void> promise;
    auto future = promise.get_future();

    send<polymorph::network::DisconnectionDto>(polymorph::network::DisconnectionDto::opId, dto, [&promise](const PacketHeader &header, const polymorph::network::DisconnectionDto &payload) {
        promise.set_value();
    });
    future.wait_for(std::chrono::seconds(1));

    if (!_context.stopped())
        _context.stop();
    _stopping = true;
    while (_writeInProgress || _receiveInProgress) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void polymorph::network::tcp::ClientImpl::_send(polymorph::network::OpId opId, const std::vector<std::byte> &data, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback)
{
    if (!_isConnecting && !_isConnected && opId != ACKDto::opId) {
        std::cerr << "Trying to send a packet before client is connected" << std::endl;
        return;
    }
    if (_stopping)
        return;
    ++_currentPacketId;
    PacketHeader header{};
    header.pId = _currentPacketId;
    header.opId = opId;
    header.sId = _currentSession;
    header.pSize = static_cast<std::uint16_t>(data.size());
    std::vector<std::byte> sPacket = SerializerTrait<PacketHeader>::serialize(header);
    sPacket.insert(sPacket.end(), data.begin(), data.end());
    std::unique_lock<std::mutex> lock(_sendQueueMutex);
    _sendQueue.push(std::make_pair(sPacket, callback));

    if (!_writeInProgress.exchange(true)) {
        lock.unlock();
        _doSend();
    }
}
