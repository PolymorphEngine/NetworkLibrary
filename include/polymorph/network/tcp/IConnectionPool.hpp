/*
** EPITECH PROJECT, 2020
** IConnectionPool.hpp
** File description:
** header for IConnectionPool.c
*/

#pragma once

#include <memory>
#include "polymorph/network/udp/IClientConnection.hpp"

namespace polymorph::network::tcp
{

    class ClientConnection;

    /**
     * @class   Interface for connection pools
     * @note    TCP exclusive
     */
    class IConnectionPool {

        public:
            virtual ~IConnectionPool() = default;

            /**
             * @brief   Add a connection to the pool
             *
             * @param   connection  The connection to add
             */
            virtual void join(std::shared_ptr<IClientConnection> connection) = 0;

            /**
             * @brief   Remove a connection from the pool
             *
             * @param   connection  The connection to remove
             */
            virtual void leave(std::shared_ptr<IClientConnection> connection) = 0;

    };

}