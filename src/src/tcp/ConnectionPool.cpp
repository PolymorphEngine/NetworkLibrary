/*
** EPITECH PROJECT, 2022
** ConnectionPool.cpp
** File description:
** ConnectionPool.cpp
*/


#include <algorithm>
#include "tcp/ConnectionPool.hpp"
#include "tcp/ClientConnection.hpp"

void polymorph::network::tcp::ConnectionPool::join(std::shared_ptr<IClientConnection> connection)
{
    std::lock_guard<std::mutex> lock(_connectionsMutex);

    _connections.insert(connection);
}

void polymorph::network::tcp::ConnectionPool::leave(std::shared_ptr<IClientConnection> connection)
{
    std::lock_guard<std::mutex> lock(_connectionsMutex);

    _connections.erase(connection);
}

std::vector<std::shared_ptr<polymorph::network::tcp::IClientConnection>> polymorph::network::tcp::ConnectionPool::getConnections()
{
    std::lock_guard<std::mutex> lock(_connectionsMutex);
    std::vector<std::shared_ptr<IClientConnection>> connections;

    std::copy_if(_connections.begin(), _connections.end(), std::back_inserter(connections), [](const auto &connection) {
        return connection->isConnected();
    });
    return connections;
}

std::shared_ptr<polymorph::network::tcp::IClientConnection> polymorph::network::tcp::ConnectionPool::getConnectionBySessionId(polymorph::network::SessionId id)
{
    std::lock_guard<std::mutex> lock(_connectionsMutex);

    auto it = std::find_if(_connections.begin(), _connections.end(), [id](const auto &connection) {
        return connection->isConnected() && connection->getSessionId() == id;
    });
    return it != _connections.end() ? *it : nullptr;
}
