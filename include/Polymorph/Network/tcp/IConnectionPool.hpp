/*
** EPITECH PROJECT, 2020
** IConnectionPool.hpp
** File description:
** header for IConnectionPool.c
*/

#pragma once

#include <memory>
#include "Polymorph/Network/udp/IClientConnection.hpp"

namespace polymorph::network::tcp {
    class ClientConnection;

    class IConnectionPool {

        public:
            virtual ~IConnectionPool() = default;

            virtual void join(std::shared_ptr<IClientConnection> connection) = 0;

            virtual void leave(std::shared_ptr<IClientConnection> connection) = 0;

    };

}