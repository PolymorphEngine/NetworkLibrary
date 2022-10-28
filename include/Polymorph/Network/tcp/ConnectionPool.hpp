/*
** EPITECH PROJECT, 2020
** ConnectionPool.hpp
** File description:
** header for ConnectionPool.c
*/


#pragma once

#include <set>
#include <vector>
#include "Polymorph/Network/tcp/IConnectionPool.hpp"
#include "Polymorph/Network/types.hpp"

namespace polymorph::network::tcp
{

    /**
     * @class   Base class for connection pools
     * @note    TCP exclusive
     *
     * @inherit IConnectionPool    The interface for connection pools
     */
    class ConnectionPool : public IConnectionPool
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            ConnectionPool() = default;

            ~ConnectionPool() override = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
             * @property The set of the connections
             * @brief   The connections are stored in a set to avoid duplicates and fast access
             *
             * @return  std::set<Connection>    the set of the connections
             */
            std::set<std::shared_ptr<IClientConnection>> _connections;

            /**
             * @property The mutex for the connections
             */
            std::mutex _connectionsMutex;


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            void join(std::shared_ptr<IClientConnection> connection) override;

            void leave(std::shared_ptr<IClientConnection> connection) override;

            /**
             * @brief   Get the connections
             *
             * @return  std::vector<Connection>    The connections stored in the pool
             */
            std::vector<std::shared_ptr<IClientConnection>> getConnections();

            /**
             * @brief   Get the connections
             *
             * @return  std::vector<Connection>    The connections stored in the pool
             */
            std::shared_ptr<IClientConnection> getConnectionBySessionId(SessionId id);



        private:


//////////////////////--------------------------/////////////////////////

    };
}
