/*
** EPITECH PROJECT, 2020
** IConnectionPool.hpp
** File description:
** header for IConnectionPool.c
*/

#pragma once

#include <memory>

namespace polymorph::network::tcp {
    class ClientConnection;

    class IConnectionPool {

        public:
            virtual void join(std::shared_ptr<ClientConnection> connection) = 0;

            virtual void leave(std::shared_ptr<ClientConnection> connection) = 0;

    };

}