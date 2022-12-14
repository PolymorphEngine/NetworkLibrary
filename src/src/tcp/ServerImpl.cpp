/*
** EPITECH PROJECT, 2022
** ServerImpl.cpp
** File description:
** ServerImpl.cpp
*/


#include "tcp/ServerImpl.hpp"
#include "tcp/ClientConnection.hpp"
#include "polymorph/network/dto/DisconnectionDto.hpp"

std::unique_ptr<polymorph::network::tcp::Server> polymorph::network::tcp::Server::create(std::uint16_t port)
{
    return std::make_unique<ServerImpl>(port);
}

polymorph::network::tcp::ServerImpl::ServerImpl(std::uint16_t port)
    : _acceptor(_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), _connectionPool(std::make_shared<ConnectionPool>())
{

}

void polymorph::network::tcp::ServerImpl::start()
{
    _doAccept();
    _run();
}

void polymorph::network::tcp::ServerImpl::_doAccept()
{
    _acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (!ec) {
            std::make_shared<ClientConnection>(std::move(socket), _sessionStore, _connectionPool, *this)->start();
        } else if (ec != asio::error::operation_aborted) {
            std::cerr << "Error while accepting new connection: " << ec.message() << std::endl;
        }
        _doAccept();
    });
}

polymorph::network::tcp::ServerImpl::~ServerImpl()
{
    polymorph::network::DisconnectionDto dto;
    std::vector<std::future<void>> futures;
    std::vector<std::shared_ptr<std::promise<void>>> promises;

    for (auto &connection : _connectionPool->getConnections()) {
        auto promise = std::make_shared<std::promise<void>>();
        futures.push_back(promise->get_future());
        promises.emplace_back(promise);

        auto sId = connection->getSessionId();

        sendTo<polymorph::network::DisconnectionDto>(polymorph::network::DisconnectionDto::opId, dto, sId, [promise](const PacketHeader &header, const polymorph::network::DisconnectionDto &payload) {
            promise->set_value();
            return true;
        });
    }

    for (auto &future : futures) {
        future.wait_for(std::chrono::milliseconds(500));
    }
    if (!_context.stopped())
        _context.stop();
    while (isSending() || isReceiving()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void polymorph::network::tcp::ServerImpl::_sendTo(polymorph::network::OpId opId, const std::vector<std::byte> &data,
                                                  polymorph::network::SessionId sessionId,
                                                  std::function<void(const PacketHeader &,
                                                                     const std::vector<std::byte> &)> callback)
{
    auto connection = _connectionPool->getConnectionBySessionId(sessionId);

    if (connection == nullptr) {
        std::cerr << "Trying to send a packet to a non existing session" << std::endl;
        return;
    }
    auto pId = connection->getPacketId();
    PacketHeader header = {};
    header.opId = opId;
    header.pId = pId;
    header.sId = sessionId;
    header.pSize = static_cast<std::uint16_t>(data.size());
    auto serialized = SerializerTrait<PacketHeader>::serialize(header);
    serialized.insert(serialized.end(), data.begin(), data.end());
    connection->send(serialized, callback);
}

void polymorph::network::tcp::ServerImpl::_send(polymorph::network::OpId opId, const std::vector<std::byte> &data)
{
    auto connections = _connectionPool->getConnections();

    for (auto &connection : connections) {
        auto pId = connection->getPacketId();
        PacketHeader header = {};
        header.opId = opId;
        header.pId = pId;
        header.sId = connection->getSessionId();
        header.pSize = static_cast<std::uint16_t>(data.size());
        auto serialized = SerializerTrait<PacketHeader>::serialize(header);
        serialized.insert(serialized.end(), data.begin(), data.end());
        connection->send(serialized, std::function<void (const PacketHeader &, const std::vector<std::byte> &)>());
    }
}
std::uint16_t polymorph::network::tcp::ServerImpl::getRunningPort() const
{
    return _acceptor.local_endpoint().port();
}

polymorph::network::SessionStore *polymorph::network::tcp::ServerImpl::getSessionStore()
{
    return &_sessionStore;
}

void polymorph::network::tcp::ServerImpl::setSessionStore(polymorph::network::SessionStore *sessionStore)
{
    _sessionStore = SessionStore(*sessionStore);
}

void polymorph::network::tcp::ServerImpl::copyTcpSessionsFrom(polymorph::network::SessionStore *other)
{
    _sessionStore.copyTcpSessionsFrom(*other);
}

void polymorph::network::tcp::ServerImpl::copyUdpSessionsFrom(polymorph::network::SessionStore *other)
{
    _sessionStore.copyUdpSessionsFrom(*other);
}

void polymorph::network::tcp::ServerImpl::copyTcpAuthorizationKeysFrom(polymorph::network::SessionStore *other)
{
    _sessionStore.copyTcpAuthorizationKeysFrom(*other);
}

void polymorph::network::tcp::ServerImpl::copyUdpAuthorizationKeysFrom(polymorph::network::SessionStore *other)
{
    _sessionStore.copyUdpAuthorizationKeysFrom(*other);
}

polymorph::network::AuthorizationKey
polymorph::network::tcp::ServerImpl::generateUdpAuthorizationKey(polymorph::network::SessionId sessionId)
{
    return _sessionStore.generateUdpAuthorizationKey(sessionId);
}

polymorph::network::AuthorizationKey
polymorph::network::tcp::ServerImpl::generateTcpAuthorizationKey(polymorph::network::SessionId sessionId)
{
    return _sessionStore.generateTcpAuthorizationKey(sessionId);
}

void polymorph::network::tcp::ServerImpl::declareSending()
{
    _sendingCount++;
}

void polymorph::network::tcp::ServerImpl::declareSendingDone()
{
    if (_sendingCount > 0)
        _sendingCount--;
}

bool polymorph::network::tcp::ServerImpl::isSending()
{
    return _sendingCount > 0;
}

void polymorph::network::tcp::ServerImpl::declareReceiving()
{
    _receivingCount++;
}

void polymorph::network::tcp::ServerImpl::declareReceivingDone()
{
    if (_receivingCount > 0)
        _receivingCount--;
}

bool polymorph::network::tcp::ServerImpl::isReceiving()
{
    return _receivingCount > 0;
}
