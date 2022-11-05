/*
** EPITECH PROJECT, 2022
** ServerImpl.cpp
** File description:
** ServerImpl.cpp
*/


#include "polymorph/network/tcp/ServerImpl.hpp"
#include "tcp/ClientConnection.hpp"

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
    if (!_context.stopped())
        _context.stop();
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
    header.pSize = data.size();
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
        header.pSize = data.size();
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
