/*
** EPITECH PROJECT, 2022
** Server.cpp
** File description:
** Server.cpp
*/


#include "Polymorph/Network/tcp/Server.hpp"
#include "tcp/ClientConnection.hpp"

polymorph::network::tcp::Server::Server(std::uint16_t port, polymorph::network::SessionStore &sessionStore)
    : _sessionStore(sessionStore), _acceptor(_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), _connectionPool(std::make_shared<ConnectionPool>())
{

}

void polymorph::network::tcp::Server::start()
{
    _doAccept();
    _run();
}

void polymorph::network::tcp::Server::_doAccept()
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

polymorph::network::tcp::Server::~Server()
{
    if (!_context.stopped())
        _context.stop();
}

std::uint16_t polymorph::network::tcp::Server::getRunningPort() const
{
    return _acceptor.local_endpoint().port();
}
