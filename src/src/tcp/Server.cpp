/*
** EPITECH PROJECT, 2022
** Server.cpp
** File description:
** Server.cpp
*/


#include "Polymorph/Network/tcp/Server.hpp"
#include "tcp/ClientConnection.hpp"

polymorph::network::tcp::Server::Server(std::uint16_t port, polymorph::network::SessionStore &sessionStore)
    : _sessionStore(sessionStore), _acceptor(_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
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
        }
        _doAccept();
    });
}
